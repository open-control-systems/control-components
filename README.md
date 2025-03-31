## Introduction

`control-components` is a C++ library with basic building blocks for embedded and IoT development. It provides high-level abstractions for embedded and IoT applications: state machines, task schedulers, resource management, various system counters, analog and digital sensors, and many other things that the embedded developer has to implement from scratch for each new embedded system. The library is written in `C++17` with some features from `C++20`.

## Motivation

The main question is why to introduce yet another library for the embedded development. There are so many already. I need a simple, but robust C++ library that works well with ESP32. ESP32 with its ecosystem is good enough for the variety of projects. It's a good starting point. If for some reason ESP32 isn't good enough, it can be replaced by STM32 or any other MCU.

## Supported Platforms

The current focus is on the low-cost solutions based on the ESP32 MCUs. Other platforms will be added later. Most of the codebase is platform independent. Please refer to the list of installation instructions for the required platform:

- ESP32 [instructions](docs/install/esp32.md)

## Contribution

- `master` - stable and ready-to-use branch. [Semver](https://semver.org/) is used for versioning.
- Try to keep PR small.
- New code should be similar to existing code. Use the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).
- Ensure the code is properly formatted and includes the licence header. Use [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) and [`verify_license.py`](tools/scripts/verify_license.py).

## Build Status

- [![ESP32](https://github.com/open-control-systems/esp-components/actions/workflows/esp32.yml/badge.svg)](https://github.com/open-control-systems/esp-components/actions/workflows/esp32.yml)

## License

This project is licensed under the MPL 2.0 License - see the LICENSE file for details.
