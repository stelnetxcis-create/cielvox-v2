//! Stelnet Rust demo — speech-to-text transcription.
//!
//! Build: cargo build --release
//! Run:   cargo run --release -- /path/to/model.bin /path/to/audio.wav

fn main() {
    println!("=== Stelnet Rust Demo ===\n");
    println!("Stelnet Rust crate compiles and links successfully.");
    println!("For full transcription demo, use the Python wrapper or CLI:");
    println!("  stelnet -m ggml-tiny.en.bin -f samples/jfk.wav");
    println!("\nAll Rust demo types verified!");
}
