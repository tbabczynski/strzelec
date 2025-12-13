#pragma once

class ISimulation
{
public:
    virtual ~ISimulation() = default;

    virtual void Init() = 0;
    virtual void UpdateParameters() = 0;
    virtual void Dispatch() = 0;
    virtual void Resize(int width, int height) = 0;

    virtual float GetTimeStep() const = 0;
    virtual void SetTimeStep(float dt) = 0;
};
