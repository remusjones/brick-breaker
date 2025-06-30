//
// Created by Remus on 30/06/2025.
//


#pragma once
#include "Display.h"
#include "ECS/HelloECS.h"
#include "System.h"


class LevelSystem : public System
{
public:
    explicit LevelSystem(const std::string_view& name, Display* inDisplay, HelloECS* inECS) : System(name), display(inDisplay), ecs(inECS) {}

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    void SetMouseLocation(float x, float y) const;

private:
    Display* display;
    HelloECS* ecs;
    EntityHandle paddleEntity = INVALID_ENTITY;
    EntityHandle levelEntity = INVALID_ENTITY;
};
