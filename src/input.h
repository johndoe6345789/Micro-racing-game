#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

class Input {
public:
    Input();
    virtual ~Input() = default;
    
    virtual void handleEvent(const SDL_Event& event);
    
    virtual bool isForward() const;
    virtual bool isBackward() const;
    virtual bool isLeft() const;
    virtual bool isRight() const;
    
protected:
    const Uint8* keyState;
};

#endif // INPUT_H
