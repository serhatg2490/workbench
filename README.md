# workbench

[![CI](https://github.com/<user>/workbench/actions/workflows/ci.yml/badge.svg)](https://github.com/<user>/workbench/actions/workflows/ci.yml)

Small, self-contained re-implementations of C++ standard library components,
written from scratch in modern C++ (C++20) to understand how they work and why
they are designed the way they are.

Each module is a single header with its own test suite.

## Modules

| Module | Header | Standard counterpart |
|---|---|---|---|
| `basic_string_view` | [`workbench/string_view.hpp`](include/workbench/string_view.hpp) | `std::basic_string_view` |

## Requirements

- A C++20 compiler (CI builds with GCC, Clang and MSVC)
- CMake 3.21 or newer
- GoogleTest is downloaded automatically when the tests are built

## Building and running the tests

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Using a module

The library is header-only. Add it to your CMake project and link the
`workbench::workbench` target:

```cmake
add_subdirectory(workbench)          # or FetchContent
target_link_libraries(my_app PRIVATE workbench::workbench)
```

```cpp
#include <workbench/string_view.hpp>

constexpr workbench::string_view hello = "Hello";
static_assert(hello.size() == 5);
```

When workbench is added to another project, its tests are not built unless
`WORKBENCH_BUILD_TESTS` is turned on.

## Project layout

```
include/workbench/   one header per module
tests/               one GoogleTest executable per module
docs/                design notes, one per module
```

## Adding a new module

1. Add the header as `include/workbench/<module>.hpp`, inside `namespace workbench`.
2. Add the tests as `tests/<module>_test.cpp`.
3. Register the tests by adding `workbench_add_test(<module>)` to `tests/CMakeLists.txt`.
4. Add a row to the module table above.
