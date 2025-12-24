#include "input.h"

Input::Input()
    : keyState(nullptr)
{
    keyState = SDL_GetKeyboardState(nullptr);
}

void Input::handleEvent(const SDL_Event& event) {
    // Update key state
    keyState = SDL_GetKeyboardState(nullptr);
}

bool Input::isForward() const {
    return keyState && (keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W]);
}

bool Input::isBackward() const {
    return keyState && (keyState[SDL_SCANCODE_DOWN] || keyState[SDL_SCANCODE_S]);
}

bool Input::isLeft() const {
    return keyState && (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]);
}

bool Input::isRight() const {
    return keyState && (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]);
}
