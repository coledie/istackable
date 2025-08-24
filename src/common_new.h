#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <cmath>

// Forward declarations
class Game;
class Card;
class Board;

// TTF_Font placeholder since SDL3_ttf might not be available
struct TTF_Font;

// Card types enum
enum class CardType {
    VILLAGER,
    WOOD,
    ROCK,
    BERRY,
    BRANCH,
    LOG,
    PLANK,
    STICK
};

enum class CardState {
    IDLE,
    DRAGGING
};

struct Vector2 {
    float x, y;
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
};

struct Recipe {
    std::vector<CardType> ingredients;
    CardType result;
};
