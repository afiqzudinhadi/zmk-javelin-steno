/* Embed compiled steno dictionary binary into firmware.
 * JAVELIN_DICT_BIN_PATH is set by CMake to the path of the compiled
 * dictionary file. If not set, a 4-byte null placeholder is used.
 */

.section .rodata.steno_dict,"a",%progbits
.global _javelin_dict_start
.global _javelin_dict_end
.balign 4

_javelin_dict_start:
#ifdef JAVELIN_DICT_BIN_PATH
.incbin JAVELIN_DICT_BIN_PATH
#else
.byte 0x00, 0x00, 0x00, 0x00
#endif
_javelin_dict_end:
