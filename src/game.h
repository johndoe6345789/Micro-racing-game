#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include "menu.h"
#include "car.h"
#include "track.h"
#include "camera.h"
#include "ai_bot.h"
#include "renderer.h"
#include "input.h"

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    SETTINGS,
    QUIT
};

class Game {
public:
    Game();
    ~Game();
    
    bool initialize();
    void run();
    void cleanup();
    
private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    
    SDL_Window* window;
    SDL_Renderer* sdlRenderer;
    
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Menu> menu;
    std::unique_ptr<Input> input;
    std::unique_ptr<Track> track;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Car> playerCar;
    std::vector<std::unique_ptr<AIBot>> aiBots;
    
    GameState state;
    bool running;
    
    int screenWidth;
    int screenHeight;
    
    // Settings
    bool soundEnabled;
    int difficulty;
    
    void startGame(const std::string& trackName);
    void returnToMenu();
};

#endif // GAME_H
