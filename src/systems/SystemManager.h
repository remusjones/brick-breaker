//
// Created by Remus on 29/06/2025.
//

#pragma once
#include "System.h"

#include <memory>
#include <ranges>
#include <unordered_map>

class SystemManager
{
public:
    template<typename TSystem, typename... Args>
    TSystem* RegisterSystem(const std::string& systemName, Args&&... args)
    {
        auto system = std::make_shared<TSystem>(systemName, std::forward<Args>(args)...);
        systems.emplace(systemName, std::move(system));
        return system.get();
    }

    void UnregisterSystem(const std::string& systemName)
    {
        systems.erase(systemName);
    }

    template<typename TSystem>
    TSystem* GetSystem(const std::string& systemName)
    {
        auto it = systems.find(systemName);
        if (it != systems.end())
        {
            return static_cast<TSystem*>(it->second.get());
        }
        return nullptr;
    }

    void Init() const
    {
        for (const auto &system : systems | std::views::values)
        {
            system->Init();
        }
    }
    void Update(const float deltaTime) const
    {
        for (const auto &system : systems | std::views::values)
        {
            system->Update(deltaTime);
        }
    }
    void Shutdown() const
    {
        for (const auto &system : systems | std::views::values)
        {
            system->Shutdown();
        }
    }

private:
    std::unordered_map<std::string, std::shared_ptr<System>> systems;
};