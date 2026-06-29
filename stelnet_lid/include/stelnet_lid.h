// stelnet_lid.h — shared text-based language identification library.
//
// Two backends auto-detected from GGUF architecture:
//   lid-fasttext — GlotLID / LID-176 (fastText supervised)
//   lid-cld3     — Google CLD3 (compact, Apache-2.0)
//
// Used by Stelnet (post-ASR LID) and CrispEmbed (OCR language routing).

#ifndef STELNET_LID_H
#define STELNET_LID_H

// Re-export the dispatch API — this is the public interface.
#include "../src/text_lid_dispatch.h"

#endif // STELNET_LID_H
