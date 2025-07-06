//
// Created by Remus on 29/06/2025.
//

#pragma once
#include <string>

/*
 * Todo: This should probably be moved to the HelloECS module, and have a base constructor for an ECS handle
 * Used to abstract features away into systems that can be ticked.
 */
class System
{
public:
    virtual ~System() = default;
    System(const std::string_view name) : systemName(name) {}
    [[nodiscard]] const char* GetName() const { return systemName.c_str(); }

    virtual void Init() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() = 0;

private:
    std::string systemName;
};