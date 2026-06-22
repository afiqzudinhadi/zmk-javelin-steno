#include "flash.h"
#include "mem.h"

extern "C" {
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
}

// Flash partition IDs — must match DTS overlay.
// steno_dict_partition: compiled dictionary (read-only at runtime)
// steno_user_partition: user dictionary (read-write)
//
// Javelin's Flash class uses raw pointers into XIP-mapped flash.
// On nRF52840, internal flash is memory-mapped at 0x00000000.
// We convert between XIP pointers and partition offsets.

#ifndef FIXED_PARTITION_ID
#define FIXED_PARTITION_ID(label) FLASH_AREA_ID(label)
#endif

// Flash::instance is defined in javelin/flash.cc (high-level methods).
// We only provide the platform-specific EraseBlockInternal/WriteBlockInternal.

// Convert XIP address to flash device offset.
// nRF52840 internal flash is at 0x00000000, so pointer == offset.
static off_t xip_to_offset(const void *ptr) {
  return (off_t)(uintptr_t)ptr;
}

void Flash::EraseBlockInternal(const void *target, size_t size) {
  const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash));
  if (!device_is_ready(dev)) {
    return;
  }
  flash_erase(dev, xip_to_offset(target), size);
}

void Flash::WriteBlockInternal(const void *target, const void *data,
                               size_t size) {
  const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash));
  if (!device_is_ready(dev)) {
    return;
  }
  flash_write(dev, xip_to_offset(target), data, size);
}
