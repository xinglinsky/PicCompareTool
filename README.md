# PicCompareTool

A cross-platform image comparison tool that allows you to compare images side by side or in overlay mode.

## Features

- Load images from:
  - Local files
  - Clipboard
  - URLs (web images)
- Two comparison modes:
  - Side by side (split view)
  - Overlay mode
- Image navigation:
  - Zoom in/out with mouse wheel
  - Pan by dragging with the mouse

## Requirements

- Qt 5.15.2 or later
- CMake 3.14 or later
- C++17 compatible compiler

## Building from Source

### Windows

1. Install Qt 5.15.2 and CMake
2. Open a command prompt and navigate to the project directory
3. Create a build directory and configure the project:

```
mkdir build
cd build
cmake ..
```

4. Build the project:

```
cmake --build . --config Release
```

5. The executable will be in the `build\Release` directory

### Linux

1. Install Qt 5.15.2 and CMake
2. Open a terminal and navigate to the project directory
3. Create a build directory and configure the project:

```
mkdir build
cd build
cmake ..
```

4. Build the project:

```
make
```

5. Run the application:

```
./PicCompareTool
```

## Usage

1. Load images using the toolbar buttons or menu options
2. Use the comparison mode dropdown to switch between side-by-side and overlay modes
3. Navigate the images:
   - Zoom in/out with the mouse wheel
   - Pan by dragging with the left mouse button
   - Reset view by double-clicking

## License

This project is open source and available under the MIT License.
