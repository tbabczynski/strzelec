# strzelec

Visual Studio 2022 skeleton project for OpenGL 4.6 + ImGui + compute-shader simulation.

## Features

- OpenGL 4.6 Core Profile
- ImGui with docking and viewports
- Modular architecture (Application, Window, GL helpers, Simulation, GUI)
- Compute shader-based simulation example (2D diffusion)
- GLSL shaders loaded from separate files
- vcpkg dependency management

## Build Instructions

### Prerequisites

- Visual Studio 2022
- vcpkg (integrated with Visual Studio)
- Windows 10/11 with OpenGL 4.6 capable GPU

### Setup

1. **Install vcpkg** (if not already installed):
   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   bootstrap-vcpkg.bat
   ```

2. **Integrate vcpkg with Visual Studio**:
   ```bash
   vcpkg integrate install
   ```

3. **Install dependencies**:
   ```bash
   vcpkg install --triplet=x64-windows
   ```
   This will read `vcpkg.json` and install: glfw3, glm, glad, stb, imgui (with glfw-binding, opengl3-binding, docking-experimental features)

4. **Open the solution**:
   - Open `strzelec.sln` in Visual Studio 2022
   - Ensure the platform is set to `x64`
   - Build and run (F5 or Ctrl+F5)

### Project Structure

```
strzelec/
├── vcpkg.json                      # Dependency manifest
├── README.md                       # This file
├── .gitignore                      # VS and build artifacts
├── strzelec.sln                    # VS2022 solution
├── src/
│   ├── strzelec.vcxproj            # VS2022 project file
│   ├── main.cpp                    # Entry point
│   ├── Application.h/cpp           # Main orchestrator
│   ├── Window.h/cpp                # GLFW + OpenGL wrapper
│   ├── gl/
│   │   ├── Shader.h/cpp            # Shader loader/compiler
│   │   ├── GLBuffer.h/cpp          # VBO/SSBO wrapper
│   │   └── Framebuffer.h/cpp       # FBO wrapper
│   ├── sim/
│   │   ├── ISimulation.h           # Simulation interface
│   │   ├── ComputeSimulation.h/cpp # Compute shader simulation
│   │   └── SimulationManager.h/cpp # Simulation container
│   └── gui/
│       └── GUILayer.h/cpp          # ImGui wrapper
└── shaders/
    ├── fullscreen_quad.vert.glsl   # Fullscreen quad vertex shader
    ├── display.frag.glsl           # Display fragment shader
    └── compute_diffusion.comp.glsl # Compute shader example
```

## Technical Notes

- **OpenGL Version**: 4.6 Core Profile
- **GLSL Version**: `#version 460 core`
- **C++ Standard**: C++17
- **Compute Shader**: Uses SSBO binding 0, includes memory barriers (GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT)
- **ImGui**: Docking and viewports enabled, uses OpenGL3 backend with GLSL version 460

## Usage

The application demonstrates:
- A window management system (GLFW wrapper)
- Shader compilation and loading from files
- A compute shader-based 2D diffusion simulation
- ImGui controls to adjust simulation parameters (timestep slider)
- Modular design for easy extension

Modify `src/sim/ComputeSimulation.cpp` and `shaders/compute_diffusion.comp.glsl` to implement your own simulations.

## Troubleshooting

- **vcpkg integration not working**: Run `vcpkg integrate install` and restart Visual Studio
- **OpenGL errors**: Ensure your GPU supports OpenGL 4.6
- **Missing dependencies**: Re-run `vcpkg install --triplet=x64-windows`
- **Shader compilation errors**: Check `shaders/*.glsl` files; errors are logged to stderr

## License

[Add your license here]
