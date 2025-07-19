//
// Created by Remus on 30/06/2025.
//


#pragma once
#include "Display.h"
#include "System.h"

class HelloECS;
class InputManager;

/*
 * LevelSystem provides basic gameplay behaviour, such as level spawning, input usage and win/lose monitoring
 */
class LevelSystem : public System
{
public:
    explicit LevelSystem(const std::string_view& name, Display* inDisplay, HelloECS* inECS, InputManager* inInputManager);

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    void SetMouseLocation(float mouseX, float mouseY) const;

private:
    Display* display;
    HelloECS* ecs;
    InputManager* inputManager;

    EntityHandle paddleEntity = INVALID_ENTITY;
    EntityHandle levelEntity = INVALID_ENTITY;

    std::string winText;
    std::string loseText;

};
