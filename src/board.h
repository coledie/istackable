#pragma once

#include "common.h"

// Forward declaration
class ColorManager;

class Board {
private:
    SDL_FRect playArea;
    float borderWidth;
    
public:
    Board();
    
    void render(SDL_Renderer* renderer, const ColorManager& colorManager);
    SDL_FRect getPlayArea() const { return playArea; }
    bool isWithinPlayArea(Vector2 pos, Vector2 size) const;
    Vector2 constrainToPlayArea(Vector2 pos, Vector2 size) const;
    
private:
    void renderRoundedRect(SDL_Renderer* renderer, SDL_FRect rect, float radius, SDL_Color color, bool filled);
};
