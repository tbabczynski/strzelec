#pragma once

#include "SimulationModule.h"
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>

class ModuleRegistry
{
public:
    using FactoryFunction = std::function<std::unique_ptr<SimulationModule>()>;

    void Register(const std::string& name, FactoryFunction factory)
    {
        m_Factories[name] = factory;
    }

    std::vector<std::string> Names() const
    {
        std::vector<std::string> names;
        for (const auto& pair : m_Factories)
        {
            names.push_back(pair.first);
        }
        return names;
    }

    std::unique_ptr<SimulationModule> Create(const std::string& name) const
    {
        auto it = m_Factories.find(name);
        if (it != m_Factories.end())
        {
            return it->second();
        }
        return nullptr;
    }

private:
    std::map<std::string, FactoryFunction> m_Factories;
};
