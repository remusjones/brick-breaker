//
// Created by Remus on 29/06/2025.
//

#pragma once
#include "CoreTypes.h"
#include "System.h"

class HelloECS;
struct Dimension;

class PhysicsSystem : public System
{
    struct CollisionQuery
    {
        EntityHandle handle;
        Position position;
        Dimension rect;
    };

public:
    PhysicsSystem(const std::string_view &name, HelloECS *inECS);

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    static bool Intersection(const Position& paddlePosition, const Dimension& paddleDimension, const Position& ballPosition, const Circle& circle, Vector2& outNormal);
    static bool LevelBoundsIntersection(const Dimension& boundsDimension, const Position& ballPosition, const Circle& circle, Vector2& outNormal);
    static Vector2 Reflect(const Vector2& velocity, const Vector2& normal);

private:

    HelloECS* ecs;
    EntityHandle levelEntity;

    std::vector<CollisionQuery> paddleCollisions;
    std::vector<CollisionQuery> brickCollisions;
};
