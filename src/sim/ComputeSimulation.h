#pragma once

#include "ISimulation.h"
#include <memory>

class Shader;

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

    unsigned int GetTexture() const { return m_TexA; }

private:
    void CreateTextures(int width, int height);
    void DestroyTextures();

    std::unique_ptr<Shader> m_ComputeShader;

    unsigned int m_TexA;
    unsigned int m_TexB;
    unsigned int m_SrcTex;
    unsigned int m_DstTex;

    int m_Width;
    int m_Height;
    float m_TimeStep;
    unsigned int m_Frame;
};
