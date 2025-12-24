#include "renderer.h"
#include <cmath>

Renderer::Renderer(SDL_Renderer* sdlRenderer)
    : renderer(sdlRenderer)
{
}

Renderer::~Renderer() {
}

void Renderer::drawLine(float x1, float y1, float x2, float y2, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderLine(renderer, x1, y1, x2, y2);
}

void Renderer::drawRect(float x, float y, float w, float h, int r, int g, int b, bool filled) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_FRect rect = { x, y, w, h };
    
    if (filled) {
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_RenderRect(renderer, &rect);
    }
}

void Renderer::drawCircle(float cx, float cy, float radius, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        drawCirclePoints(cx, cy, x, y);
        
        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void Renderer::drawCirclePoints(int cx, int cy, int x, int y) {
    SDL_RenderPoint(renderer, cx + x, cy + y);
    SDL_RenderPoint(renderer, cx - x, cy + y);
    SDL_RenderPoint(renderer, cx + x, cy - y);
    SDL_RenderPoint(renderer, cx - x, cy - y);
    SDL_RenderPoint(renderer, cx + y, cy + x);
    SDL_RenderPoint(renderer, cx - y, cy + x);
    SDL_RenderPoint(renderer, cx + y, cy - x);
    SDL_RenderPoint(renderer, cx - y, cy - x);
}

void Renderer::drawPolygon(SDL_FPoint* points, int count, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    // Draw filled polygon using scanline algorithm (simplified)
    // For now, just draw the outline
    for (int i = 0; i < count - 1; ++i) {
        SDL_RenderLine(renderer, points[i].x, points[i].y, 
                      points[i + 1].x, points[i + 1].y);
    }
    SDL_RenderLine(renderer, points[count - 1].x, points[count - 1].y,
                  points[0].x, points[0].y);
}

void Renderer::renderText(const std::string& text, int x, int y, int r, int g, int b, float scale) {
    // Simple bitmap-style text rendering
    // For a real game, use SDL_ttf
    // For now, just draw a placeholder rectangle
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    int charWidth = 8 * scale;
    int charHeight = 12 * scale;
    
    for (size_t i = 0; i < text.length(); ++i) {
        SDL_FRect rect = { 
            x + i * charWidth, 
            y, 
            charWidth - 2, 
            charHeight 
        };
        SDL_RenderRect(renderer, &rect);
    }
}
