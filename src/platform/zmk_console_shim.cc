#include "console.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static char line_buf[256];
static size_t line_pos = 0;

void ConsoleWriter::Write(const char *data, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    if (data[i] == '\n' || line_pos >= sizeof(line_buf) - 1) {
      line_buf[line_pos] = '\0';
      if (line_pos > 0) {
        LOG_INF("[steno] %s", line_buf);
      }
      line_pos = 0;
    } else {
      line_buf[line_pos++] = data[i];
    }
  }
}
