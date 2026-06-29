#ifndef STELNET_PUNC_LOADER_H
#define STELNET_PUNC_LOADER_H

// CLI-layer alias for the shared `--punc-model` resolver. The pure resolution
// table now lives in src/stelnet_punc_model.h so the C-ABI session layer
// (src/stelnet_c_api.cpp) can share it too; this header just re-exports it for
// the CLI one-shot path (stelnet_run.cpp) and the HTTP server
// (stelnet_server.cpp), which include it by this name.
#include "stelnet_punc_model.h"

#endif // STELNET_PUNC_LOADER_H
