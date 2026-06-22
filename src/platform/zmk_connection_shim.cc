#include "hal/connection.h"
#include "hal/ble.h"
#include "hal/usb_status.h"

// Connection::IsConnected and friends are already implemented in
// hal/connection.cc using the weak Ble:: and UsbStatus:: defaults.
// This file exists for future ZMK-specific connection status overrides.

// TODO Phase 2: Optionally override Ble::IsConnected() with
// zmk_ble_active_profile_is_connected() for accurate BLE status.
