//
// Created by Remus on 29/06/2025.
//

#include "PhysicsSystem.h"
#include "CoreTypes.h"
#include "ECS/HelloECS.h"
#include <cmath>
#include <ranges>

PhysicsSystem::PhysicsSystem(const std::string_view &name, HelloECS *inECS)
    : System(name), ecs(inECS), levelEntity(0)
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
    // Do nothing
}

void PhysicsSystem::FixedUpdate(float fixedDeltaTime)
{
    // Ignore iteration outside of fixed delta step
    paddleCollisions.clear();
    brickCollisions.clear();

    // basic configurations for the collision behaviour
    constexpr float gainPerPaddleHitScale = 1.005f;
    constexpr float gainPerBrickHitScale = 1.0f;
    constexpr float gainPerBoundsHitScale = 1.0f;

    // update paddle location based on last mouse location
    auto inputView = ecs->GetView<Position, Paddle, Dimension>();
    inputView.Each([&](const EntityHandle& entityHandle, Position& position, const Paddle& paddle, const Dimension& rect) {
        position.x = std::lerp(position.x, paddle.mousePositionX - rect.width / 2, paddle.paddleSpeed * fixedDeltaTime);
    });

    // update all velocities
    auto velocityView = ecs->GetView<Position, Body>();
    velocityView.Each([&](const EntityHandle& handle, Position& position, Body& body) {
        position.x += body.velocity.x * fixedDeltaTime;
        position.y += body.velocity.y * fixedDeltaTime;

        if (ecs->HasComponent<Attached>(handle))
        {
            Attached* attached = ecs->GetComponent<Attached>(handle);
            const Position* parentPosition = ecs->GetComponent<Position>(attached->parent);
            position = *parentPosition + attached->offset;
        }
    });

    // Find all potential collisions and apply them to collision arrays
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
    ballPhysicsView.Each([&](const EntityHandle& ballHandle, Position& position, Body& body, const Circle& circle) {

        Vector2 collisionNormal{};
        for (const CollisionQuery &paddle : paddleCollisions)
        {
            if (Intersection(paddle.position, paddle.rect, position, circle, collisionNormal))
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

        // iterate over all the bricks, and collision test them
        // todo: consider adding this to a spatial system to avoid O(balls*bricks) complexity, scales poorly when stress
        // testing with a large ball amount
        for (const CollisionQuery& brick : brickCollisions)
        {
            if (ecs->HasComponent<DestroyTag>(brick.handle))
                continue;

            if (Intersection(brick.position, brick.rect, position, circle, collisionNormal))
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

            // Penetration test and separate the ball to avoid velocity normalization error where ball appears to hug
            // the ceiling as it moves along it
            float penetration = 0.0f;
            if (position.x - circle.radius < 0.0f)
            {
                penetration = 0.0f - (position.x - circle.radius);
            }
            else if (position.x + circle.radius > level->dimension.width)
            {
                penetration = (position.x + circle.radius) - level->dimension.width;
            }
            else if (position.y - circle.radius < 0.0f)
            {
                penetration = 0.0f - (position.y - circle.radius);
            }
            position.x += collisionNormal.x * penetration;
            position.y += collisionNormal.y * penetration;
        }

        if (position.y > level->dimension.height)
        {
            DestroyTag doomed{};
            ecs->AddComponent<DestroyTag>(ballHandle, doomed);
        }
    });

    // Iterate all entities that are pending destroy from collisions and destroy them here
    auto doomedView = ecs->GetView<DestroyTag>();
    doomedView.Each([&](const EntityHandle handle, const DestroyTag& destroyTag) { ecs->DestroyEntity(handle); });
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

