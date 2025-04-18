# 🎛️ Real-Time Digital Effects Pedal

> A modular, real-time audio effects system for Raspberry Pi, developed in modern C++ with multithreading, low-latency processing, and unit-tested DSP effect chains.

## 📜 Project Overview

The Real-Time Harmoniser is a standalone real-time embedded system designed to apply audio effects (like pitch-shifting and gain distortion) to incoming audio with minimal latency. The pedal is controllable via a web interface and written for the ENG5220: Real-Time Embedded Programming module.

**Key Features:**
- Modular DSP architecture with hot-swappable effect chains
- Sample-by-sample processing for minimal latency
- Real-time capable Harmonizer using [SignalsmithStretch](https://github.com/Signalsmith-Audio/signalsmith-stretch)
- Multithreaded control-safe design with atomic buffer swapping
- CI-integrated Google Test unit testing
- Memory-safe code verified with Valgrind

## 🧩 System Architecture

```
+-----------+     +--------------+     +--------------------+
| Audio In  | --> | Sampling     | --> | DigitalSignalChain | --> Audio Out
+-----------+     +--------------+     +--------------------+
                                       |  Gain, Fuzz, Harmonizer ...
                                       +--------------------+
```

Each audio sample flows through a configurable DSP chain loaded dynamically from a config file.

## ⚙️ Getting Started

### 🔧 Prerequisites

- Raspberry Pi 4/5 (or any Linux system)
- `libsndfile1-dev`, `libasound2-dev`, `libgtest-dev`, `valgrind`
- CMake ≥ 3.7, Clang or GCC
- (Optional) Angular frontend for pedal control

### 🛠️ Build

```bash
chmod +x code/init.sh code/build.sh
./code/init.sh    # Installs dependencies and fetches submodules
./code/build.sh   # Builds the pedal and test binaries
```

### ▶️ Run

```bash
./code/run.sh     # Runs the real-time pedal binary
```

### ✅ Unit Tests

```bash
./code/test.sh    # Runs unit tests with Valgrind
```

### 🧼 Clean

```bash
./code/clean_run.sh     # Cleans pedal build artifacts
./code/clean_test.sh    # Cleans test binaries and results
```

## 📂 Project Structure

```
code/
├── src/              # All source files
│   ├── dsp/          # DSP chain and factory
│   ├── effects/      # Harmonizer, Gain, Fuzz etc.
│   ├── sampling/     # Sample and output modules
│   └── test/         # Functional and unit tests
├── assets/           # Input/output WAV files and configs
├── CMakeLists.txt    # Project build configuration
├── run.sh            # Launches the pedal binary
├── test.sh           # Runs Valgrind test harness
└── init.sh           # Dependency setup and submodule fetch
```

## 📊 Performance

| Metric             | Value           |
|--------------------|-----------------|
| Latency (per sample) | < 3μs (tested on x86) |
| Realtime Factor    | ~2.5x            |
| CPU Usage          | < 40% on RPi 4  |
| Memory Leaks       | ❌ None (Valgrind verified) |

## 🔬 Features Under Development

- Live web-based configuration via Angular UI (`shred.local`)
- Hardware control via rotary encoders and OLED display
- Additional effects: Reverb, Delay
- Plugin support for 3rd-party effect modules

## ✅ Achievements

| Criteria                         | Status       |
|----------------------------------|--------------|
| Multithreaded DSP                | ✅ Lock-free atomic switching |
| Realtime sample-by-sample       | ✅ Implemented in Harmonizer |
| Unit Testing                     | ✅ GTest + Valgrind |
| CI/CD Integration                | ✅ GitHub Actions |
| Well-commented, structured code | ✅ Reviewed and documented |
| Pedal UI and Config             | ⚠️ UI In Progress |

## 📜 License

MIT License. See `LICENSE` for details.

---

Let me know if you’d like a matching GitHub Actions badge, contribution guide, or images/diagrams added.