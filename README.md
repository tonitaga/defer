# `defer` for C++

A lightweight, header-only C++ library that brings Go-style `defer` statements to C++. Execute cleanup code automatically at scope exit, ensuring resource safety and cleaner code.

## Features

- **Header-only**: Just include and use
- **vcpkg compatible**: Easy installation via package manager
- **Simple syntax**: Clean, intuitive API inspired by Go

## Installation

### vcpkg

```bash
vcpkg install defer
```

```cmake
find_package(defer REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE defer::defer)
```

### submodule

```cmake
add_subdirectory(third_party/defer)
target_link_libraries(${PROJECT_NAME} PRIVATE defer::defer)
```

## Usage

```cpp
{
    FILE* f = fopen("file.txt", "r");
    if (!f) return;
    defer { fclose(f); }; // This will run at the end of the scope.

    // ... work with the file ...
    // fclose(f) is called automatically here, even if an exception is thrown.
}
```
