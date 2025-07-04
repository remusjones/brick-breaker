//
// Created by Remus on 4/07/2025.
//

#include "InputManager.h"

void InputManager::ProcessInput(const SDL_MouseButtonEvent& mouseEvent)
{
    // we might want to pass the mouse event along to the function, but for now we won't
    if (mouseClickEvents.contains(mouseEvent.button))
    {
        for (auto& event : mouseClickEvents[mouseEvent.button])
        {
            event.second();
        }
    }
}

CallbackID InputManager::RegisterMouseClickEvent(uint8_t mouseButton, const std::function<void()> &callback)
{
    size_t callbackId = nextCallbackId++;
    mouseClickEvents[mouseButton].emplace_back(callbackId, callback);

    return callbackId;
}

void InputManager::UnregisterMouseClickEvent(uint8_t mouseButton, CallbackID callbackId)
{
    auto& vec = mouseClickEvents[mouseButton];
    std::erase_if(vec, [callbackId](const auto &pair) { return pair.first == callbackId; });
}