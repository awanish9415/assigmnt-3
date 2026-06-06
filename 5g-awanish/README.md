# 5g-awanish — L2/MAC Stack

5G L2/MAC stack practice project — built incrementally over 7 days.

## 📁 Project Structure

```
5g-awanish/
├── CMakeLists.txt          # Top-level build config
├── include/                # Public headers
│   ├── common/             # logger, timer, config (Day 3-5)
│   ├── mac/                # ue_context (Day 6), scheduler
│   ├── rlc/                # RLC layer (future)
│   └── pdcp/               # PDCP layer (future)
├── src/                    # Implementation (.cpp)
│   ├── common/
│   ├── mac/
│   ├── rlc/
│   ├── pdcp/
│   └── main.cpp            # Entry point
├── tests/                  # Google Test (Day 7)
├── config/                 # JSON configs (Day 5)
└── docs/                   # Design notes
```

## 🛠️ Prerequisites (Day 1 setup)

```bash
sudo apt install -y build-essential cmake git gdb valgrind \
                    libgtest-dev nlohmann-json3-dev
```

## 🚀 Build & Run

```bash
# Configure
cmake -S . -B build

# Build
cmake --build build -j

# Run
./build/bin/5g_awanish

# Run tests
cd build && ctest --output-on-failure
```

## 📅 7-Day Plan

| Day | Task | Status |
|-----|------|--------|
| 1   | Tools setup (gcc, cmake, gtest, etc.) | ✅ |
| 2   | Project structure + CMakeLists.txt    | ✅ |
| 3   | Logger module (file + console)        | ⬜ |
| 4   | Timer framework (event-driven queue)  | ⬜ |
| 5   | Config parser (JSON via nlohmann)     | ⬜ |
| 6   | UE Context structure                  | ⬜ |
| 7   | Unit tests (Google Test)              | ⬜ |
