//
// Created by Remus on 29/06/2025.
//

#include "Game.h"

#include "RenderSystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "systems/PhysicsSystem.h"
#include <SDL3/SDL.h>

Game::Game() : currentAppState(SDL_APP_FAILURE), lastFrameTime(0), deltaTime(0)
{
}

SDL_AppResult Game::Start()
{
    currentAppState = display.Init();
    if (currentAppState != SDL_APP_CONTINUE)
        return currentAppState;

    ecs = std::make_unique<HelloECS>();
    systemManager.RegisterSystem<PhysicsSystem>("physics", ecs.get());
    systemManager.RegisterSystem<RenderSystem>("render", &display, ecs.get());
    CreateLevel();

    return currentAppState;
}

SDL_AppResult Game::Update()
{
    Uint64 currentFrameTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    deltaTime = static_cast<float>(currentFrameTime - lastFrameTime) / static_cast<float>(frequency);
    lastFrameTime = currentFrameTime;

    systemManager.Update(deltaTime);

    display.PreRender();
    display.Render();
    display.PostRender();

    return currentAppState;
}

void Game::Shutdown()
{
}

void Game::CreateLevel()
{
    constexpr float LevelWidth = 500;
    constexpr float LevelHeight = 500;

    constexpr float brickHeight = 5.f;
    constexpr float brickWidth = 15.f;

    constexpr int columnCount = 25;
    constexpr int rowCount = 8;
    constexpr float brickSpacing = 0.5f;

    float totalBricksWidth = columnCount * (brickWidth - brickSpacing);
    float startX = (LevelWidth - totalBricksWidth) / 2.0f;
    float startY = 100;

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

    float paddleHeight = 5.f;
    float paddleHeightPositionOffset = 100.f;
    float paddleWidth = 15.f;

    paddleEntity = ecs->CreateEntity();
    Rect paddleRect(paddleHeight, paddleWidth);
    Position paddlePosition(LevelWidth  / 2, LevelHeight - paddleHeight - paddleHeightPositionOffset);
    Color paddleColor(0, 80, 255, 255);
    MouseInput mouseMovement {};

    ecs->AddComponent<Rect>(paddleEntity, paddleRect);
    ecs->AddComponent<Position>(paddleEntity, paddlePosition);
    ecs->AddComponent<Color>(paddleEntity, paddleColor);
    ecs->AddComponent<MouseInput>(paddleEntity,mouseMovement);
}

SDL_AppResult Game::GetState() const
{
    return currentAppState;
}

void Game::WindowEvent(SDL_Event* event)
{
    if (event->key.key == SDLK_ESCAPE)
    {
        currentAppState = SDL_APP_SUCCESS;
    }

    if (abs(event->motion.xrel) > 0)
    {
        int width, height;
        SDL_GetWindowSize(display.window, &width, &height);

        float scaleX = static_cast<float>(width) / display.GetWindowWidth();

        MouseInput* inputComponent = ecs->GetComponent<MouseInput>(paddleEntity);
        inputComponent->mousePositionX = event->motion.x /= scaleX;
    }
}
