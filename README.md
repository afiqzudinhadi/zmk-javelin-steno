# zmk-javelin-steno

ZMK module that integrates the [Javelin steno engine](https://github.com/jthlim/javelin-steno) for on-board stenography on ZMK keyboards.

## Status

Dropped — my hardware (nice_nano_v2, 1MB flash) can't fit any dictionary. Build passes on boards with sufficient flash but has not been hardware tested. Contributions welcome.

## Flash Requirements

Total flash usage (firmware + engine + dict) on a corne split (central half):

| Dictionary | Total flash |
|------------|------------|
| Plover + Lapwing | 3.78 MB |
| Plover only | 2.53 MB |
| Lapwing only | 2.08 MB |
| No dict (engine only) | 534 KB |

## Making It Work on 1MB Flash

If you want to use this on boards with limited flash (nice_nano_v2, etc.), some options:

- **Optimize dictionary format** — Javelin uses JSC4 hash maps (~2-3MB compiled). A trie/DAWG-based format could reduce this significantly.
- **Reduce dictionary size** — Trim to most common entries. ~120K entries fits ~458KB, ~100K entries fits ~384KB.
- **Add external QSPI flash** — Boards with external flash (W25Q128, etc.) can store the full dictionary. The engine already supports XIP reads via `XipPointer`.

## Setup

Add to your `west.yml`:

```yaml
manifest:
  remotes:
    - name: afiqzudinhadi
      url-base: https://github.com/afiqzudinhadi
    - name: jthlim
      url-base: https://github.com/jthlim
  projects:
    - name: zmk-javelin-steno
      remote: afiqzudinhadi
      revision: main
    - name: javelin-steno
      remote: jthlim
      revision: main
      path: zmk-javelin-steno/javelin
```

Add to your `.conf`:

```ini
CONFIG_ZMK_JAVELIN_STENO=y
CONFIG_ZMK_JAVELIN_STENO_DICT_PLOVER=y
CONFIG_ZMK_JAVELIN_STENO_DICT_LAPWING=y
CONFIG_CPLUSPLUS=y
CONFIG_LIB_CPLUSPLUS=y
CONFIG_HEAP_MEM_POOL_SIZE=32768
```

Set `CONFIG_ZMK_JAVELIN_STENO_DICT_PLOVER` and `CONFIG_ZMK_JAVELIN_STENO_DICT_LAPWING` to choose which dictionaries to include. Both can be enabled simultaneously.

Add to your `.keymap` (see [steno_keys.h](include/dt-bindings/zmk/javelin_steno.h) for all available key bindings):

```dts
#include <dt-bindings/zmk/javelin_steno.h>
#include <behaviors/javelin_steno.dtsi>

steno_layer {
    bindings = <
        &javsteno STENO_S1  &javsteno STENO_TL  &javsteno STENO_PL ...
    >;
};
```

## License

This module is a bridge layer. The Javelin steno engine itself is licensed under [PolyForm Noncommercial 1.0.0](https://polyformproject.org/licenses/noncommercial/1.0.0/).
