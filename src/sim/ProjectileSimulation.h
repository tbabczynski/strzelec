#pragma once

#include "ISimulation.h"
#include <glm/glm.hpp>
#include <vector>

class ProjectileSimulation : public ISimulation
{
public:
    ProjectileSimulation();
    ~ProjectileSimulation() override;

    void Init() override;
    void UpdateParameters() override;
    void Dispatch() override;
    void Resize(int width, int height) override;

    float GetTimeStep() const override { return m_TimeStep; }
    void SetTimeStep(float dt) override { m_TimeStep = dt; }

    // Control methods
    void Launch(float speed, float angleDeg);
    void SetWind(const glm::vec2& wind);
    void SetTarget(const glm::vec2& pos, float radius);
    void RequestStep();
    void SetPaused(bool paused);
    void Reset();

    // Getters
    glm::vec2 GetPosition() const { return m_Position; }
    glm::vec2 GetVelocity() const { return m_Velocity; }
    float GetAngle() const;
    bool IsFlying() const { return m_Flying; }
    bool HasHit() const { return m_Hit; }
    float GetSimTime() const { return m_SimTime; }
    const std::vector<glm::vec2>& GetTrajectory() const { return m_Trajectory; }

    // Target info
    glm::vec2 GetTargetPosition() const { return m_TargetPosition; }
    float GetTargetRadius() const { return m_TargetRadius; }

    // Initial parameters (for GUI)
    float GetInitialSpeed() const { return m_InitialSpeed; }
    float GetInitialAngle() const { return m_InitialAngle; }
    glm::vec2 GetWind() const { return m_Wind; }

    // Rendering options
    void SetShowTrajectory(bool show) { m_ShowTrajectory = show; }
    bool GetShowTrajectory() const { return m_ShowTrajectory; }

private:
    void UpdatePhysics(float dt);
    bool CheckGroundCollision();
    bool CheckTargetHit();

    // Simulation state
    glm::vec2 m_Position;
    glm::vec2 m_Velocity;
    glm::vec2 m_InitialPosition;
    glm::vec2 m_Wind;
    
    glm::vec2 m_TargetPosition;
    float m_TargetRadius;

    // Physical constants
    float m_Mass;           // kg
    float m_Cd;             // drag coefficient
    float m_Area;           // cross-sectional area (m^2)
    float m_Rho;            // air density (kg/m^3)
    float m_G;              // gravitational acceleration (m/s^2)

    // Simulation parameters
    float m_TimeStep;
    int m_SubSteps;
    float m_SimTime;
    
    bool m_Flying;
    bool m_Hit;
    bool m_Paused;
    bool m_StepRequested;

    // Initial launch parameters (for GUI)
    float m_InitialSpeed;
    float m_InitialAngle;

    // Trajectory history
    std::vector<glm::vec2> m_Trajectory;

    // Rendering options
    bool m_ShowTrajectory;
};
