#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void zmk_javelin_steno_init(void);
void zmk_javelin_steno_process_key(int steno_key_index, bool is_press);

#ifdef __cplusplus
}
#endif
