// ws_stream.h — minimal RFC 6455 WebSocket server for real-time ASR streaming.
//
// Runs on a separate port (default: HTTP port + 1). Clients connect,
// send binary PCM chunks (16kHz mono float32), receive JSON text updates:
//   {"text": "...", "t0": 0.0, "t1": 1.5, "counter": 1}
//
// Uses raw POSIX/Winsock sockets. No external dependencies beyond the
// C standard library and the stelnet Session API.
//
// Usage from the HTTP server main():
//   ws_stream_start(ctx, ws_port, params);  // spawns listener thread
//   ws_stream_stop();                        // joins and cleans up

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Forward-declare Session API (exported by libstelnet)
struct StelnetSession;
struct StelnetStream;
struct StelnetSession* stelnet_session_open(const char* model_path, int n_threads);
void stelnet_session_close(struct StelnetSession* s);
struct StelnetStream* stelnet_session_stream_open(struct StelnetSession* s, int n_threads,
                                                     int step_ms, int length_ms, int keep_ms,
                                                     const char* language, int translate);
int stelnet_stream_feed(struct StelnetStream* s, const float* pcm, int n_samples);
int stelnet_stream_get_text(struct StelnetStream* s, char* out_text, int out_cap,
                              double* out_t0_s, double* out_t1_s, long long* out_counter);
int stelnet_stream_flush(struct StelnetStream* s);
void stelnet_stream_close(struct StelnetStream* s);

// Start the WebSocket listener thread on `port`. `model_path` is used
// to create per-connection stelnet sessions. Returns 0 on success.
int ws_stream_start(const char* model_path, int port, int n_threads);

// Stop the listener and join the thread. Safe to call multiple times.
void ws_stream_stop(void);

#ifdef __cplusplus
}
#endif
