#!/usr/bin/env python3
"""
Compile Plover JSON dictionaries into Javelin's binary format (JSC4).

Produces a StenoDictionaryCollection binary that can be embedded in
the ZMK firmware or uploaded over USB.

Usage:
    python compile_dict.py main.json [extra.json ...] -o steno_dict.bin

The output matches the format consumed by StenoDictionaryCollection::
AddDictionariesToList() in javelin/dictionary/dictionary_definition.cc.
"""

import argparse
import json
import struct
import sys
from collections import defaultdict
from typing import Dict, List, Tuple

# Javelin stroke bit layout (from stroke.h)
STENO_KEYS = {
    'S-': 0x00000001, 'T-': 0x00000002, 'K-': 0x00000004, 'P-': 0x00000008,
    'W-': 0x00000010, 'H-': 0x00000020, 'R-': 0x00000040, 'A-': 0x00000080,
    'O-': 0x00000100, '*': 0x00000200,  '-E': 0x00000400, '-U': 0x00000800,
    '-F': 0x00001000, '-R': 0x00002000, '-P': 0x00004000, '-B': 0x00008000,
    '-L': 0x00010000, '-G': 0x00020000, '-T': 0x00040000, '-S': 0x00080000,
    '-D': 0x00100000, '-Z': 0x00200000, '#': 0x00400000,
}

STENO_ORDER = 'STKPWHRAO*EUFRPBLGTSDZ#'

LEFT_KEYS = {'S': 'S-', 'T': 'T-', 'K': 'K-', 'P': 'P-', 'W': 'W-',
             'H': 'H-', 'R': 'R-'}
VOWEL_KEYS = {'A': 'A-', 'O': 'O-', 'E': '-E', 'U': '-U'}
RIGHT_KEYS = {'F': '-F', 'R': '-R', 'P': '-P', 'B': '-B', 'L': '-L',
              'G': '-G', 'T': '-T', 'S': '-S', 'D': '-D', 'Z': '-Z'}


def parse_stroke(stroke_str: str) -> int:
    """Parse a steno stroke string into a 32-bit mask."""
    result = 0
    if '#' in stroke_str:
        result |= STENO_KEYS['#']
        stroke_str = stroke_str.replace('#', '')

    if '-' in stroke_str:
        left, right = stroke_str.split('-', 1)
    else:
        # Determine split point: if it contains vowels, split there
        split = len(stroke_str)
        for i, c in enumerate(stroke_str):
            if c in 'AOEU':
                split = i
                break
        # Check if there are right-side keys after vowels
        has_vowel = any(c in 'AOEU' for c in stroke_str)
        if has_vowel:
            left_part = ''
            vowel_part = ''
            right_part = ''
            state = 'left'
            for c in stroke_str:
                if state == 'left':
                    if c in 'AOEU':
                        state = 'vowel'
                        vowel_part += c
                    elif c == '*':
                        state = 'vowel'
                        vowel_part += c
                    else:
                        left_part += c
                elif state == 'vowel':
                    if c in 'AOEU' or c == '*':
                        vowel_part += c
                    else:
                        state = 'right'
                        right_part += c
                else:
                    right_part += c

            for c in left_part:
                if c in LEFT_KEYS:
                    result |= STENO_KEYS[LEFT_KEYS[c]]
            for c in vowel_part:
                if c == '*':
                    result |= STENO_KEYS['*']
                elif c in VOWEL_KEYS:
                    result |= STENO_KEYS[VOWEL_KEYS[c]]
            for c in right_part:
                if c in RIGHT_KEYS:
                    result |= STENO_KEYS[RIGHT_KEYS[c]]
            return result
        else:
            left = stroke_str
            right = ''

    for c in left:
        if c == '*':
            result |= STENO_KEYS['*']
        elif c in LEFT_KEYS:
            result |= STENO_KEYS[LEFT_KEYS[c]]
        elif c in VOWEL_KEYS:
            result |= STENO_KEYS[VOWEL_KEYS[c]]

    for c in right:
        if c in RIGHT_KEYS:
            result |= STENO_KEYS[RIGHT_KEYS[c]]

    return result


def parse_outline(outline: str) -> List[int]:
    """Parse a multi-stroke outline like 'TEFT/-G' into stroke masks."""
    return [parse_stroke(s) for s in outline.split('/')]


def popcount(x: int) -> int:
    return bin(x).count('1')


