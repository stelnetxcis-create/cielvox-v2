// stelnet_model_mgr_cli.cpp — CLI model-resolve with TTY prompting.
//
// Delegates non-interactive resolution to the shared library; layers a
// `Download now?` prompt on top when stdin is a TTY.

#include "stelnet_model_mgr_cli.h"
#include "stelnet_cache.h"
#include "stelnet_model_registry.h"

#include <cstdio>
#include <string>

#if defined(_WIN32)
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

static bool parse_auto_quant_spec(const std::string& spec, std::string& base, std::string& quant) {
    const size_t pos = spec.find(':');
    if (pos == std::string::npos)
        return false;
    const std::string prefix = spec.substr(0, pos);
    if (prefix != "auto" && prefix != "default")
        return false;
    const std::string suffix = spec.substr(pos + 1);
    if (suffix.empty())
        return false;
    base = prefix;
    quant = suffix;
    return true;
}

static StelnetResolvePreview build_preview(const std::string& model_arg, const std::string& backend_name,
                                            const std::string& cache_dir_override, const std::string& preferred_quant,
                                            bool ignore_cache) {
    std::string effective_model_arg = model_arg;
    std::string effective_quant = preferred_quant;
    std::string auto_base;
    std::string auto_quant;
    if (effective_quant.empty() && parse_auto_quant_spec(effective_model_arg, auto_base, auto_quant)) {
        effective_model_arg = auto_base;
        effective_quant = auto_quant;
    }

    StelnetResolvePreview out;
    out.requested = model_arg;
    out.backend = backend_name;

    auto cache_path_for = [&](const std::string& filename) {
        return stelnet_cache::dir(cache_dir_override) + "/" + filename;
    };

    if (effective_model_arg != "auto" && effective_model_arg != "default") {
        FILE* f = fopen(effective_model_arg.c_str(), "rb");
        if (f) {
            fclose(f);
            out.exists_locally = true;
            out.resolved_path = effective_model_arg;
            out.filename = effective_model_arg;
            return out;
        }
    }

    StelnetRegistryEntry match;
    bool have_match = false;
    if (effective_model_arg == "auto" || effective_model_arg == "default") {
        have_match = stelnet_registry_lookup(backend_name, match, effective_quant);
    } else {
        // Mirror stelnet_resolve_model's match priority exactly, or the preview
        // lies: (1) exact filename/companion, (2) backend-key match on the
        // literal -m arg (so sub-variant keys like parakeet-tdt_ctc-110m resolve
        // to their own entry rather than being shadowed by the filename-inferred
        // backend), (3) fallback to the --backend name.
        have_match = stelnet_registry_lookup_by_filename(effective_model_arg, match, effective_quant);
        if (!have_match)
            have_match = stelnet_registry_lookup(effective_model_arg, match, effective_quant);
        if (!have_match && !backend_name.empty())
            have_match = stelnet_registry_lookup(backend_name, match, effective_quant);
    }
    if (!have_match) {
        out.unresolved = true;
        out.resolved_path = effective_model_arg;
        return out;
    }

    out.matched_registry = true;
    out.filename = match.filename;
    out.url = match.url;
    out.approx_size = match.approx_size;
    out.resolved_path = cache_path_for(match.filename);

    if (!ignore_cache) {
        const std::string found = stelnet_cache::probe_cached_file(match.filename, cache_dir_override);
        if (!found.empty()) {
            out.exists_locally = true;
            out.resolved_path = found;
            return out;
        }
    }

    out.would_download = true;
    return out;
}

StelnetResolvePreview stelnet_preview_model_cli(const std::string& model_arg, const std::string& backend_name,
                                                  const std::string& cache_dir_override,
                                                  const std::string& preferred_quant, bool ignore_cache) {
    return build_preview(model_arg, backend_name, cache_dir_override, preferred_quant, ignore_cache);
}

