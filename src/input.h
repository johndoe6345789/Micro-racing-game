#ifndef INPUT_H
#define INPUT_H

#include <SDL3/SDL.h>

class Input {
public:
    Input();
    
    void handleEvent(const SDL_Event& event);
    
    bool isForward() const;
    bool isBackward() const;
    bool isLeft() const;
    bool isRight() const;
    
private:
    const Uint8* keyState;
};

#endif // INPUT_H
