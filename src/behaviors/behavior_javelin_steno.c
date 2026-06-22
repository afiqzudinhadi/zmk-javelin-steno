#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>

#include "zmk_javelin_steno/zmk_platform_shim.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct behavior_javelin_steno_config {};
struct behavior_javelin_steno_data {};

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    int steno_key = binding->param1;
    zmk_javelin_steno_process_key(steno_key, true);
    return 0;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    int steno_key = binding->param1;
    zmk_javelin_steno_process_key(steno_key, false);
    return 0;
}

static int behavior_javelin_steno_init(const struct device *dev) {
    zmk_javelin_steno_init();
    return 0;
}

static const struct behavior_driver_api behavior_javelin_steno_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
};

static struct behavior_javelin_steno_data behavior_javelin_steno_data;
static struct behavior_javelin_steno_config behavior_javelin_steno_config;

BEHAVIOR_DT_INST_DEFINE(0, behavior_javelin_steno_init, NULL,
                        &behavior_javelin_steno_data,
                        &behavior_javelin_steno_config,
                        POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_javelin_steno_driver_api);
