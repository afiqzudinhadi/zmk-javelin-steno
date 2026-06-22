#include "console.h"

// ConsoleWriter::Write is the main output path for Javelin's console system.
// For Phase 1, stub it out. Phase 4 will route to USB CDC or ZMK logging.

void ConsoleWriter::Write(const char *data, size_t length) {
  (void)data;
  (void)length;
}
