#include "key.h"
#include "key_code.h"

// TODO Phase 2: Replace stubs with ZMK HID event emission.
// Key::Press/Release are called by Javelin's output pipeline to send
// translated text to the host. These need to map to
// raise_zmk_keycode_state_changed() with correct usage pages.

bool Key::historyEnabled = false;

void Key::Press(KeyCode key) {
  (void)key;
}

void Key::Release(KeyCode key) {
  (void)key;
}

void Key::Flush() {
}
