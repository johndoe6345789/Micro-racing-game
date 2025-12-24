#ifndef EDITOR_H
#define EDITOR_H

#include <SDL2/SDL.h>
#include <memory>
#include "track.h"
#include "renderer.h"
#include "camera.h"

enum class EditorTool {
    PLACE_TRACK,
    PLACE_WALL,
    PLACE_JUMP,
    PLACE_START,
    ERASE,
    MOVE_CAMERA
};

class Editor {
public:
    Editor();
    ~Editor();
    
    bool initialize();
    void run();
    void cleanup();
    
private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    
    void handleMouseClick(int x, int y, bool rightClick);
    void saveTrack();
    void loadTrack();
    
    SDL_Window* window;
    SDL_Renderer* sdlRenderer;
    
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Track> track;
    std::unique_ptr<Camera> camera;
    
    EditorTool currentTool;
    bool running;
    
    int screenWidth;
    int screenHeight;
    
    int mouseX, mouseY;
    bool isDragging;
    
    std::string currentTrackName;
};

#endif // EDITOR_H
