//
// Created by Remus on 30/06/2025.
//

#include "LevelSystem.h"

#include "ECS/HelloECS.h"
#include "GameUtils.h"
#include "InputManager.h"
#include <algorithm>

LevelSystem::LevelSystem(const std::string_view &name, Display *inDisplay, HelloECS *inECS, InputManager *inInputManager)
    : System(name), display(inDisplay), ecs(inECS), inputManager(inInputManager)
{
    winText = "Win";
    loseText = "Lose";
}

void LevelSystem::Init()
{
    // create level entity
    levelEntity = ecs->CreateEntity();

    Level level{600, 400};

    constexpr float brickHeight = 5.f;
    constexpr float brickWidth = 15.f;

    constexpr int columnCount = 35;
    constexpr int rowCount = 12;
    constexpr float brickSpacing = 0.5f;

    // generate all the bricks in column & rows
    // Todo: Ideal location to read from a file
    float totalBricksWidth = columnCount * (brickWidth + brickSpacing);
    const float startX = (level.dimension.width - totalBricksWidth) / 2.0f;
    const float startY = 50;

    for (int i = 0; i < columnCount; ++i)
    {
        for (int j = 0; j < rowCount; ++j)
        {
            float posX = startX + i * (brickWidth + brickSpacing);
            float posY = startY + j * (brickHeight + brickSpacing);
            Color brickColor(255, std::lerp(0, 255, static_cast<float>(j) / rowCount), 0, 255);

            GameUtils::CreateBrick(ecs, posX, posY, brickWidth, brickHeight, brickColor);
        }
    }

    // create player paddle
    constexpr float paddleHeightPositionOffset = 25.f;
    constexpr float paddleHeight = 5.f;
    constexpr float paddleWidth = 25.f;
    constexpr float paddleSpeed = 10.f;

    const float paddleX = level.dimension.width / 2;
    const float paddleY = level.dimension.height - paddleHeight - paddleHeightPositionOffset;
    paddleEntity = GameUtils::CreatePaddle(ecs,
        paddleX, paddleY,
        paddleWidth, paddleHeight,
        paddleSpeed, Color(0, 80, 255, 255)
    );

    // create first ball
    constexpr float ballHeightOffset = 2;
    constexpr float ballRadius = 4;

    EntityHandle ballHandle = GameUtils::CreateBall(ecs,
        paddleX, paddleY - ballHeightOffset, ballRadius,
        0, 0,
        Color(255, 255, 255, 255)
    );

    // Attach the ball to the players paddle
    Attached ballAttachment(paddleEntity, {paddleWidth/2, -paddleHeight});
    ecs->AddComponent(ballHandle, ballAttachment);

    constexpr float startingBallSpeed = -300.f;

    // Configure the ball release on input down
    level.ballReleaseCallbackHandle = inputManager->RegisterMouseClickEvent(1, [ballHandle, this] {
        ecs->RemoveComponent<Attached>(ballHandle);
        Body* body = ecs->GetComponent<Body>(ballHandle);
        Level* level = ecs->GetComponent<Level>(levelEntity);
        body->velocity.y = startingBallSpeed;

        inputManager->UnregisterMouseClickEvent(1, level->ballReleaseCallbackHandle);
    });


    ecs->AddComponent<Level>(levelEntity, level);
    // update our display for appropriate scaling
    display->SetLevelSize(level.dimension.width, level.dimension.height);
}

void LevelSystem::Update(float deltaTime)
{

    // Todo: Placeholder Win/loss conditions for full game loop
    // todo: ECS should provide component counts for things like this
    bool lost = true;
    auto ballView = ecs->GetView<Circle, Body>();
    ballView.Each([&](const EntityHandle& handle, const Circle& circle, const Body& body) {
        lost = false;
    });

    bool win = true;
    auto brickView = ecs->GetView<Dimension, Position>();
    brickView.Each([&](const EntityHandle& handle, const Dimension& dimension, const Position& position) {
        if (handle == paddleEntity) return;

        win = false;
    });


    // todo: ideal use-case for an event dispatcher to maintain brick/ball count
    if (lost)
    {
        display->textDrawList.push_back(loseText);
    }

    if (win)
    {
        display->textDrawList.push_back(winText);
    }
}

void LevelSystem::Shutdown()
{
}

void LevelSystem::SetMouseLocation(float mouseX, float mouseY) const
{
    int width, height;
    SDL_GetWindowSize(display->window, &width, &height);

    const Level* level = ecs->GetComponent<Level>(levelEntity);
    const float scaleX = static_cast<float>(width) / level->dimension.width;

    const Dimension* paddleRect = ecs->GetComponent<Dimension>(paddleEntity);
    Paddle* inputComponent = ecs->GetComponent<Paddle>(paddleEntity);
    inputComponent->mousePositionX = std::clamp<float>(mouseX / scaleX, paddleRect->width / 2, level->dimension.width - paddleRect->width / 2);
}