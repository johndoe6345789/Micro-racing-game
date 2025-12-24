#include "menu.h"

Menu::Menu(Renderer* renderer)
    : renderer(renderer)
    , selectedIndex(0)
{
    menuItems = {
        "Start Game",
        "Map Editor",
        "Settings",
        "Quit"
    };
}

void Menu::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
            case SDLK_W:
                selectPrevious();
                break;
            case SDLK_DOWN:
            case SDLK_S:
                selectNext();
                break;
            default:
                break;
        }
    }
}

MenuAction Menu::update() {
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    if (keyState[SDL_SCANCODE_RETURN] || keyState[SDL_SCANCODE_SPACE]) {
        return activate();
    }
    
    return MenuAction::NONE;
}

void Menu::render() {
    // Render title
    renderer->renderText("MICRO RACING GAME", 400, 100, 255, 215, 0, 2.0f);
    
    // Render menu items
    int startY = 300;
    int spacing = 60;
    
    for (size_t i = 0; i < menuItems.size(); ++i) {
        int y = startY + i * spacing;
        
        if (i == selectedIndex) {
            // Highlighted item
            renderer->renderText("> " + menuItems[i] + " <", 450, y, 255, 255, 0, 1.5f);
        } else {
            // Normal item
            renderer->renderText(menuItems[i], 470, y, 200, 200, 200, 1.2f);
        }
    }
    
    // Instructions
    renderer->renderText("Use Arrow Keys or W/S to navigate", 380, 600, 150, 150, 150);
    renderer->renderText("Press ENTER or SPACE to select", 390, 630, 150, 150, 150);
}

void Menu::selectNext() {
    selectedIndex = (selectedIndex + 1) % menuItems.size();
}

void Menu::selectPrevious() {
    selectedIndex = (selectedIndex - 1 + menuItems.size()) % menuItems.size();
}

MenuAction Menu::activate() {
    switch (selectedIndex) {
        case 0: return MenuAction::START_GAME;
        case 1: return MenuAction::MAP_EDITOR;
        case 2: return MenuAction::SETTINGS;
        case 3: return MenuAction::QUIT;
        default: return MenuAction::NONE;
    }
}
