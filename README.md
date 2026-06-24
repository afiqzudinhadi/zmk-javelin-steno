# zmk-javelin-steno

ZMK module that integrates the [Javelin steno engine](https://github.com/jthlim/javelin-steno) for on-board stenography on ZMK keyboards.

## Status

Work in progress. Engine compiles and initializes. Dictionary auto-downloads and compiles during build. Requires sufficient flash for the compiled dictionary (~3-4MB for full Plover/Lapwing). Currently focused on nRF52840 boards with 1MB flash (nice_nano_v2). Not yet tested on boards with more flash or external QSPI.

## Requirements

- ZMK v0.3.0
- nRF52840-based board (nice_nano_v2, etc.)
- Split keyboard: steno runs on central half only

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
CONFIG_CPLUSPLUS=y
CONFIG_LIB_CPLUSPLUS=y
CONFIG_HEAP_MEM_POOL_SIZE=32768
```

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

## Known Issues

- Plover and Lapwing dictionaries exceed nRF52840 flash (1MB). Dictionary embedding works but linker rejects due to flash overflow.
- Boards with external QSPI flash (like the original Javelin hardware) would not have this limitation.

## License

This module is a bridge layer. The Javelin steno engine itself is licensed under [PolyForm Noncommercial 1.0.0](https://polyformproject.org/licenses/noncommercial/1.0.0/).
