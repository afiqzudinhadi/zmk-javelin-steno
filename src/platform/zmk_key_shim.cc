#include "key.h"
#include "key_code.h"

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>

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
