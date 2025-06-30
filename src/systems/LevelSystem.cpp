//
// Created by Remus on 30/06/2025.
//

#include "LevelSystem.h"
void LevelSystem::Init()
{
    // create level entity
    const EntityHandle levelEntity = ecs->CreateEntity();

    Level level{600, 400};
    ecs->AddComponent<Level>(levelEntity, level);

    constexpr float brickHeight = 5.f;
    constexpr float brickWidth = 15.f;

    constexpr int columnCount = 35;
    constexpr int rowCount = 12;
    constexpr float brickSpacing = 0.5f;

    // generate all the bricks in column & rows
    float totalBricksWidth = columnCount * (brickWidth + brickSpacing);
    const float startX = (level.width - totalBricksWidth) / 2.0f;
    const float startY = 50;

    for (int i = 0; i < columnCount; ++i)
    {
        for (int j = 0; j < rowCount; ++j)
        {
            EntityHandle entity = ecs->CreateEntity();
            Rect brickRect(brickHeight, brickWidth);

            float posX = startX + i * (brickWidth + brickSpacing);
            float posY = startY + j * (brickHeight + brickSpacing);
            Position brickPosition{ posX, posY };

            Color brickColor(255, std::lerp(0, 255, static_cast<float>(j) / rowCount), 0, 255);
            ecs->AddComponent<Rect>(entity, brickRect);
            ecs->AddComponent<Position>(entity, brickPosition);
            ecs->AddComponent<Color>(entity, brickColor);
        }
    }

    // create player paddle
    constexpr float paddleHeight = 5.f;
    constexpr float paddleHeightPositionOffset = 25.f;
    constexpr float paddleWidth = 20.f;

    paddleEntity = ecs->CreateEntity();
    Rect paddleRect(paddleHeight, paddleWidth);
    Position paddlePosition(level.width  / 2, level.height - paddleHeight - paddleHeightPositionOffset);
    Color paddleColor(0, 80, 255, 255);
    MouseInput mouseMovement {};

    ecs->AddComponent<Rect>(paddleEntity, paddleRect);
    ecs->AddComponent<Position>(paddleEntity, paddlePosition);
    ecs->AddComponent<Color>(paddleEntity, paddleColor);
    ecs->AddComponent<MouseInput>(paddleEntity,mouseMovement);

    // update our display for appropriate scaling
    display->SetLevelSize(level.width, level.height);
}

void LevelSystem::Update(float deltaTime)
{
}

void LevelSystem::Shutdown()
{
}

void LevelSystem::SetMouseLocation(float x, float y) const
{
    int width, height;
    SDL_GetWindowSize(display->window, &width, &height);

    const Level* level = ecs->GetComponent<Level>(levelEntity);
    const float scaleX = static_cast<float>(width) / level->width;

    const Rect* paddleRect = ecs->GetComponent<Rect>(paddleEntity);
    MouseInput* inputComponent = ecs->GetComponent<MouseInput>(paddleEntity);
    inputComponent->mousePositionX = std::clamp<float>(x /= scaleX, paddleRect->width / 2, level->width - paddleRect->width / 2);
}