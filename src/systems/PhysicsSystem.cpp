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
    constexpr float gainPerPaddleHitScale = 0.99f;
    constexpr float gainPerBrickHitScale = 1.02f;
    constexpr float gainPerBoundsHitScale = 1.0f; // Adjust if you want bounds to affect speed


    // update all velocities
    // todo: good use-case for a .without filter to ignore attached bodies
    auto velocityView = ecs->GetView<Position, Body>();
    velocityView.Each([&](const EntityHandle& handle, Position& position, Body & body) {

        position.x += body.velocity.x * deltaTime;
        position.y += body.velocity.y * deltaTime;
    });

    // update attachments
    // this only really works if there is 1 level of attachment, and that's probably fine for this
    auto attachedView = ecs->GetView<Position, Attached>();
    attachedView.Each([&](const EntityHandle& handle, Position& position, const Attached& attached) {
        if (const Position* parentPosition = ecs->GetComponent<Position>(attached.parent))
        {
            position = *parentPosition + attached.offset;
        }
    });

    struct CollisionQuery
    {
        EntityHandle handle;
        Position position;
        Dimension rect;
    };

    std::unordered_map<EntityHandle, CollisionQuery> paddleCollisions;
    auto paddleCollision = ecs->GetView<Position, Dimension, Paddle>();
    paddleCollision.Each([&](const EntityHandle& handle, const Position & position, const Dimension & rect, const Paddle& paddle) {
        paddleCollisions[handle] = CollisionQuery(handle, position, rect);
    });

    // todo: another use-case for a .without in the ecs
    std::vector<CollisionQuery> brickCollisions;
    auto rectCollisionView = ecs->GetView<Position, Dimension>();
    rectCollisionView.Each([&](const EntityHandle& handle, const Position& position, const Dimension& rect) {

        // don't add paddles to the brick filter
        if (paddleCollisions.contains(handle)) return;

        brickCollisions.emplace_back(handle, position, rect);
    });

    Level* level = ecs->GetComponent<Level>(levelEntity);
    auto ballPhysicsView = ecs->GetView<Position, Body, Circle>();
    ballPhysicsView.Each([&](const EntityHandle& handle, Position& position, Body& body, const Circle & circle) {

        for (const CollisionQuery& paddle : paddleCollisions | std::views::values)
        {
            Vector2 collisionNormal;
            if (Intersection(paddle.position, paddle.rect, position, circle,collisionNormal))
            {
                 float paddleCenter = paddle.position.x + paddle.rect.width * 0.5f;
                 float hitOffset = (position.x - paddleCenter) / (paddle.rect.width * 0.5f);

                 hitOffset = std::clamp(hitOffset, -1.0f, 1.0f);

                 constexpr float maxAngle = 60.0f * (M_PI / 180.0f);

                 float angle = hitOffset * maxAngle;
                 float speed = std::sqrt(body.velocity.x * body.velocity.x + body.velocity.y * body.velocity.y);

                 body.velocity.x = speed * std::sin(angle);
                 body.velocity.y = -speed * std::cos(angle);

                 body.velocity = body.velocity * gainPerPaddleHitScale;
            }
        }

        for (const CollisionQuery& brick : brickCollisions)
        {
            Vector2 collisionNormal;
            if (Intersection(brick.position, brick.rect, position, circle,  collisionNormal))
            {
                body.velocity = Reflect(body.velocity, collisionNormal) * gainPerBrickHitScale;
                // todo: make this add a component instead, and handle drops in a different system before destroying it
                ecs->DestroyEntity(brick.handle);
                break;
            }
        }

        Vector2 collisionNormal;
        if (LevelBoundsIntersection(level->dimension, position, circle, collisionNormal))
        {
            body.velocity = Reflect(body.velocity, collisionNormal) * gainPerBoundsHitScale;

            // Lock balls to playing space if they are intersecting or outside of the wall
            position.x = std::clamp<float>(position.x, 0, level->dimension.width);
            position.y = std::clamp<float>(position.y, 0, std::numeric_limits<float>::infinity());
        }

        if (position.y > level->dimension.height)
        {
            // Ball has left the level, and will be destroyed
            ecs->DestroyEntity(handle);
        }
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

