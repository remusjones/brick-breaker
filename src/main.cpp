#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL_main.h>
#include <memory>
#include "Game.h"

static std::unique_ptr<Game> game;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    game = std::make_unique<Game>();
    return game->Start();
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    game->WindowEvent(event);
    return game->GetState();
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    return game->Update();
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    game->Shutdown();
}