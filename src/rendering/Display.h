//
// Created by Remus on 29/06/2025.
//


#pragma once
#include "CoreTypes.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include <cmath>
#include <vector>



struct DrawRect
{
    DrawRect(const Position& position, const float& width, const float& height, const SDL_Color& color)
           : rect{ position.x, position.y, width, height }, color(color)
    {
    }

    SDL_FRect rect;
    SDL_Color color;
};

struct DrawBall
{
    DrawBall(const Position& position, const float& radius)
    {
        constexpr int32_t segments = 16;
        for (int32_t i = 0; i < segments; ++i)
        {
            float theta = 2.0f * M_PI * i / segments;
            SDL_FPoint point;
            point.x = position.x + radius * cosf(theta);
            point.y = position.y + radius * sinf(theta);
            points.emplace_back(point);
        }
        points.push_back(points.front());
    }

    std::vector<SDL_FPoint> points;
};

class Display
{
public:
    Display();

    SDL_AppResult Init();

    void PreRender();
    void Render();
    void PostRender();

    SDL_Renderer* renderer;
    SDL_Window* window;

    std::vector<DrawRect> rectDrawList;
    std::vector<DrawBall> ballDrawList;

    int GetWindowWidth() const { return windowWidth; }
    int GetWindowHeight() const {return windowHeight; }

    void SetLevelSize(float x, float y);

private:
    int windowHeight;
    int windowWidth;

    float levelWidth;
    float levelHeight;
};
