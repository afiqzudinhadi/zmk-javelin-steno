#include "zmk_javelin_steno/zmk_platform_shim.h"

#include "engine.h"
#include "processor/all_up.h"
#include "processor/processor.h"
#include "steno_key_state.h"

// Static storage for the processor pipeline.
// Pipeline: StenoProcessor → StenoAllUp → StenoEngine
static StenoAllUp *allUp = nullptr;
static StenoProcessor *processor = nullptr;

extern "C" {

void zmk_javelin_steno_init(void) {
  // TODO Phase 2: Initialize with real dictionary and orthography.
  // For now this is a skeleton — engine construction requires a
  // StenoDictionaryCollection loaded from flash (Phase 3).
  //
  // Full init will look like:
  //   1. Read dictionary collection from flash partition
  //   2. Validate magic (0x3443534a = 'JSC4')
  //   3. Build dictionary list from collection
  //   4. Load compiled orthography
  //   5. Construct StenoEngine with dict + ortho
  //   6. Build processor pipeline: AllUp → Engine
  //   7. Wrap in StenoProcessor for key input
}

void zmk_javelin_steno_process_key(int steno_key_index, bool is_press) {
  if (!processor) {
    return;
  }
  StenoKey key = static_cast<StenoKey>(steno_key_index);
  processor->Process(key, is_press);
}

} // extern "C"
