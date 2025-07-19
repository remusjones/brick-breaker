//
// Created by Remus on 29/06/2025.
//

#include "Display.h"
#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>

Display::Display() : renderer(nullptr), window(nullptr), windowHeight(0), windowWidth(0), levelWidth(0), levelHeight(0)
{
}

SDL_AppResult Display::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) == false)
    {
        SDL_Log("Failed to get initialize video: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    const SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(displayID);
    if (mode == nullptr)
    {
        SDL_Log("Failed to get display mode: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    windowHeight = mode->h;
    windowWidth = mode->w;

    if (!SDL_CreateWindowAndRenderer("Brick Breaker", windowWidth, windowHeight,
        SDL_WINDOW_FULLSCREEN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_GRABBED, &window, &renderer))
    {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

void Display::PreRender()
{
    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Display::Render()
{
    // Calculate scale factors
    float scaleX = static_cast<float>(windowWidth) / levelWidth;
    float scaleY = static_cast<float>(windowHeight) / levelHeight;

    SDL_SetRenderScale(renderer, scaleX, scaleY);

    for (const auto& brick : rectDrawList)
    {
        SDL_SetRenderDrawColor(renderer, brick.color.r, brick.color.g, brick.color.b, brick.color.a);
        SDL_RenderFillRect(renderer, &brick.rect);
    }
    // todo: use the color component
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (const auto& ball : ballDrawList)
    {
        SDL_RenderLines(renderer, ball.points.data(), ball.points.size());
    }

    for (const auto& text : textDrawList)
    {
        SDL_RenderDebugText(renderer, levelWidth / 2, levelHeight / 2, text.data());
    }

    SDL_RenderPresent(renderer);
}

void Display::PostRender()
{
    ballDrawList.clear();
    rectDrawList.clear();
    textDrawList.clear();
}

void Display::SetLevelSize(const float x, const float y)
{
    levelWidth = x;
    levelHeight = y;
}