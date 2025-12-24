#include "game.h"
#include <iostream>
#include <chrono>

Game::Game()
    : state(GameState::MENU)
    , running(false)
    , screenWidth(1280)
    , screenHeight(720)
    , soundEnabled(true)
    , difficulty(1)
{
}

Game::~Game() {
    cleanup();
}

bool Game::initialize() {
    frontend = std::make_unique<SDLFrontend>();
    if (!frontend->initialize()) {
        return false;
    }

    if (!frontend->createWindow("Micro Racing Game", screenWidth, screenHeight, true)) {
        return false;
    }
    
    // Initialize subsystems
    renderer = std::make_unique<Renderer>(frontend->getRenderer());
    menu = std::make_unique<Menu>(renderer.get());
    input = std::make_unique<Input>();
    camera = std::make_unique<Camera>(screenWidth, screenHeight);
    
    running = true;
    return true;
}

void Game::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Cap delta time to avoid large jumps
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        handleEvents();
        update(deltaTime);
        render();

        frontend->delay(1); // Small delay to prevent 100% CPU usage
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (frontend->pollEvent(event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
            return;
        }
        
        if (state == GameState::MENU) {
            menu->handleEvent(event);
        } else if (state == GameState::PLAYING) {
            input->handleEvent(event);
            
            // Pause with Escape key
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                state = GameState::PAUSED;
            }
        } else if (state == GameState::PAUSED) {
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                state = GameState::PLAYING;
            }
        }
    }
}

void Game::update(float deltaTime) {
    if (state == GameState::MENU) {
        MenuAction action = menu->update();
        
        switch (action) {
            case MenuAction::START_GAME:
                startGame("track1.json");
                break;
            case MenuAction::SETTINGS:
                state = GameState::SETTINGS;
                break;
            case MenuAction::QUIT:
                running = false;
                break;
            case MenuAction::NONE:
            default:
                break;
        }
    } else if (state == GameState::PLAYING) {
        if (playerCar && track) {
            // Update player car
            playerCar->update(deltaTime, *input);
            
            // Update AI bots
            for (auto& bot : aiBots) {
                bot->update(deltaTime, *track);
            }
            
            // Update camera to follow player
            camera->followTarget(playerCar->getX(), playerCar->getY());
            camera->update(deltaTime);
            
            // Check collisions with track boundaries
            track->checkCollisions(*playerCar);
            for (auto& bot : aiBots) {
                track->checkCollisions(bot->getCar());
            }
        }
    }
}

void Game::render() {
    frontend->clear(20, 20, 20, 255);
    
    if (state == GameState::MENU) {
        menu->render();
    } else if (state == GameState::PLAYING || state == GameState::PAUSED) {
        if (track && playerCar) {
            // Apply camera transform
            camera->apply(frontend->getRenderer());
            
            // Render track
            track->render(*renderer, *camera);
            
            // Render cars
            playerCar->render(*renderer, *camera);
            for (auto& bot : aiBots) {
                bot->render(*renderer, *camera);
            }
            
            // Reset camera transform
            camera->reset(frontend->getRenderer());
            
            // Render UI
            renderer->renderText("Lap: 1/3", 10, 10, 255, 255, 255);
            
            if (state == GameState::PAUSED) {
                renderer->renderText("PAUSED - Press ESC to continue", 
                    screenWidth / 2 - 150, screenHeight / 2, 255, 255, 0);
            }
        }
    }

    frontend->present();
}

void Game::startGame(const std::string& trackName) {
    // Load track
    track = std::make_unique<Track>();
    if (!track->loadFromFile("tracks/" + trackName)) {
        std::cerr << "Failed to load track: " << trackName << std::endl;
        returnToMenu();
        return;
    }
    
    // Create player car
    auto startPos = track->getStartPosition(0);
    playerCar = std::make_unique<Car>(startPos.x, startPos.y, 0, 255, 0);
    
    // Create AI bots
    aiBots.clear();
    for (int i = 1; i <= 3; ++i) {
        auto aiStartPos = track->getStartPosition(i);
        auto bot = std::make_unique<AIBot>(aiStartPos.x, aiStartPos.y, difficulty);
        aiBots.push_back(std::move(bot));
    }
    
    // Initialize camera at player position
    camera->setPosition(playerCar->getX(), playerCar->getY());
    
    state = GameState::PLAYING;
}

void Game::returnToMenu() {
    playerCar.reset();
    track.reset();
    aiBots.clear();
    state = GameState::MENU;
}

void Game::cleanup() {
    if (frontend) {
        frontend->cleanup();
        frontend.reset();
    }
}
