//
// Created by Remus on 29/06/2025.
//

#include "PhysicsSystem.h"
#include "CoreTypes.h"
#include "ECS/HelloECS.h"
#include <cmath>
#include <ranges>

PhysicsSystem::PhysicsSystem(const std::string_view &name, HelloECS *inECS) : System(name), ecs(inECS)
{
}

void PhysicsSystem::Init()
{
    // todo: ideal spot for component singleton
    auto levelView = ecs->GetView<Level>();
    levelView.Each([&](const EntityHandle& handle, const Level& level) {
        levelEntity = handle;
    });
}

void PhysicsSystem::Update(float deltaTime)
{
    paddleCollisions.clear();
    brickCollisions.clear();

    constexpr float gainPerPaddleHitScale = 0.99f;
    constexpr float gainPerBrickHitScale = 1.02f;
    constexpr float gainPerBoundsHitScale = 1.0f;

    // update all velocities
    // todo: good use-case for a .without filter to ignore attached bodies
    auto velocityView = ecs->GetView<Position, Body>();
    velocityView.Each([&](const EntityHandle& handle, Position& position, Body & body) {

        position.x += body.velocity.x * deltaTime;
        position.y += body.velocity.y * deltaTime;

        if (ecs->HasComponent<Attached>(handle))
        {
            Attached* attached = ecs->GetComponent<Attached>(handle);
            const Position* parentPosition = ecs->GetComponent<Position>(attached->parent);
            position = *parentPosition + attached->offset;
        }
    });

    // Find all potential collisions
    auto rectCollisionView = ecs->GetView<Position, Dimension>();
    rectCollisionView.Each([&](const EntityHandle& handle, const Position& position, const Dimension& rect) {
        if (ecs->HasComponent<Paddle>(handle))
        {
            paddleCollisions.emplace_back(CollisionQuery(handle, position, rect));
            return;
        }

        brickCollisions.emplace_back(handle, position, rect);
    });

    Level* level = ecs->GetComponent<Level>(levelEntity);

    // iterate over all the balls and intersect with the bricks, paddles and world
    auto ballPhysicsView = ecs->GetView<Position, Body, Circle>();
    ballPhysicsView.Each([&](const EntityHandle& ballHandle, const Position& position, Body& body, const Circle& circle) {

        Vector2 collisionNormal{};
        for (const CollisionQuery& paddle : paddleCollisions)
        {
            if (Intersection(paddle.position, paddle.rect, position, circle,collisionNormal))
            {
                const float paddleCenter = paddle.position.x + paddle.rect.width * 0.5f;
                float hitOffset = (position.x - paddleCenter) / (paddle.rect.width * 0.5f);

                hitOffset = std::clamp(hitOffset, -1.0f, 1.0f);

                constexpr float maxAngle = 60.0f * (M_PI / 180.0f);

                const float angle = hitOffset * maxAngle;
                const float speed = std::sqrt(body.velocity.x * body.velocity.x + body.velocity.y * body.velocity.y);

                body.velocity.x = speed * std::sin(angle);
                body.velocity.y = -speed * std::cos(angle);

                body.velocity = body.velocity * gainPerPaddleHitScale;
            }
        }

        for (const CollisionQuery& brick : brickCollisions)
        {
            if (ecs->HasComponent<DestroyTag>(brick.handle)) continue;

            if (Intersection(brick.position, brick.rect, position, circle,  collisionNormal))
            {
                body.velocity = Reflect(body.velocity, collisionNormal) * gainPerBrickHitScale;

                DestroyTag doomed{};
                ecs->AddComponent<DestroyTag>(brick.handle, doomed);
                break;
            }
        }

        if (LevelBoundsIntersection(level->dimension, position, circle, collisionNormal))
        {
            body.velocity = Reflect(body.velocity, collisionNormal) * gainPerBoundsHitScale;
        }

        if (position.y > level->dimension.height)
        {
            DestroyTag doomed{};
            ecs->AddComponent<DestroyTag>(ballHandle, doomed);
        }
    });

    auto doomedView = ecs->GetView<DestroyTag>();
    doomedView.Each([&](const EntityHandle handle, const DestroyTag& destroyTag)
    {
        ecs->DestroyEntity(handle);
    });
}

bool PhysicsSystem::Intersection(
    const Position& paddlePosition, const Dimension& paddleDimension, const Position& ballPosition,
    const Circle& circle, Vector2& outNormal)
{
    float closestX = std::max(paddlePosition.x, std::min(ballPosition.x, paddlePosition.x + paddleDimension.width));
    float closestY = std::max(paddlePosition.y, std::min(ballPosition.y, paddlePosition.y + paddleDimension.height));

    float dx = ballPosition.x - closestX;
    float dy = ballPosition.y - closestY;
    float distSq = dx * dx + dy * dy;
    float radius = circle.radius;

    if (distSq < radius * radius && distSq > 0.0f)
    {
        float dist = std::sqrt(distSq);
        outNormal = Vector2(dx / dist, dy / dist); // normalized direction
        return true;
    }
    return false;
}

bool PhysicsSystem::LevelBoundsIntersection(
    const Dimension& boundsDimension,
    const Position& ballPosition,
    const Circle& circle,
    Vector2& outNormal)
{
    bool collided = false;
    outNormal = Vector2(0.0f, 0.0f);

    if (ballPosition.x - circle.radius < 0.0f)
    {
        outNormal = Vector2(1.0f, 0.0f);
        collided = true;
    }

    else if (ballPosition.x + circle.radius > boundsDimension.width)
    {
        outNormal = Vector2(-1.0f, 0.0f);
        collided = true;
    }

    else if (ballPosition.y - circle.radius < 0.0f)
    {
        outNormal = Vector2(0.0f, 1.0f);
        collided = true;
    }

    if (collided)
    {
        outNormal = outNormal.Normalized();
    }

    return collided;
}

void PhysicsSystem::Shutdown()
{
}

Vector2 PhysicsSystem::Reflect(const Vector2& velocity, const Vector2& normal)
{
    float dot = velocity.x * normal.x + velocity.y * normal.y;
    return velocity - normal * (2.0f * dot);
}

