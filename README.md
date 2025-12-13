# strzelec

Visual Studio 2022 skeleton project for OpenGL 4.6 + ImGui + compute-shader simulation.

## Features

- OpenGL 4.6 Core Profile
- ImGui with docking and viewports
- Modular architecture (Application, Window, GL helpers, Simulation, GUI)
- CPU-based projectile (archery) simulation with physics
  - Semi-implicit Euler integration
  - Gravity, aerodynamic drag, and wind effects
  - Target collision detection
  - Real-time trajectory visualization
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
├── vcpkg.json                         # Dependency manifest
├── README.md                          # This file
├── .gitignore                         # VS and build artifacts
├── strzelec.sln                       # VS2022 solution
├── src/
│   ├── strzelec.vcxproj               # VS2022 project file
│   ├── main.cpp                       # Entry point
│   ├── Application.h/cpp              # Main orchestrator + rendering
│   ├── Window.h/cpp                   # GLFW + OpenGL wrapper
│   ├── gl/
│   │   ├── Shader.h/cpp               # Shader loader/compiler
│   │   ├── GLBuffer.h/cpp             # VBO/SSBO wrapper
│   │   └── Framebuffer.h/cpp          # FBO wrapper
│   ├── sim/
│   │   ├── ISimulation.h              # Simulation interface
│   │   ├── ProjectileSimulation.h/cpp # CPU-based projectile physics
│   │   ├── ComputeSimulation.h/cpp    # Compute shader simulation
│   │   └── SimulationManager.h/cpp    # Simulation container
│   └── gui/
│       └── GUILayer.h/cpp             # ImGui wrapper + controls
└── shaders/
    ├── fullscreen_quad.vert.glsl      # Fullscreen quad vertex shader
    ├── display.frag.glsl              # Display fragment shader
    └── compute_diffusion.comp.glsl    # Compute shader example
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
- A CPU-based projectile (archery) simulation with realistic physics
  - **Launch Parameters**: Adjust speed (10-100 m/s) and angle (0-90 degrees)
  - **Target Configuration**: Set target position (X, Y in meters) and radius
  - **Wind Effects**: Configure wind speed in X and Y directions
  - **Controls**: Launch, Reset, Pause, and Step-through simulation
  - **Visualization**: Toggle trajectory display, see real-time arrow position and target
  - **Status Display**: Shows flight time, position, velocity, and hit detection
- A compute shader-based 2D diffusion simulation
- ImGui controls to adjust simulation parameters
- Modular design for easy extension

### Projectile Simulation Details

The projectile simulation uses realistic physics:
- **Mass**: 0.02 kg (20 grams, typical arrow mass)
- **Drag Coefficient**: 0.3
- **Cross-sectional Area**: π × (0.0035 m)² (7mm diameter arrow)
- **Air Density**: 1.225 kg/m³ (sea level)
- **Gravity**: 9.80665 m/s²
- **Integration**: Semi-implicit Euler with 4 sub-steps per frame for stability

Default parameters:
- Initial position: (0, 1.5) m above ground
- Default launch speed: 50 m/s
- Default launch angle: 25 degrees
- Target position: (30, 1.5) m
- Target radius: 0.5 m
- Wind: (0, 0) m/s (no wind)

Modify `src/sim/ProjectileSimulation.cpp` to adjust physics parameters or add new features.
Modify `src/sim/ComputeSimulation.cpp` and `shaders/compute_diffusion.comp.glsl` to implement other simulations.

## Troubleshooting

- **vcpkg integration not working**: Run `vcpkg integrate install` and restart Visual Studio
- **OpenGL errors**: Ensure your GPU supports OpenGL 4.6
- **Missing dependencies**: Re-run `vcpkg install --triplet=x64-windows`
- **Shader compilation errors**: Check `shaders/*.glsl` files; errors are logged to stderr

## License

[Add your license here]
