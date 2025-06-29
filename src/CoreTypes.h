//
// Created by Remus on 29/06/2025.
//
#pragma once

struct Position
{
    float x;
    float y;
};

struct Rect
{
    float height;
    float width;
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

struct MouseInput
{
    float mousePositionX;
};

//struct Physics
//{
//
//};