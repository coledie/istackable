#pragma once

#include "common.h"
#include "card.h"
#include "board.h"
#include "color_manager.h"
#include "design_manager.h"
#include <vector>

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    
    std::vector<Card> cards;        // Cards on the playmat
    std::vector<Card> handCards;    // Cards in player's hand
    std::vector<Recipe> recipes;
    Board board;
    ColorManager colorManager;
    DesignManager designManager;
    
    // Click and animation state
    Card* animatingCard;
    float animationTimer;
    float animationDuration;
    Vector2 lastClickPos;  // For debugging
    Card* lastClickedCard; // For debugging
    
    // Drag state
    Card* draggingCard;
    Vector2 dragOffset;    // Offset from card position to mouse when drag started
    bool isDragging;
    
    // Stacking state
    bool isOverStackTarget;     // True if currently dragged card is overlapping a stack target > threshold
    int stackTargetIndex;       // Index of the card being hovered as stack target
    float stackOverlapThreshold; // Fraction (0-1) of overlap required to snap/stack (default 0.5)
    float stackVisualOffsetY;   // Vertical offset per stacked card when rendering (visual)
    float stackVisualOffsetX;   // Horizontal offset per stacked card when rendering (visual, to the right)
    
    // Hand drag state
    Card* draggingHandCard;    // Hand card being dragged
    Vector2 handCardOriginalPos; // Original position to return to if cancelled
    bool isDraggingFromHand;   // Flag to distinguish hand vs playmat drags
    
    // Hand state
    Card* hoveredHandCard;  // Card currently being hovered in hand
    float handCardScale;    // Scale factor for hovered card
    Vector2 handArea;       // Position and size of hand area
    float handCardSpacing;  // Spacing between cards in hand
    
public:
    Game();
    ~Game();
    
    bool init();
    void run();
    void cleanup();
    
private:
    void handleEvents();
    void update();
    void render();
    
    void initializeCards();
    void initializeHand();
    void initializeRecipes();
    void processRecipes();
    
    // Click and animation methods
    Card* getCardAt(Vector2 pos);
    Card* getHandCardAt(Vector2 pos);
    void bringCardToFront(Card* card);
    void startPickupAnimation(Card* card);
    void renderDebugInfo(SDL_Renderer* renderer);
    
    // Drag methods
    void startDrag(Card* card, Vector2 mousePos);
    void updateDrag(Vector2 mousePos);
    void stopDrag();
    
    // Hand card drag methods
    void startHandCardDrag(Card* handCard, Vector2 mousePos);
    void updateHandCardDrag(Vector2 mousePos);
    void stopHandCardDrag();
    bool isOverPlaymat(Vector2 mousePos); // Helper to check if position is over playmat
    
    // Hand methods
    void updateHandHover(Vector2 mousePos);
    void renderHand(SDL_Renderer* renderer);
    void playCardFromHand(Card* handCard, Vector2 position);

    // Stacking helpers
    int getStackCountAtBasePosition(const Vector2& basePos) const;
    int findOverlapTargetIndex(const Card* sourceCard, float requiredFraction) const;
    void finalizeStacking(int targetIndex, int sourceIndex);
    
    // Color management
    const ColorManager& getColorManager() const { return colorManager; }
};
