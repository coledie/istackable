#include "card.h"
#include <cstdio>
#include "debug.h"
#include "color_manager.h"

Card::Card(CardType t, Vector2 pos) : type(t), position(pos), basePosition(pos), size({95, 132}), 
                                     state(CardState::IDLE), animationOffset(0.0f) {}

void Card::render(SDL_Renderer* renderer, const ColorManager& colorManager) const {
    // Get MTG-themed color for this card type
    Color cardColor = colorManager.getCardColor(type);
    
    // Apply animation offset to Y position
    Vector2 renderPos = Vector2(position.x, position.y - animationOffset);
    
    // Render white border (card frame)
    renderRoundedRect(renderer, 
                     {renderPos.x, renderPos.y, size.x, size.y}, 
                     8.0f, 
                     {255, 255, 255, 255}, 
                     true);
    
    // Render card color (inner area)
    renderRoundedRect(renderer, 
                     {renderPos.x + 3, renderPos.y + 3, size.x - 6, size.y - 6}, 
                     5.0f, 
                     cardColor.toSDL(), 
                     true);
}

void Card::update() {
    // Update logic here if needed
}

bool Card::containsPoint(Vector2 point) const {
    bool contains = point.x >= position.x && point.x <= position.x + size.x &&
                   point.y >= position.y && point.y <= position.y + size.y;
    
    // Debug output
    if (contains) {
        DEBUG_CARD("Card type %d at (%.1f, %.1f) contains point (%.1f, %.1f)\n", 
               (int)type, position.x, position.y, point.x, point.y);
    }
    
    return contains;
}

void Card::renderRoundedRect(SDL_Renderer* renderer, SDL_FRect rect, float radius, SDL_Color color, bool filled) const {
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
