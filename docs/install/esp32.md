## Install ESP-IDF

Make sure ESP-IDF is properly installed, see to the official [documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html). Espressif provides a very clear and detailed explanation of each installation step.

## Install Library

All components are located in the `components` directory. It's a default directory name where the ESP-IDF build system looks up for the project libraries, or components in terms of the ESP-IDF SDK. See the build system [documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/build-system.html#component-cmakelists-files) for more details.

These components can be manually copied into the project `components` directory, or added as a git submodule:

```bash
git submodule add https://github.com/open-control-systems/control-components.git control-components
git submodule update --init --recursive
```

and then the project's root `CMakeLists.txt` should be updated file as follows:

```cmake
# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    "control-components/components"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(main)
```

## Configuration

The firmware can be configured using the following command:

```bash
idf.py menuconfig
```

There are many configuration options. Each of them contains the prefix "OCS_".

## Build and Flash

```bash
idf.py build
idf.py flash
```
