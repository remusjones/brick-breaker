//
// Created by Remus on 29/06/2025.
//

#include "PhysicsSystem.h"

#include "CoreTypes.h"

void PhysicsSystem::Init()
{
}

void PhysicsSystem::Update(float deltaTime)
{
    // Update paddle
    auto inputView = ecs->GetView<MouseInput, Position, Rect>();
    inputView.Each([&](const EntityHandle& handle, const MouseInput& mouseInput, Position& position, const Rect& rect) {
        position.x = mouseInput.mousePositionX - (rect.width / 2);
    });
}

void PhysicsSystem::Shutdown()
{
}