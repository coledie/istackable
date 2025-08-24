#include "board.h"
#include "color_manager.h"
#include <cmath>

Board::Board() {
    borderWidth = 20.0f;
    playArea = {borderWidth, borderWidth, 1920 - 2 * borderWidth, 1080 - 2 * borderWidth};
}

void Board::render(SDL_Renderer* renderer, const ColorManager& colorManager) {
    // Create outer border with rounded corners using config colors
    SDL_FRect outerRect = {0, 0, 1920, 1080};
    Color borderLight = colorManager.getBorderLight();
    renderRoundedRect(renderer, outerRect, 30.0f, borderLight.toSDL(), true);
    
    // Create inner play area with tan playmat color
    SDL_FRect innerRect = playArea;
    Color playmatColor = colorManager.getBackgroundColor();
    renderRoundedRect(renderer, innerRect, 20.0f, playmatColor.toSDL(), true);
}

bool Board::isWithinPlayArea(Vector2 pos, Vector2 size) const {
    return (pos.x >= playArea.x && 
            pos.y >= playArea.y && 
            pos.x + size.x <= playArea.x + playArea.w && 
            pos.y + size.y <= playArea.y + playArea.h);
}

Vector2 Board::constrainToPlayArea(Vector2 pos, Vector2 size) const {
    Vector2 constrained = pos;
    
    if (constrained.x < playArea.x) constrained.x = playArea.x;
    if (constrained.y < playArea.y) constrained.y = playArea.y;
    if (constrained.x + size.x > playArea.x + playArea.w) constrained.x = playArea.x + playArea.w - size.x;
    if (constrained.y + size.y > playArea.y + playArea.h) constrained.y = playArea.y + playArea.h - size.y;
    
    return constrained;
}

void Board::renderRoundedRect(SDL_Renderer* renderer, SDL_FRect rect, float radius, SDL_Color color, bool filled) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    if (filled) {
        // Fill center rectangle
        SDL_FRect centerRect = {rect.x, rect.y + radius, rect.w, rect.h - 2 * radius};
        SDL_RenderFillRect(renderer, &centerRect);
        
        // Fill left and right rectangles
        SDL_FRect leftRect = {rect.x + radius, rect.y, rect.w - 2 * radius, rect.h};
        SDL_RenderFillRect(renderer, &leftRect);
        
        // Fill rounded corners
        for (int y = 0; y < (int)radius; y++) {
            for (int x = 0; x < (int)radius; x++) {
                float dx = radius - x - 0.5f;
                float dy = radius - y - 0.5f;
                if (dx * dx + dy * dy <= radius * radius) {
                    // Top-left corner
                    SDL_RenderPoint(renderer, rect.x + x, rect.y + y);
                    // Top-right corner
                    SDL_RenderPoint(renderer, rect.x + rect.w - 1 - x, rect.y + y);
                    // Bottom-left corner
                    SDL_RenderPoint(renderer, rect.x + x, rect.y + rect.h - 1 - y);
                    // Bottom-right corner
                    SDL_RenderPoint(renderer, rect.x + rect.w - 1 - x, rect.y + rect.h - 1 - y);
                }
            }
        }
    }
}
