//
// Created by Remus on 29/06/2025.
//

#pragma once
#include "ECS/HelloECS.h"
#include "System.h"

class PhysicsSystem : public System
{
public:
    explicit PhysicsSystem(const std::string_view &name, HelloECS *inECS) : System(name), ecs(inECS)
    {
    }

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

private:
    HelloECS* ecs;
};
