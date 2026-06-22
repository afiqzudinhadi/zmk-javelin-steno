#include "flash.h"
#include "mem.h"

// Stub implementations — real flash ops deferred until flash partition configured.
// EraseBlockInternal/WriteBlockInternal are weak in javelin/flash.cc.
// Our strong definitions here override them.

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
