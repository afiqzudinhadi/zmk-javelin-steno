#include "flash.h"
#include "mem.h"
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>

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
