#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void zmk_javelin_steno_init(void);
void zmk_javelin_steno_process_key(int steno_key_index, bool is_press);

#ifdef __cplusplus
}

class StenoDictionary;
struct StenoOrthography;

// Complete engine initialization with loaded dictionary.
// Called from dictionary loading code once flash data is validated.
void zmk_javelin_steno_init_engine(StenoDictionary &dictionary,
                                   const StenoOrthography &orthography);
#endif
