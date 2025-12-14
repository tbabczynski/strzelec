#pragma once

#include <memory>

// Forward declarations
class ISimulation;
class IRenderer;
class IControlPanel;

struct SimulationModule
{
    std::unique_ptr<ISimulation> simulation;
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<IControlPanel> controls;
};
