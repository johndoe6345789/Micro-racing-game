#ifndef FRONTEND_H
#define FRONTEND_H

#include <SDL3/SDL.h>
#include <string>

class Frontend {
public:
    virtual ~Frontend() = default;

    virtual bool initialize() = 0;
    virtual bool createWindow(const std::string& title, int width, int height, bool resizable) = 0;
    virtual bool pollEvent(SDL_Event& event) = 0;
    virtual void delay(Uint32 ms) = 0;
    virtual void clear(int r, int g, int b, int a) = 0;
    virtual void present() = 0;
    virtual SDL_Renderer* getRenderer() = 0;
    virtual void cleanup() = 0;
};

class SDLFrontend : public Frontend {
public:
    SDLFrontend();
    ~SDLFrontend() override;

    bool initialize() override;
    bool createWindow(const std::string& title, int width, int height, bool resizable) override;
    bool pollEvent(SDL_Event& event) override;
    void delay(Uint32 ms) override;
    void clear(int r, int g, int b, int a) override;
    void present() override;
    SDL_Renderer* getRenderer() override;
    void cleanup() override;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif // FRONTEND_H