std::string stelnet_resolve_model_cli(const std::string& model_arg, const std::string& backend_name, bool quiet,
                                       const std::string& cache_dir_override, bool auto_download,
                                       const std::string& preferred_quant) {
    std::string effective_model_arg = model_arg;
    std::string effective_quant = preferred_quant;
    std::string auto_base;
    std::string auto_quant;
    if (effective_quant.empty() && parse_auto_quant_spec(effective_model_arg, auto_base, auto_quant)) {
        effective_model_arg = auto_base;
        effective_quant = auto_quant;
    }

    // "auto"/"default" and already-on-disk paths: library handles them.
    if (effective_model_arg == "auto" || effective_model_arg == "default") {
        return stelnet_resolve_model(effective_model_arg, backend_name, quiet, cache_dir_override, auto_download,
                                      effective_quant);
    }

    // Concrete path: check existence ourselves so we can interpose the
    // TTY prompt before asking the library to download.
    FILE* f = fopen(effective_model_arg.c_str(), "rb");
    if (f) {
        fclose(f);
        return effective_model_arg;
    }

    // File missing — see whether the registry recognises it.
    // Match priority:
    //   1. exact filename / known-companion match (e.g. -m parakeet-tdt-0.6b-v2-q4_k.gguf)
    //   2. backend-key match on the literal -m arg (e.g. -m parakeet-v2 → the parakeet-v2 entry)
    //   3. fallback: backend name passed via --backend (or inferred from filename)
    // Step 2 must precede step 3, otherwise the CLI's filename-inferred
    // backend (always "parakeet" for any "parakeet*" arg) would shadow
    // sub-variant keys like "parakeet-v2" / "parakeet-tdt-1.1b" / etc.
    StelnetRegistryEntry match;
    bool have_match = stelnet_registry_lookup_by_filename(effective_model_arg, match, effective_quant);
    if (!have_match)
        have_match = stelnet_registry_lookup(effective_model_arg, match, effective_quant);
    if (!have_match && !backend_name.empty())
        have_match = stelnet_registry_lookup(backend_name, match, effective_quant);

    if (!have_match) {
        // Nothing to download — return the arg and let the load layer
        // produce a real error.
        return effective_model_arg;
    }

    const std::string cached_path = stelnet_cache::dir(cache_dir_override) + "/" + match.filename;
    if (stelnet_cache::file_present(cached_path)) {
        if (!match.license.empty()) {
            const bool is_nc = match.license.find("NC") != std::string::npos ||
                               match.license.find("NonCommercial") != std::string::npos;
            if (is_nc)
                fprintf(stderr,
                        "stelnet: WARNING: %s is licensed %s — NON-COMMERCIAL USE ONLY.\n"
                        "  By loading this model you confirm you will not use it for commercial purposes.\n",
                        match.filename.c_str(), match.license.c_str());
        }
        return cached_path;
    }

    fprintf(stderr, "stelnet: model '%s' not found locally.\n", effective_model_arg.c_str());
    fprintf(stderr, "  Available for download: %s (%s)\n", match.filename.c_str(), match.approx_size.c_str());
    if (!match.license.empty()) {
        const bool is_nc =
            match.license.find("NC") != std::string::npos || match.license.find("NonCommercial") != std::string::npos;
        if (is_nc)
            fprintf(stderr, "  LICENSE: %s — NON-COMMERCIAL USE ONLY. Do not use for commercial purposes.\n",
                    match.license.c_str());
        else
            fprintf(stderr, "  License: %s\n", match.license.c_str());
    }

    bool do_download = false;
    if (auto_download) {
        do_download = true;
        fprintf(stderr, "  Auto-downloading (--auto-download is set)...\n");
    } else if (isatty(fileno(stdin))) {
        fprintf(stderr, "  Download now? [Y/n] ");
        fflush(stderr);
        char c = 'y';
        int ch = fgetc(stdin);
        if (ch != EOF && ch != '\n')
            c = (char)ch;
        do_download = (c == 'y' || c == 'Y' || c == '\n');
    } else {
        fprintf(stderr, "  Use --auto-download or -m auto to download automatically.\n");
    }

    if (do_download) {
        return stelnet_cache::ensure_cached_file(match.filename, match.url, quiet, "stelnet", cache_dir_override);
    }

    return effective_model_arg;
}
