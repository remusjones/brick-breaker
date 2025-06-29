//
// Created by Remus on 29/06/2025.
//


#pragma once
#include "Display.h"
#include "ECS/HelloECS.h"
#include "System.h"

class RenderSystem : public System
{

public:
    explicit RenderSystem(const std::string_view& name, Display* inDisplay, HelloECS* inECS)
    : System(name), display(inDisplay), ecs(inECS) {}

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

private:
    Display* display;
    HelloECS* ecs;
};
