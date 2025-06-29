//
// Created by Remus on 29/06/2025.
//

#pragma once
#include <string>

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