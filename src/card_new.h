#pragma once

#include "common.h"

class Card {
private:
    CardType type;
    Vector2 position;
    Vector2 size;
    CardState state;
    
public:
    Card(CardType t, Vector2 pos);
    
    void render(SDL_Renderer* renderer) const;
    void update();
    
    bool containsPoint(Vector2 point) const;
    
    Vector2 getPosition() const { return position; }
    void setPosition(Vector2 pos) { position = pos; }
    Vector2 getSize() const { return size; }
    CardType getType() const { return type; }
    CardState getState() const { return state; }
    void setState(CardState s) { state = s; }
    
private:
    void renderRoundedRect(SDL_Renderer* renderer, SDL_FRect rect, float radius, SDL_Color color, bool filled) const;
};
