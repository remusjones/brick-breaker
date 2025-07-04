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
    DrawRect(const Vector2& position, const Dimension& rect, const SDL_Color& color)
           : rect{ position.x, position.y, rect.width, rect.height }, color(color)
    {
    }

    SDL_FRect rect;
    SDL_Color color;
};

struct DrawBall
{
    DrawBall(const Vector2& position, const Circle& circle)
    {
        int32_t segments = 8 * circle.radius;
        for (int32_t i = 0; i < segments; ++i)
        {
            float theta = 2.0f * M_PI * i / segments;
            SDL_FPoint point;
            point.x = position.x + circle.radius * cosf(theta);
            point.y = position.y + circle.radius * sinf(theta);
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
    std::vector<std::string_view> textDrawList;

    int GetWindowWidth() const { return windowWidth; }
    int GetWindowHeight() const {return windowHeight; }

    void SetLevelSize(float x, float y);

private:
    int windowHeight;
    int windowWidth;

    float levelWidth;
    float levelHeight;
};
