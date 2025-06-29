//
// Created by Remus on 29/06/2025.
//

#include "Display.h"
#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>

Display::Display() : renderer(nullptr), window(nullptr), windowHeight(0), windowWidth(0)
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
    Position pos(100, 100);
    DrawBall ball1(pos, 2);

    Position pos2(1, 100);
    DrawRect brick1 (pos2, 10, 5, SDL_Color(255, 0, 0, 255));

    ballDrawList.emplace_back(ball1);

    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}
void Display::Render()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (const auto& ball : ballDrawList)
    {
        SDL_RenderLines(renderer, ball.points.data(), ball.points.size());
    }

    for (const auto& brick : rectDrawList)
    {
        SDL_SetRenderDrawColor(renderer, brick.color.r, brick.color.g, brick.color.b, brick.color.a);
        SDL_RenderFillRect(renderer, &brick.rect);
    }

    SDL_RenderPresent(renderer);

}
void Display::PostRender()
{
    ballDrawList.clear();
    rectDrawList.clear();
}