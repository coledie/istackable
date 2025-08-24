#include "card.h"

Card::Card(CardType t, Vector2 pos) : type(t), position(pos), size({95, 132}), state(CardState::IDLE) {}

void Card::render(SDL_Renderer* renderer) const {
    SDL_Color cardColor;
    switch (type) {
        case CardType::VILLAGER:
            cardColor = {255, 182, 193, 255}; // Light pink
            break;
        case CardType::WOOD:
            cardColor = {139, 69, 19, 255}; // Saddle brown
            break;
        case CardType::ROCK:
            cardColor = {105, 105, 105, 255}; // Dim gray
            break;
        case CardType::BERRY:
            cardColor = {220, 20, 60, 255}; // Crimson
            break;
        case CardType::BRANCH:
            cardColor = {160, 82, 45, 255}; // Sienna
            break;
        case CardType::LOG:
            cardColor = {101, 67, 33, 255}; // Dark brown
            break;
        case CardType::PLANK:
            cardColor = {205, 133, 63, 255}; // Peru
            break;
        case CardType::STICK:
            cardColor = {210, 180, 140, 255}; // Tan
            break;
    }
    
    renderRoundedRect(renderer, 
                     {position.x, position.y, size.x, size.y}, 
                     8.0f, 
                     {255, 255, 255, 255}, 
                     true);
    
    renderRoundedRect(renderer, 
                     {position.x + 3, position.y + 3, size.x - 6, size.y - 6}, 
                     5.0f, 
                     cardColor, 
                     true);
}

void Card::update() {
    // Update logic here if needed
}

bool Card::containsPoint(Vector2 point) const {
    return point.x >= position.x && point.x <= position.x + size.x &&
           point.y >= position.y && point.y <= position.y + size.y;
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
