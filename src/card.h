#pragma once

#include "common.h"

// Forward declaration
class ColorManager;

class Card {
private:
    CardType type;
    Vector2 position;
    Vector2 basePosition;  // Original position for animation
    Vector2 size;
    CardState state;
    float animationOffset; // Y offset for pickup animation
    
public:
    Card(CardType t, Vector2 pos);
    
    void render(SDL_Renderer* renderer, const ColorManager& colorManager) const;
    void update();
    
    bool containsPoint(Vector2 point) const;
    
    Vector2 getPosition() const { return position; }
    Vector2 getBasePosition() const { return basePosition; }
    void setPosition(Vector2 pos) { position = pos; basePosition = pos; }
    void setBasePosition(Vector2 pos) { basePosition = pos; }
    Vector2 getSize() const { return size; }
    CardType getType() const { return type; }
    CardState getState() const { return state; }
    void setState(CardState s) { state = s; }
    
    // Animation methods
    void setAnimationOffset(float offset) { animationOffset = offset; }
    float getAnimationOffset() const { return animationOffset; }
    
private:
    void renderRoundedRect(SDL_Renderer* renderer, SDL_FRect rect, float radius, SDL_Color color, bool filled) const;
};
