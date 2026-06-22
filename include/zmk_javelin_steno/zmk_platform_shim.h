#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void zmk_javelin_steno_init(void);
void zmk_javelin_steno_process_key(int steno_key_index, bool is_press);

extern const uint8_t _javelin_dict_start[];
extern const uint8_t _javelin_dict_end[];

#ifdef __cplusplus
}

class StenoDictionary;
struct StenoOrthography;
#endif
