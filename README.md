# UI Framework Library

A flexible and extensible UI framework for game development using SDL2.

## Features

- Component-based UI system
- Container hierarchy support
- Focus management
- Button and interactive elements
- Tooltip system
- Inventory UI components
- Crafting panel UI

## Dependencies

- SDL2
- SDL2_ttf

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

See `examples/basic_ui_example.cpp` for basic usage.

## Integration

To integrate this library into your project:

1. Copy the library to your project
2. Add to your CMakeLists.txt:
   ```cmake
   add_subdirectory(ui_framework)
   target_link_libraries(your_target UIFramework)
   ```