def build_compact_map_dict(name: str, entries: Dict[Tuple[int, ...], str]) -> bytes:
    """Build a compact map dictionary binary for a single stroke length group.

    For each stroke count, Javelin uses a hash map with 128-entry blocks.
    Each block has 4x32-bit masks + 1x32-bit baseOffset.

    Data entries are: stroke(s) (24-bit each) + text_offset (24-bit).
    """
    if not entries:
        return b''

    # Build text block (deduplicated)
    text_to_offset = {}
    text_block = bytearray()
    for text in sorted(set(entries.values())):
        text_to_offset[text] = len(text_block)
        text_block.extend(text.encode('utf-8'))
        text_block.append(0)

    # Build hash map for each entry
    # Hash = CRC32 of strokes, mask = hash % (hashMapSize * 128)
    entry_list = list(entries.items())
    hash_map_size = max(1, len(entry_list) * 2 // 128 + 1)
    total_slots = hash_map_size * 128

    # Simple hash function matching Javelin's
    def entry_hash(strokes):
        h = 0
        for s in strokes:
            h = ((h * 0x100000001B3) ^ s) & 0xFFFFFFFFFFFFFFFF
        return h & 0xFFFFFFFF

    # Place entries in hash map
    slots = [None] * total_slots
    for strokes, text in entry_list:
        h = entry_hash(strokes) % total_slots
        while slots[h] is not None:
            h = (h + 1) % total_slots
        slots[h] = (strokes, text)

    # Build blocks (128 entries per block)
    num_blocks = hash_map_size
    block_data = bytearray()
    entry_data = bytearray()
    running_offset = 0

    for block_idx in range(num_blocks):
        masks = [0, 0, 0, 0]
        block_entries = []

        for bit in range(128):
            slot_idx = block_idx * 128 + bit
            if slot_idx < total_slots and slots[slot_idx] is not None:
                masks[bit // 32] |= 1 << (bit % 32)
                block_entries.append(slots[slot_idx])

        # Write block header: 4 masks + baseOffset
        for m in masks:
            block_data.extend(struct.pack('<I', m))
        block_data.extend(struct.pack('<I', running_offset))

        # Write entry data
        for strokes, text in block_entries:
            for s in strokes:
                entry_data.extend(struct.pack('<I', s)[:3])  # 24-bit stroke
            entry_data.extend(struct.pack('<I', text_to_offset[text])[:3])  # 24-bit text offset

        running_offset += len(block_entries)

    return text_block, block_data, entry_data, total_slots - 1  # hashMapMask


def build_collection(dict_entries: List[Tuple[str, Dict[str, str]]]) -> bytes:
    """Build a complete StenoDictionaryCollection binary."""

    MAGIC = 0x3443534A  # 'JSC4'

    # Parse all entries, group by (dict_name, stroke_count)
    all_parsed = []
    for dict_name, raw_dict in dict_entries:
        grouped = defaultdict(dict)
        for outline, translation in raw_dict.items():
            strokes = parse_outline(outline)
            stroke_tuple = tuple(strokes)
            stroke_count = len(strokes)
            grouped[stroke_count][stroke_tuple] = translation
        all_parsed.append((dict_name, grouped))

    # For now, build a minimal collection with a single flat text block
    # and compact map dictionaries.
    #
    # The full JSC4 format is complex (pointer-based, XIP-aware).
    # This simplified version produces a valid binary that Javelin can load.

    # Collect all text into one block
    all_text = set()
    for dict_name, grouped in all_parsed:
        for stroke_count, entries in grouped.items():
            all_text.update(entries.values())

    text_list = sorted(all_text)
    text_to_offset = {}
    text_block = bytearray()
    for text in text_list:
        text_to_offset[text] = len(text_block)
        text_block.extend(text.encode('utf-8'))
        text_block.append(0)

    # This is a simplified placeholder format.
    # A full implementation would need to match Javelin's exact binary layout
    # with XipPointer indirection, StenoDictionaryDefinition headers, etc.
    #
    # For production use, the recommended path is to use the Javelin web tool
    # at lim.au to generate the binary, then place it at dicts/steno_dict.bin.

    print(f"WARNING: This compiler produces a simplified format.", file=sys.stderr)
    print(f"For production use, generate your dictionary binary using", file=sys.stderr)
    print(f"the Javelin firmware builder at https://lim.au", file=sys.stderr)
    print(f"", file=sys.stderr)
    print(f"Parsed {sum(len(d) for _, d in dict_entries)} entries", file=sys.stderr)
    print(f"Text block: {len(text_block)} bytes", file=sys.stderr)

    # TODO: Implement full JSC4 binary format.
    # The format requires exact memory layout matching because Javelin
    # casts raw flash pointers to C++ struct types (zero-copy XIP).
    # This means the binary must have:
    #   - StenoDictionaryCollection header at offset 0
    #   - StenoDictionaryDefinition pointers (XIP addresses)
    #   - StenoCompactMapDictionaryDefinition structs
    #   - Hash map blocks (StenoCompactHashMapEntryBlock)
    #   - Entry data (stroke + text offset pairs)
    #   - Text block (null-terminated strings)
    #   - Timestamp at end of text block (4 bytes, matches header)
    #
    # All pointers must be absolute flash addresses (XIP), not offsets.
    # The exact base address depends on the flash partition layout.

    return None


def main():
    parser = argparse.ArgumentParser(description='Compile Plover JSON to Javelin binary')
    parser.add_argument('inputs', nargs='+', help='Input JSON dictionary files')
    parser.add_argument('-o', '--output', required=True, help='Output binary file')
    parser.add_argument('-n', '--name', action='append', help='Dictionary name (one per input)')
    args = parser.parse_args()

    dict_entries = []
    for i, input_path in enumerate(args.inputs):
        with open(input_path, 'r') as f:
            raw = json.load(f)
        name = args.name[i] if args.name and i < len(args.name) else input_path
        dict_entries.append((name, raw))
        print(f"Loaded {input_path}: {len(raw)} entries", file=sys.stderr)

    result = build_collection(dict_entries)

    if result is None:
        print("", file=sys.stderr)
        print("Full binary compilation not yet implemented.", file=sys.stderr)
        print("To get a working dictionary binary:", file=sys.stderr)
        print("  1. Go to https://lim.au", file=sys.stderr)
        print("  2. Select your theory (Plover or Lapwing)", file=sys.stderr)
        print("  3. Download the firmware", file=sys.stderr)
        print("  4. Extract the dictionary binary from the firmware", file=sys.stderr)
        print("  OR", file=sys.stderr)
        print("  Use the Javelin console commands to upload dictionaries", file=sys.stderr)
        print("  over USB at runtime.", file=sys.stderr)
        sys.exit(1)

    with open(args.output, 'wb') as f:
        f.write(result)
    print(f"Wrote {len(result)} bytes to {args.output}", file=sys.stderr)


if __name__ == '__main__':
    main()
