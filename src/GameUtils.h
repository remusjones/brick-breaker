//
// Created by Remus on 19/07/2025.
//
#pragma once
#include "CoreTypes.h"
#include "ECS/CoreTypes.h"
#include "ECS/HelloECS.h"

class GameUtils
{
public:

    static EntityHandle CreateBall(
        HelloECS* ecs,
        const float& posX,
        const float& posY,
        const float& radius,
        const float& velocityX,
        const float& velocityY,
        Color color)
    {
        const EntityHandle ball = ecs->CreateEntity();
        Circle circle(radius);
        Position position(posX, posY);
        Body body({velocityX, velocityY});

        ecs->AddComponent(ball, circle);
        ecs->AddComponent(ball, position);
        ecs->AddComponent(ball, body);
        ecs->AddComponent(ball, color);

        return ball;
    }

    static EntityHandle CreateBrick(
        HelloECS* ecs,
        const float& posX,
        const float& posY,
        const float& width,
        const float& height,
        Color color)
    {
        const EntityHandle brick = ecs->CreateEntity();

        Position position(posX, posY);
        Dimension dimension(width, height);

        ecs->AddComponent<Dimension>(brick, dimension);
        ecs->AddComponent<Position>(brick, position);
        ecs->AddComponent<Color>(brick, color);
        return brick;
    }

    static EntityHandle CreatePaddle(
        HelloECS* ecs,
        const float& posX,
        const float& posY,
        const float& width,
        const float& height,
        const float& speed,
        Color color)
    {
        const EntityHandle paddleEntity = ecs->CreateEntity();

        Dimension paddleRect(width, height);
        Position position(posX, posY);
        Paddle paddle (speed, position.x);

        ecs->AddComponent<Dimension>(paddleEntity, paddleRect);
        ecs->AddComponent<Position>(paddleEntity, position);
        ecs->AddComponent<Color>(paddleEntity, color);
        ecs->AddComponent<Paddle>(paddleEntity,paddle);

        return paddleEntity;
    }
};