//
// Created by Remus on 29/06/2025.
//
#pragma once
#include "ECS/CoreTypes.h"
#include "InputManager.h"
#include <cmath>

#define M_PI 3.14159265358979323846

struct Vector2
{
    float x;
    float y;

    Vector2 operator+(const Vector2& other) const
    {
        return Vector2{x + other.x, y + other.y};
    }

    [[nodiscard]] Vector2 Normalized() const
    {
        const float length = std::sqrt(x * x + y * y);
        if (length == 0.0f)
        {
            return Vector2{0.0f, 0.0f};
        }
        return Vector2{x / length, y / length};
    }

    Vector2 operator*(const float& other) const
    {
        return Vector2{x * other, y * other};
    }

    Vector2 operator-(const Vector2 & other) const
    {
        return {x - other.x, y - other.y};
    }

};

using Position = Vector2;

struct Attached
{
    EntityHandle parent = INVALID_ENTITY;
    Position offset;
};

struct Body
{
    Vector2 velocity;
};

struct Circle
{
    float radius;
};

struct Dimension
{
    float width;
    float height;
};

struct Level
{
    Dimension dimension;

    CallbackID ballReleaseCallbackHandle;
};

struct Powerup
{
    enum PowerupType
    {
        None,
        DoubleBall
    };

    PowerupType brickType = PowerupType::None;
};

struct Color
{
    int r, g, b, a;
};

struct Paddle
{
    float paddleSpeed;
    float mousePositionX;

    CallbackID paddleFireMouseCallbackID;
};

struct DestroyTag
{
    int dummy;
};