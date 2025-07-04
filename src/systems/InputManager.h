//
// Created by Remus on 4/07/2025.
//


#pragma once
#include "SDL3/SDL_events.h"
#include "System.h"

#include <cstdint>
#include <functional>
#include <unordered_map>

using CallbackID = size_t;

class InputManager
{
public:
    void ProcessInput(const SDL_MouseButtonEvent& mouseEvent);

    CallbackID RegisterMouseClickEvent(uint8_t mouseButton, const std::function<void()>& callback);
    void UnregisterMouseClickEvent(uint8_t mouseButton, CallbackID callbackId);

private:

    // we could have another map here to lookup callbackID > functions instead of key > callbackID
    std::unordered_map<uint8_t, std::vector<std::pair<CallbackID, std::function<void()>>>> mouseClickEvents;
    size_t nextCallbackId = 1;
};
