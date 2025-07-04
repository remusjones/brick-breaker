//
// Created by Remus on 30/06/2025.
//

#include "LevelSystem.h"

#include "ECS/HelloECS.h"
#include "InputManager.h"
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
    float totalBricksWidth = columnCount * (brickWidth + brickSpacing);
    const float startX = (level.dimension.width - totalBricksWidth) / 2.0f;
    const float startY = 50;

    for (int i = 0; i < columnCount; ++i)
    {
        for (int j = 0; j < rowCount; ++j)
        {
            EntityHandle entity = ecs->CreateEntity();

            float posX = startX + i * (brickWidth + brickSpacing);
            float posY = startY + j * (brickHeight + brickSpacing);
            Dimension brickRect(brickWidth, brickHeight);
            Position brickPosition(posX, posY);

            Color brickColor(255, std::lerp(0, 255, static_cast<float>(j) / rowCount), 0, 255);
            ecs->AddComponent<Dimension>(entity, brickRect);
            ecs->AddComponent<Position>(entity, brickPosition);
            ecs->AddComponent<Color>(entity, brickColor);
        }
    }

    // create player paddle
    constexpr float paddleHeightPositionOffset = 25.f;
    constexpr float paddleHeight = 5.f;
    constexpr float paddleWidth = 20.f;
    constexpr float paddleSpeed = 0.01f;

    paddleEntity = ecs->CreateEntity();
    Dimension paddleRect(paddleWidth, paddleHeight);
    Position position(level.dimension.width  / 2, level.dimension.height - paddleHeight - paddleHeightPositionOffset);
    Color paddleColor(0, 80, 255, 255);
    Paddle paddle (paddleSpeed, position.x);

    ecs->AddComponent<Dimension>(paddleEntity, paddleRect);
    ecs->AddComponent<Position>(paddleEntity, position);
    ecs->AddComponent<Color>(paddleEntity, paddleColor);
    ecs->AddComponent<Paddle>(paddleEntity,paddle);

    // create first ball
    EntityHandle ballHandle = ecs->CreateEntity();

    Circle ballCircle(4);
    Position ballPosition(position.x, position.y - 2);
    Body ballBody({0, 0});
    Color ballColor(255,255,255,255);
    Attached ballAttachment(paddleEntity, {paddleWidth/2, -paddleHeight});

    ecs->AddComponent(ballHandle, ballAttachment);
    ecs->AddComponent(ballHandle, ballCircle);
    ecs->AddComponent(ballHandle, ballPosition);
    ecs->AddComponent(ballHandle, ballBody);
    ecs->AddComponent(ballHandle, ballColor);


    // Configure the ball release on input down
    level.ballReleaseCallbackHandle = inputManager->RegisterMouseClickEvent(1, [ballHandle, this] {
        ecs->RemoveComponent<Attached>(ballHandle);
        Body *body = ecs->GetComponent<Body>(ballHandle);
        Level* level = ecs->GetComponent<Level>(levelEntity);
        body->velocity.y = -0.33f;

        inputManager->UnregisterMouseClickEvent(1, level->ballReleaseCallbackHandle);
    });

    ecs->AddComponent<Level>(levelEntity, level);

    // update our display for appropriate scaling
    display->SetLevelSize(level.dimension.width, level.dimension.height);
}

void LevelSystem::Update(float deltaTime)
{
    auto inputView = ecs->GetView<Position, Paddle, Dimension>();
    inputView.Each([&](const EntityHandle& entityHandle, Position& position, const Paddle& paddle, const Dimension& rect) {
        position.x = std::lerp(position.x, paddle.mousePositionX - rect.width / 2, paddle.paddleSpeed * deltaTime);
    });


    // Win/loss conditions

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

void LevelSystem::SetMouseLocation(float x, float y) const
{
    int width, height;
    SDL_GetWindowSize(display->window, &width, &height);

    const Level* level = ecs->GetComponent<Level>(levelEntity);
    const float scaleX = static_cast<float>(width) / level->dimension.width;

    const Dimension* paddleRect = ecs->GetComponent<Dimension>(paddleEntity);
    Paddle* inputComponent = ecs->GetComponent<Paddle>(paddleEntity);
    inputComponent->mousePositionX = std::clamp<float>(x / scaleX, paddleRect->width / 2, level->dimension.width - paddleRect->width / 2);
}