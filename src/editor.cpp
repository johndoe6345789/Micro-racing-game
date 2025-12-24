#include "editor.h"
#include <iostream>
#include <chrono>

Editor::Editor()
    : window(nullptr)
    , sdlRenderer(nullptr)
    , currentTool(EditorTool::PLACE_TRACK)
    , running(false)
    , screenWidth(1280)
    , screenHeight(720)
    , mouseX(0), mouseY(0)
    , isDragging(false)
    , currentTrackName("new_track.json")
{
}

Editor::~Editor() {
    cleanup();
}

bool Editor::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow(
        "Track Editor - Micro Racing Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight,
        SDL_WINDOW_RESIZABLE
    );
    
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlRenderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = std::make_unique<Renderer>(sdlRenderer);
    track = std::make_unique<Track>();
    camera = std::make_unique<Camera>(screenWidth, screenHeight);
    
    running = true;
    return true;
}

void Editor::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        
        handleEvents();
        update(deltaTime);
        render();
        
        SDL_Delay(1);
    }
}

void Editor::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    handleMouseClick(event.button.x, event.button.y, false);
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    handleMouseClick(event.button.x, event.button.y, true);
                }
                break;
                
            case SDL_MOUSEMOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                
                if (currentTool == EditorTool::MOVE_CAMERA && isDragging) {
                    camera->setPosition(
                        camera->getX() - event.motion.xrel,
                        camera->getY() - event.motion.yrel
                    );
                }
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        currentTool = EditorTool::PLACE_TRACK;
                        break;
                    case SDLK_2:
                        currentTool = EditorTool::PLACE_WALL;
                        break;
                    case SDLK_3:
                        currentTool = EditorTool::PLACE_JUMP;
                        break;
                    case SDLK_4:
                        currentTool = EditorTool::PLACE_START;
                        break;
                    case SDLK_E:
                        currentTool = EditorTool::ERASE;
                        break;
                    case SDLK_SPACE:
                        currentTool = EditorTool::MOVE_CAMERA;
                        isDragging = true;
                        break;
                    case SDLK_S:
                        if (SDL_GetModState() & KMOD_CTRL) {
                            saveTrack();
                        }
                        break;
                    case SDLK_L:
                        if (SDL_GetModState() & KMOD_CTRL) {
                            loadTrack();
                        }
                        break;
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                }
                break;
                
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_SPACE) {
                    isDragging = false;
                }
                break;
        }
    }
}

void Editor::update(float deltaTime) {
    camera->update(deltaTime);
}

void Editor::render() {
    SDL_SetRenderDrawColor(sdlRenderer, 40, 40, 40, 255);
    SDL_RenderClear(sdlRenderer);
    
    // Apply camera
    camera->apply(sdlRenderer);
    
    // Draw grid
    int gridSize = 50;
    for (int x = -500; x < 2000; x += gridSize) {
        float screenX = x - camera->getX();
        renderer->drawLine(screenX, -camera->getY(), 
                          screenX, 1500 - camera->getY(), 60, 60, 60);
    }
    for (int y = -500; y < 2000; y += gridSize) {
        float screenY = y - camera->getY();
        renderer->drawLine(-camera->getX(), screenY,
                          2000 - camera->getX(), screenY, 60, 60, 60);
    }
    
    // Render track
    track->render(*renderer, *camera);
    
    // Draw cursor preview
    float worldX = mouseX + camera->getX();
    float worldY = mouseY + camera->getY();
    
    int tileSize = 50;
    float snapX = std::floor(worldX / tileSize) * tileSize;
    float snapY = std::floor(worldY / tileSize) * tileSize;
    
    int r, g, b;
    switch (currentTool) {
        case EditorTool::PLACE_TRACK:
            r = 80; g = 80; b = 80;
            break;
        case EditorTool::PLACE_WALL:
            r = 100; g = 50; b = 50;
            break;
        case EditorTool::PLACE_JUMP:
            r = 150; g = 75; b = 0;
            break;
        case EditorTool::PLACE_START:
            r = 255; g = 255; b = 255;
            break;
        case EditorTool::ERASE:
            r = 255; g = 0; b = 0;
            break;
        default:
            r = 128; g = 128; b = 128;
    }
    
    float screenX = snapX - camera->getX();
    float screenY = snapY - camera->getY();
    renderer->drawRect(screenX, screenY, tileSize, tileSize, r, g, b, false);
    
    camera->reset(sdlRenderer);
    
    // UI
    renderer->renderText("Track Editor", 10, 10, 255, 255, 255);
    renderer->renderText("1:Track 2:Wall 3:Jump 4:Start E:Erase SPACE:Pan", 10, 40, 200, 200, 200);
    renderer->renderText("Ctrl+S:Save Ctrl+L:Load ESC:Quit", 10, 60, 200, 200, 200);
    
    std::string toolName;
    switch (currentTool) {
        case EditorTool::PLACE_TRACK: toolName = "Track"; break;
        case EditorTool::PLACE_WALL: toolName = "Wall"; break;
        case EditorTool::PLACE_JUMP: toolName = "Jump"; break;
        case EditorTool::PLACE_START: toolName = "Start"; break;
        case EditorTool::ERASE: toolName = "Erase"; break;
        case EditorTool::MOVE_CAMERA: toolName = "Pan"; break;
    }
    renderer->renderText("Tool: " + toolName, 10, 80, 255, 215, 0);
    
    SDL_RenderPresent(sdlRenderer);
}

void Editor::handleMouseClick(int x, int y, bool rightClick) {
    float worldX = x + camera->getX();
    float worldY = y + camera->getY();
    
    int tileSize = 50;
    float snapX = std::floor(worldX / tileSize) * tileSize;
    float snapY = std::floor(worldY / tileSize) * tileSize;
    
    if (rightClick) {
        // Right click to erase
        // Would need to implement tile removal
        return;
    }
    
    switch (currentTool) {
        case EditorTool::PLACE_TRACK:
            track->addTile(TileType::TRACK, snapX, snapY, tileSize, tileSize);
            break;
        case EditorTool::PLACE_WALL:
            track->addTile(TileType::WALL, snapX, snapY, tileSize, tileSize);
            break;
        case EditorTool::PLACE_JUMP:
            track->addTile(TileType::JUMP, snapX, snapY, tileSize, tileSize);
            break;
        case EditorTool::PLACE_START:
            track->addTile(TileType::START_FINISH, snapX, snapY, tileSize, tileSize);
            break;
        default:
            break;
    }
}

void Editor::saveTrack() {
    std::string filename = "tracks/" + currentTrackName;
    if (track->saveToFile(filename)) {
        std::cout << "Track saved to " << filename << std::endl;
    }
}

void Editor::loadTrack() {
    std::string filename = "tracks/" + currentTrackName;
    if (track->loadFromFile(filename)) {
        std::cout << "Track loaded from " << filename << std::endl;
    }
}

void Editor::cleanup() {
    if (sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
}
