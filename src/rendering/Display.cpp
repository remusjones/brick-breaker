//
// Created by Remus on 29/06/2025.
//

#include "Display.h"
#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>

void Display::PreRender()
{
    Position pos(100, 100);
    DrawBall ball1(pos, 2);

    Position pos2(400, 100);
    DrawBrick brick1 (pos2, 10, 5, SDL_Color(255, 0, 0, 255));

    ballDrawList.emplace_back(ball1);
    brickDrawList.emplace_back(brick1);


    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}
void Display::Render()
{
    const char *message = "Hello World!";
    int w = 0, h = 0;
    float x, y;
    const float scale = 4.0f;

    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, x, y, message);

    for (const auto& ball : ballDrawList)
    {
        SDL_RenderLines(renderer, ball.points.data(), ball.points.size());
    }

    for (const auto& brick : brickDrawList)
    {
        SDL_SetRenderDrawColor(renderer, brick.color.r, brick.color.g, brick.color.b, brick.color.a);
        SDL_RenderFillRect(renderer, &brick.rect);
    }

    SDL_RenderPresent(renderer);

}
void Display::PostRender()
{
    ballDrawList.clear();
    brickDrawList.clear();
}