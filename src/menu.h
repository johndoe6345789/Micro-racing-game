#ifndef MENU_H
#define MENU_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "renderer.h"

enum class MenuAction {
    NONE,
    START_GAME,
    MAP_EDITOR,
    SETTINGS,
    QUIT
};

class Menu {
public:
    Menu(Renderer* renderer);
    
    void handleEvent(const SDL_Event& event);
    MenuAction update();
    void render();
    
private:
    Renderer* renderer;
    
    std::vector<std::string> menuItems;
    int selectedIndex;
    
    void selectNext();
    void selectPrevious();
    MenuAction activate();
};

#endif // MENU_H
