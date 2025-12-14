#pragma once

class IControlPanel
{
public:
    virtual ~IControlPanel() = default;

    virtual void RenderControls() = 0;
};
