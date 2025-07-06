//
// Created by Remus on 29/06/2025.
//

#include "RenderSystem.h"

void RenderSystem::Init()
{
}

void RenderSystem::Update(float deltaTime)
{
    auto rectangleView = ecs->GetView<Position, Dimension, Color>();
    rectangleView.Each([&](const EntityHandle& entity, const Position& position, const Dimension& rect, const Color& color) {
        DrawRect rectangle{position, rect, SDL_Color(color.r, color.g, color.b, color.a)};
        display->rectDrawList.emplace_back(rectangle);
    });

    // Todo: Color is not used here
    auto circleView = ecs->GetView<Position, Circle, Color>();
    circleView.Each([&](const EntityHandle& entity, const Position& position, const Circle& circle, const Color& color) {
        DrawBall circleDraw{position, circle};
        display->ballDrawList.emplace_back(circleDraw);
    });
}

void RenderSystem::Shutdown()
{
}