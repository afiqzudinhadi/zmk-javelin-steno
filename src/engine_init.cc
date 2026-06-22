#include "zmk_javelin_steno/zmk_platform_shim.h"

#include "dictionary/dictionary_definition.h"
#include "dictionary/dictionary_list.h"
#include "dictionary/invalid_dictionary.h"
#include "engine.h"
#include "orthography.h"
#include "processor/all_up.h"
#include "processor/first_up.h"
#include "processor/jeff_modifiers.h"
#include "processor/processor.h"
#include "processor/repeat.h"
#include "steno_key_state.h"
#include "static_allocate.h"
#include "stroke.h"

// Pipeline: StenoProcessor → StenoRepeat → StenoAllUp → StenoJeffModifiers → StenoEngine
//
// StenoProcessor converts raw StenoKey press/release → StenoKeyState.
// StenoRepeat handles stroke repetition on held keys.
// StenoAllUp triggers when all keys released (standard steno behavior).
// StenoJeffModifiers handles modifier key combos within steno.
// StenoEngine does dictionary lookup → text output via Key::Press/Release.

static JavelinStaticAllocate<StenoCompiledOrthography> compiledOrthography;
static JavelinStaticAllocate<StenoJeffModifiers> jeffModifiers;
static JavelinStaticAllocate<StenoRepeat> repeat;
static JavelinStaticAllocate<StenoAllUp> allUp;
static StenoProcessor *processor = nullptr;

static bool initialized = false;

extern "C" {

void zmk_javelin_steno_init(void) {
  if (initialized) {
    return;
  }

  // Use empty orthography until a dictionary collection is loaded (Phase 3).
  // The empty orthography has no rules — suffix folding won't work, but
  // basic dictionary lookups will function once a dict is in flash.
  new (compiledOrthography)
      StenoCompiledOrthography(StenoOrthography::emptyOrthography);

  // TODO Phase 3: Load StenoDictionaryCollection from flash partition.
  // 1. Get pointer to steno_dict_partition start address
  // 2. Validate magic == 0x3443534a ('JSC4')
  // 3. Call collection->AddDictionariesToList() to populate dict list
  // 4. Construct engine with real dictionary
  //
  // For now, engine construction is deferred until dictionary is available.
  // The processor pipeline is NOT built yet — process_key will early-return.

  initialized = true;
}

void zmk_javelin_steno_process_key(int steno_key_index, bool is_press) {
  if (!processor) {
    return;
  }
  processor->Process(static_cast<StenoKey>(steno_key_index), is_press);
}

} // extern "C"

// Called once dictionary is loaded (Phase 3) to complete initialization.
void zmk_javelin_steno_init_engine(StenoDictionary &dictionary,
                                   const StenoOrthography &orthography) {
  new (compiledOrthography) StenoCompiledOrthography(orthography);

  new (StenoEngine::container)
      StenoEngine(dictionary, nullptr, compiledOrthography.value);

  new (jeffModifiers) StenoJeffModifiers(StenoEngine::container.value);
  new (allUp) StenoAllUp(jeffModifiers.value);
  new (repeat) StenoRepeat(allUp.value);

  static StenoProcessor processorInstance(repeat.value);
  processor = &processorInstance;
}
