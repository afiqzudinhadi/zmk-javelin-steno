#!/usr/bin/env bash
# Download Plover and Lapwing steno dictionaries from their official repos.
# Usage: ./download_dicts.sh [plover|lapwing|all] [output_dir]

set -euo pipefail

THEORY="${1:-all}"
OUTDIR="${2:-./dicts_src}"

mkdir -p "$OUTDIR"

download_plover() {
  echo "Downloading Plover main.json..."
  curl -fsSL \
    "https://raw.githubusercontent.com/openstenoproject/plover/main/plover/assets/main.json" \
    -o "$OUTDIR/plover-main.json"
  echo "  $(python3 -c "import json; print(len(json.load(open('$OUTDIR/plover-main.json'))))" 2>/dev/null || echo '?') entries"
}

download_lapwing() {
  local BASE="https://raw.githubusercontent.com/aerickt/plover-lapwing-aio/main/plover_lapwing/dictionaries"
  echo "Downloading Lapwing dictionaries..."
  for f in lapwing-base lapwing-commands lapwing-numbers; do
    curl -fsSL "$BASE/$f.json" -o "$OUTDIR/$f.json"
    echo "  $f.json: $(python3 -c "import json; print(len(json.load(open('$OUTDIR/$f.json'))))" 2>/dev/null || echo '?') entries"
  done
}

case "$THEORY" in
  plover)  download_plover ;;
  lapwing) download_lapwing ;;
  all)     download_plover; download_lapwing ;;
  *)       echo "Usage: $0 [plover|lapwing|all] [output_dir]"; exit 1 ;;
esac

echo "Done. Dictionaries saved to $OUTDIR/"
