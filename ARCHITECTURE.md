# Architecture Documentation

## Overview

This document describes the modular architecture implemented in the Strzelec simulation application. The architecture follows the Dependency Inversion Principle (DIP) and uses the Factory pattern to create simulation modules.

## Core Interfaces

### ISimulation
Defines the interface for all simulation implementations. Includes methods for initialization, parameter updates, dispatching compute operations, and handling resize events.

**Location:** `src/sim/ISimulation.h`

### IRenderer
Defines the interface for rendering simulation visuals. Each simulation can have its own renderer implementation.

**Location:** `src/sim/IRenderer.h`

**Methods:**
- `void Render()` - Renders the simulation visuals

### IControlPanel
Defines the interface for rendering GUI controls. Each simulation can have its own control panel implementation.

**Location:** `src/sim/IControlPanel.h`

**Methods:**
- `void RenderControls()` - Renders ImGui controls for the simulation

## Module System

### SimulationModule
A simple aggregate structure that combines a simulation with its renderer and control panel.

**Location:** `src/sim/SimulationModule.h`

**Members:**
- `std::unique_ptr<ISimulation> simulation` - The simulation logic
- `std::unique_ptr<IRenderer> renderer` - The visual renderer
- `std::unique_ptr<IControlPanel> controls` - The GUI controls

### ModuleRegistry
A registry that manages factory functions for creating simulation modules. Uses the Factory pattern to decouple module creation from module usage.

**Location:** `src/sim/ModuleRegistry.h`

**Methods:**
- `void Register(const std::string& name, FactoryFunction factory)` - Registers a new module factory
- `std::vector<std::string> Names() const` - Returns all registered module names
- `std::unique_ptr<SimulationModule> Create(const std::string& name) const` - Creates a module by name

## Implementations

### Projectile Simulation Module

**Components:**
- `ProjectileSimulation` - CPU-based projectile physics simulation
- `ProjectileRenderer` - Renders the projectile, target, trajectory, and ground
- `ProjectileControlPanel` - ImGui controls for launch parameters, wind, target configuration

**Files:**
- `src/sim/ProjectileSimulation.h/cpp`
- `src/sim/ProjectileRenderer.h/cpp`
- `src/sim/ProjectileControlPanel.h/cpp`

### Compute Simulation Module

**Components:**
- `ComputeSimulation` - GPU compute shader-based simulation
- `ComputeRenderer` - Placeholder for future visual rendering
- `ComputeControlPanel` - ImGui controls for timestep adjustment

**Files:**
- `src/sim/ComputeSimulation.h/cpp`
- `src/sim/ComputeRenderer.h/cpp`
- `src/sim/ComputeControlPanel.h/cpp`

## Application Flow

### Initialization
1. `Application::Init()` creates and initializes rendering resources
2. `SimulationManager::Init()` is called with rendering resources
3. `SimulationManager::RegisterModules()` registers all module factories
4. Modules are created for each registered factory
5. `GUILayer::Init()` initializes ImGui

### Update Loop
1. `Application::Update()` polls events
2. `SimulationManager::UpdateAllParameters()` updates all simulations
3. `SimulationManager::DispatchAll()` dispatches all simulation computations

### Render Loop
1. **Background rendering:** Clear screen with background color
2. **Simulation rendering:** `IRenderer::Render()` of active simulation
3. **Controls rendering:** `IControlPanel::RenderControls()` of active simulation
4. Swap buffers

## Benefits of This Architecture

### Separation of Concerns
- **Simulation logic** (ISimulation) is separate from **rendering** (IRenderer) and **UI** (IControlPanel)
- Each component can be developed, tested, and maintained independently

### Dependency Inversion
- High-level modules (Application, SimulationManager) depend on abstractions (IRenderer, IControlPanel)
- Low-level modules (ProjectileRenderer, ComputeRenderer) implement these abstractions
- Dependencies point inward toward stable abstractions

### Factory Pattern
- Module creation is centralized in ModuleRegistry
- Easy to add new simulation types without modifying existing code
- Supports runtime module discovery and creation

### Extensibility
To add a new simulation type:
1. Implement `ISimulation`, `IRenderer`, and `IControlPanel`
2. Register the module factory in `SimulationManager::RegisterModules()`
3. No changes needed to Application, GUILayer, or other modules

## Rendering Separation

The Application::Render() method now cleanly separates three phases:

1. **Background Rendering:** Sets up the frame buffer and clears with background color
2. **Simulation Rendering:** Delegates to IRenderer to draw simulation-specific visuals
3. **Controls Rendering:** Delegates to IControlPanel (via GUILayer) for simulation-specific GUI

This separation makes it easy to:
- Replace rendering backends
- Add post-processing effects
- Implement multiple viewports
- Add overlays or HUD elements

## Design Patterns Used

- **Dependency Inversion Principle (DIP):** High-level modules depend on abstractions
- **Factory Pattern:** ModuleRegistry creates modules via factory functions
- **Strategy Pattern:** Different simulation modules can be swapped at runtime
- **Facade Pattern:** SimulationManager provides a simplified interface to the module system

## Future Extensions

This architecture supports:
- Multiple active simulations displayed side-by-side
- Hot-reloading of simulation modules
- Plugin system for external simulations
- Recording and playback of simulations
- Network synchronization for distributed simulations
