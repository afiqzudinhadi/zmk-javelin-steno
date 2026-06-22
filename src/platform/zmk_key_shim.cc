#include "key.h"
#include "key_code.h"

#include <zephyr/kernel.h>

// ZMK event headers need C linkage for the function declarations
// but Zephyr's own headers are C++-safe.
extern "C" {
struct zmk_keycode_state_changed {
  uint16_t usage_page;
  uint32_t keycode;
  uint8_t implicit_modifiers;
  uint8_t explicit_modifiers;
  bool state;
  int64_t timestamp;
};
int raise_zmk_keycode_state_changed(struct zmk_keycode_state_changed ev);
}

static constexpr uint16_t HID_USAGE_PAGE_KEYBOARD = 0x07;
static constexpr uint16_t HID_USAGE_PAGE_CONSUMER = 0x0C;

static void emit_keycode(KeyCode key, bool pressed) {
  uint32_t v = key.value;
  uint16_t usage_page;
  uint32_t keycode;

  if (v >= 0x10000) {
    usage_page = HID_USAGE_PAGE_CONSUMER;
    keycode = v & 0xFFFF;
  } else {
    usage_page = HID_USAGE_PAGE_KEYBOARD;
    keycode = v;
  }

  raise_zmk_keycode_state_changed(
      (struct zmk_keycode_state_changed){
          .usage_page = usage_page,
          .keycode = keycode,
          .implicit_modifiers = 0,
          .explicit_modifiers = 0,
          .state = pressed,
          .timestamp = k_uptime_get(),
      });
}

bool Key::historyEnabled = false;

void Key::Press(KeyCode key) { emit_keycode(key, true); }

void Key::Release(KeyCode key) { emit_keycode(key, false); }

void Key::Flush() {}
