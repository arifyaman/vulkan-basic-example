# Vulkan Example - CMake Build Guide

This project uses CMake for cross-platform building.

## Prerequisites

- **CMake** 3.10 or higher
- **Visual Studio 2022** (on Windows) or compatible C++17 compiler
- **Vulkan SDK** 1.4.335.0 (installed and in system PATH)
- **GLFW3**, **GLM**, **stb**, **tiny_obj_loader** (included in `lib/` directory)

## Build Instructions

### Windows (Using CMake)

#### Option 1: Using batch script (easiest)
```bash
# Configure and build in Debug mode
build.bat

# Configure and build in Release mode with run
build.bat release run
```

#### Option 2: Using CMake directly
```bash
# Configure (one-time)
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug -B build -S .

# Build Debug
cmake --build build --config Debug

# Build Release
cmake --build build --config Release
```

#### Option 3: Using VS Code
1. Press `Ctrl+Shift+B` to build (uses "CMake: Build Debug" as default task)
2. Press `F5` to debug and run
3. Use Command Palette (`Ctrl+Shift+P`) and select "CMake: Build Release" for release builds

### Linux/macOS

```bash
# Configure
cmake -DCMAKE_BUILD_TYPE=Release -B build -S .

# Build
cmake --build build

# Run
./build/bin/vulkan_app
```

## Project Structure

```
.
├── CMakeLists.txt          # CMake configuration
├── build.bat               # Windows build helper script
├── main.cpp                # Application entry point
├── Model.h/cpp             # Model class for loading and storing geometry
├── ModelInstance.h         # Instance class for per-object transforms
├── SceneManager.h          # Scene graph manager
├── lib/
│   ├── glfw-3.4/          # GLFW3 library
│   ├── glm-1.0.3/         # GLM math library
│   ├── stb-master/        # STB utilities (image loading)
│   └── tiny_obj_loader/   # OBJ file loader
├── shaders/
│   ├── shader.vert        # Vertex shader source
│   ├── shader.frag        # Fragment shader source
│   ├── vert.spv           # Compiled vertex shader
│   └── frag.spv           # Compiled fragment shader
├── models/
│   └── viking_room.obj    # 3D model asset
├── textures/
│   └── viking_room.png    # Texture asset
└── .vscode/
    ├── tasks.json         # VS Code build tasks
    ├── launch.json        # VS Code debug configuration
    └── c_cpp_properties.json  # C++ IntelliSense settings
```

## Output Locations

- **Debug Build**: `build/bin/Debug/vulkan_app.exe`
- **Release Build**: `build/bin/Release/vulkan_app.exe`

## Cleaning Build Artifacts

```bash
# Option 1: Using CMake
cmake --build build --target clean

# Option 2: Using batch script
build.bat clean

# Option 3: Manual
rmdir /s /q build
```

## Troubleshooting

### CMake not found
- Ensure CMake is installed and in your system PATH
- Download from: https://cmake.org/download/

### Vulkan SDK not found
- Install Vulkan SDK from: https://vulkan.lunarg.com/sdk/home
- Ensure it's in your PATH or set `Vulkan_DIR` environment variable

### GLFW linking errors
- Verify the GLFW library exists: `lib/glfw-3.4/lib-vc2022/glfw3.lib`
- For different Visual Studio versions, update the path in CMakeLists.txt

## Next Steps

After setting up CMake:
1. Run `build.bat` or use VS Code to compile
2. Press `F5` to debug, or run the executable directly
3. To add new files, update the `SOURCES` list in CMakeLists.txt
