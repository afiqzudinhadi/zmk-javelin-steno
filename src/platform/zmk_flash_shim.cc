#include "flash.h"

// TODO Phase 3: Replace with Zephyr flash API.
// #include <zephyr/drivers/flash.h>
// #include <zephyr/storage/flash_map.h>

Flash Flash::instance;

void Flash::EraseBlockInternal(const void *target, size_t size) {
  (void)target;
  (void)size;
}

void Flash::WriteBlockInternal(const void *target, const void *data,
                               size_t size) {
  (void)target;
  (void)data;
  (void)size;
}
