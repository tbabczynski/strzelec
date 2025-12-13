#include "ProjectileSimulation.h"
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <iostream>

ProjectileSimulation::ProjectileSimulation()
    : m_Position(0.0f, 1.5f)
    , m_Velocity(0.0f, 0.0f)
    , m_InitialPosition(0.0f, 1.5f)
    , m_Wind(0.0f, 0.0f)
    , m_TargetPosition(30.0f, 1.5f)
    , m_TargetRadius(0.5f)
    , m_Mass(0.02f)              // 20 grams
    , m_Cd(0.3f)                 // drag coefficient
    , m_Area(glm::pi<float>() * 0.0035f * 0.0035f)  // pi * r^2, r = 3.5mm
    , m_Rho(1.225f)              // air density at sea level
    , m_G(9.80665f)              // standard gravity
    , m_TimeStep(0.016f)         // ~60 FPS
    , m_SubSteps(4)
    , m_SimTime(0.0f)
    , m_Flying(false)
    , m_Hit(false)
    , m_Paused(false)
    , m_StepRequested(false)
    , m_InitialSpeed(50.0f)
    , m_InitialAngle(25.0f)
    , m_ShowTrajectory(true)
{
}

ProjectileSimulation::~ProjectileSimulation()
{
}

void ProjectileSimulation::Init()
{
    Reset();
    std::cout << "ProjectileSimulation initialized" << std::endl;
}

void ProjectileSimulation::UpdateParameters()
{
    // Parameters are updated via GUI controls
}

void ProjectileSimulation::Dispatch()
{
    if (m_Flying && (!m_Paused || m_StepRequested))
    {
        float dt = m_TimeStep / static_cast<float>(m_SubSteps);
        
        for (int i = 0; i < m_SubSteps; ++i)
        {
            UpdatePhysics(dt);
            
            if (!m_Flying)
                break;
        }

        m_StepRequested = false;
    }
}

void ProjectileSimulation::Resize(int width, int height)
{
    // Not used for CPU simulation
}

void ProjectileSimulation::Launch(float speed, float angleDeg)
{
    Reset();

    m_InitialSpeed = speed;
    m_InitialAngle = angleDeg;

    float angleRad = glm::radians(angleDeg);
    m_Velocity.x = speed * std::cos(angleRad);
    m_Velocity.y = speed * std::sin(angleRad);

    m_Flying = true;
    m_Hit = false;
    m_SimTime = 0.0f;
    m_Trajectory.clear();
    m_Trajectory.push_back(m_Position);

    std::cout << "Arrow launched: speed=" << speed << " m/s, angle=" << angleDeg 
              << " deg, velocity=(" << m_Velocity.x << ", " << m_Velocity.y << ")" << std::endl;
}

void ProjectileSimulation::SetWind(const glm::vec2& wind)
{
    m_Wind = wind;
}

void ProjectileSimulation::SetTarget(const glm::vec2& pos, float radius)
{
    m_TargetPosition = pos;
    m_TargetRadius = radius;
}

void ProjectileSimulation::RequestStep()
{
    m_StepRequested = true;
}

void ProjectileSimulation::SetPaused(bool paused)
{
    m_Paused = paused;
}

void ProjectileSimulation::Reset()
{
    m_Position = m_InitialPosition;
    m_Velocity = glm::vec2(0.0f, 0.0f);
    m_Flying = false;
    m_Hit = false;
    m_SimTime = 0.0f;
    m_Paused = false;
    m_StepRequested = false;
    m_Trajectory.clear();
}

float ProjectileSimulation::GetAngle() const
{
    if (glm::length(m_Velocity) < 0.01f)
        return 0.0f;
    
    return std::atan2(m_Velocity.y, m_Velocity.x);
}

void ProjectileSimulation::UpdatePhysics(float dt)
{
    if (!m_Flying)
        return;

    // Relative velocity (arrow velocity relative to wind)
    glm::vec2 v_rel = m_Velocity - m_Wind;
    float v_rel_mag = glm::length(v_rel);

    // Drag force: F_d = -0.5 * rho * Cd * A * |v_rel| * v_rel
    glm::vec2 dragForce(0.0f, 0.0f);
    if (v_rel_mag > 0.001f)
    {
        dragForce = -0.5f * m_Rho * m_Cd * m_Area * v_rel_mag * v_rel;
    }

    // Gravitational force: F_g = m * g (downward)
    glm::vec2 gravityForce(0.0f, -m_Mass * m_G);

    // Total force
    glm::vec2 totalForce = dragForce + gravityForce;

    // Semi-implicit Euler integration
    // v(t+dt) = v(t) + (F/m) * dt
    // x(t+dt) = x(t) + v(t+dt) * dt
    glm::vec2 acceleration = totalForce / m_Mass;
    m_Velocity += acceleration * dt;
    m_Position += m_Velocity * dt;

    m_SimTime += dt;

    // Record trajectory point (sample every substep to get smooth trajectory)
    m_Trajectory.push_back(m_Position);

    // Check collisions
    if (CheckGroundCollision())
    {
        m_Flying = false;
        std::cout << "Arrow hit the ground at position (" << m_Position.x << ", " << m_Position.y 
                  << ") after " << m_SimTime << " seconds" << std::endl;
    }
    else if (CheckTargetHit())
    {
        m_Flying = false;
        m_Hit = true;
        std::cout << "Arrow hit the target at position (" << m_Position.x << ", " << m_Position.y 
                  << ") after " << m_SimTime << " seconds!" << std::endl;
    }
}

bool ProjectileSimulation::CheckGroundCollision()
{
    return m_Position.y <= 0.0f;
}

bool ProjectileSimulation::CheckTargetHit()
{
    // Check if the arrow tip is within the target radius
    float distance = glm::length(m_Position - m_TargetPosition);
    return distance <= m_TargetRadius;
}
