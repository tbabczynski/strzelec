#pragma once

#include "ISimulation.h"
#include <memory>

class Shader;
class GLBuffer;

class ComputeSimulation : public ISimulation
{
public:
    ComputeSimulation();
    ~ComputeSimulation() override;

    void Init() override;
    void UpdateParameters() override;
    void Dispatch() override;
    void Resize(int width, int height) override;

    float GetTimeStep() const override { return m_TimeStep; }
    void SetTimeStep(float dt) override { m_TimeStep = dt; }

private:
    std::unique_ptr<Shader> m_ComputeShader;
    std::unique_ptr<GLBuffer> m_StateBuffer;

    int m_Width;
    int m_Height;
    float m_TimeStep;
    unsigned int m_Frame;
};
