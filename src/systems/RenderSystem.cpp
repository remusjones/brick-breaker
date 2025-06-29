//
// Created by Remus on 29/06/2025.
//

#include "RenderSystem.h"

#include <iostream>

void RenderSystem::Init()
{
}

void RenderSystem::Update(float deltaTime)
{
    auto rectangleView = ecs->GetView<Position, Rect, Color>();
    rectangleView.Each([&](const EntityHandle& entity, const Position& position, const Rect& rect, const Color& color) {
        DrawRect rectangle{position, rect.width, rect.height, SDL_Color(color.r, color.g, color.b, color.a)};
        display->rectDrawList.emplace_back(rectangle);
    });
}

void RenderSystem::Shutdown()
{
}