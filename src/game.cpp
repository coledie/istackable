#include "game.h"
#include <cstdio>
#include "debug.h"

Game::Game() : window(nullptr), renderer(nullptr), running(false), 
               animatingCard(nullptr), animationTimer(0.0f), animationDuration(0.3f),
               lastClickPos(Vector2(0, 0)), lastClickedCard(nullptr),
               draggingCard(nullptr), dragOffset(Vector2(0, 0)), isDragging(false),
               isOverStackTarget(false), stackTargetIndex(-1), stackOverlapThreshold(0.5f), stackVisualOffsetY(8.0f), stackVisualOffsetX(6.0f),
               draggingHandCard(nullptr), handCardOriginalPos(Vector2(0, 0)), isDraggingFromHand(false),
               hoveredHandCard(nullptr), handCardScale(1.0f), handArea(Vector2(480, 1014)), 
               handCardSpacing(120.0f) {}

Game::~Game() {
    cleanup();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }
    
    window = SDL_CreateWindow("MTG Card Game", 1920, 1080, SDL_WINDOW_RESIZABLE);
    if (!window) {
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        return false;
    }
    
    // Load color configuration
    colorManager.loadFromFile("../config/colors.conf");
    
    // Load design configuration
    designManager.loadFromFile("../config/design.json");
    
    // Debug: Show key design settings
    printf("[DESIGN] Hand enabled: %s\n", designManager.getShowHand() ? "true" : "false");
    printf("[DESIGN] Hover animation: %s\n", designManager.getEnableCardHover() ? "true" : "false");
    
    initializeCards();
    initializeHand();
    initializeRecipes();
    
    running = true;
    return true;
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
    }
}

void Game::cleanup() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            running = false;
        }
        else if (e.type == SDL_EVENT_KEY_DOWN) {
            // Close the game when Escape is pressed
            if (e.key.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
            }
        }
        else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                Vector2 mousePos = Vector2((float)e.button.x, (float)e.button.y);
                lastClickPos = mousePos;
                
                DEBUG_CLICK("Mouse down at: (%.1f, %.1f)\n", mousePos.x, mousePos.y);
                
                // Check if we clicked a hand card first (only if hand is enabled)
                Card* clickedHandCard = nullptr;
                if (designManager.getShowHand()) {
                    clickedHandCard = getHandCardAt(mousePos);
                }
                
                if (clickedHandCard) {
                    DEBUG_CLICK("Starting drag from hand card type: %d\n", (int)clickedHandCard->getType());
                    // Start dragging the hand card instead of immediately playing it
                    startHandCardDrag(clickedHandCard, mousePos);
                } else {
                    // Check for playmat cards
                    Card* clickedCard = getCardAt(mousePos);
                    if (clickedCard) {
                        lastClickedCard = clickedCard;
                        
                        DEBUG_CLICK("Starting drag on playmat card type: %d\n", (int)clickedCard->getType());
                        
                        bringCardToFront(clickedCard);
                        startDrag(clickedCard, mousePos);
                    } else {
                        DEBUG_CLICK("No card found at click position\n");
                    }
                }
            }
        }
        else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                if (isDraggingFromHand) {
                    DEBUG_DRAG("Mouse up - stopping hand card drag\n");
                    stopHandCardDrag();
                } else if (isDragging) {
                    DEBUG_DRAG("Mouse up - stopping playmat drag\n");
                    stopDrag();
                }
            }
        }
        else if (e.type == SDL_EVENT_MOUSE_MOTION) {
            Vector2 mousePos = Vector2((float)e.motion.x, (float)e.motion.y);
            if (isDraggingFromHand) {
                updateHandCardDrag(mousePos);
            } else if (isDragging) {
                updateDrag(mousePos);
            } else if (designManager.getShowHand()) {
                // Update hand hover when not dragging and hand is enabled
                updateHandHover(mousePos);
            }
        }
    }
}

void Game::update() {
    // Update animation
    if (animatingCard) {
        animationTimer += 0.016f; // Assuming ~60 FPS
        
        if (animationTimer >= animationDuration) {
            // Animation finished
            animatingCard->setAnimationOffset(0.0f);
            animatingCard->setState(CardState::IDLE);
            animatingCard = nullptr;
            animationTimer = 0.0f;
        } else {
            // Calculate animation progress (0 to 1)
            float progress = animationTimer / animationDuration;
            
            // Use sine wave for smooth bounce effect
            float bounceHeight = 20.0f;
            float offset = bounceHeight * sin(progress * 3.14159f); // Full sine wave (0 to 1)
            
            animatingCard->setAnimationOffset(offset);
        }
    }
    
    for (auto& card : cards) {
        card.update();
    }
    processRecipes();
}

void Game::render() {
    // Use tan background color from config
    Color bgColor = colorManager.getBackgroundColor();
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(renderer);
    
    board.render(renderer, colorManager);
    
    for (const auto& card : cards) {
        card.render(renderer, colorManager);
    }
    
    // Render hand after playmat cards but before debug info (if enabled)
    if (designManager.getShowHand()) {
        renderHand(renderer);
    }
    
    renderDebugInfo(renderer);
    
    SDL_RenderPresent(renderer);
}

void Game::initializeCards() {
    cards.push_back(Card(CardType::VILLAGER, {100, 100}));
    cards.push_back(Card(CardType::WOOD, {200, 100}));
    cards.push_back(Card(CardType::ROCK, {300, 100}));
    cards.push_back(Card(CardType::BERRY, {400, 100}));
    cards.push_back(Card(CardType::BRANCH, {500, 100}));
    cards.push_back(Card(CardType::LOG, {600, 100}));
    cards.push_back(Card(CardType::PLANK, {700, 100}));
    cards.push_back(Card(CardType::STICK, {800, 100}));
}

void Game::initializeHand() {
    // Add one card of each type to the hand
    float startX = handArea.x;
    float y = handArea.y;
    
    handCards.push_back(Card(CardType::VILLAGER, {startX + 0 * handCardSpacing, y}));
    handCards.push_back(Card(CardType::WOOD, {startX + 1 * handCardSpacing, y}));
    handCards.push_back(Card(CardType::ROCK, {startX + 2 * handCardSpacing, y}));
    handCards.push_back(Card(CardType::BERRY, {startX + 3 * handCardSpacing, y}));
    handCards.push_back(Card(CardType::BRANCH, {startX + 4 * handCardSpacing, y}));
    handCards.push_back(Card(CardType::LOG, {startX + 5 * handCardSpacing, y}));
    handCards.push_back(Card(CardType::PLANK, {startX + 6 * handCardSpacing, y}));
    handCards.push_back(Card(CardType::STICK, {startX + 7 * handCardSpacing, y}));
}

void Game::initializeRecipes() {
    Recipe recipe1 = {{CardType::VILLAGER, CardType::BERRY}, CardType::VILLAGER};
    Recipe recipe2 = {{CardType::VILLAGER, CardType::WOOD}, CardType::VILLAGER};
    Recipe recipe3 = {{CardType::BRANCH, CardType::ROCK}, CardType::STICK};
    Recipe recipe4 = {{CardType::LOG, CardType::VILLAGER}, CardType::PLANK};
    Recipe recipe5 = {{CardType::WOOD, CardType::WOOD}, CardType::LOG};
    
    recipes.push_back(recipe1);
    recipes.push_back(recipe2);
    recipes.push_back(recipe3);
    recipes.push_back(recipe4);
    recipes.push_back(recipe5);
}

void Game::processRecipes() {
    for (const auto& recipe : recipes) {
        for (int i = 0; i < cards.size(); i++) {
            for (int j = i + 1; j < cards.size(); j++) {
                if (cards[i].getState() == CardState::IDLE && cards[j].getState() == CardState::IDLE) {
                    bool matches = false;
                    
                    if ((cards[i].getType() == recipe.ingredients[0] && cards[j].getType() == recipe.ingredients[1]) ||
                        (cards[i].getType() == recipe.ingredients[1] && cards[j].getType() == recipe.ingredients[0])) {
                        
                        Vector2 pos1 = cards[i].getPosition();
                        Vector2 pos2 = cards[j].getPosition();
                        Vector2 size = cards[i].getSize();
                        
                        float dx = pos1.x - pos2.x;
                        float dy = pos1.y - pos2.y;
                        float distance = sqrt(dx * dx + dy * dy);
                        
                        if (distance < 50.0f) {
                            matches = true;
                        }
                    }
                    
                    if (matches) {
                        Vector2 newPos = Vector2((cards[i].getPosition().x + cards[j].getPosition().x) / 2,
                                                (cards[i].getPosition().y + cards[j].getPosition().y) / 2);
                        
                        cards.erase(cards.begin() + j);
                        cards.erase(cards.begin() + i);
                        
                        cards.push_back(Card(recipe.result, newPos));
                        return;
                    }
                }
            }
        }
    }
}

Card* Game::getCardAt(Vector2 pos) {
    for (int i = cards.size() - 1; i >= 0; i--) {
        if (cards[i].containsPoint(pos)) {
            return &cards[i];
        }
    }
    return nullptr;
}

void Game::bringCardToFront(Card* card) {
    for (int i = 0; i < cards.size(); i++) {
        if (&cards[i] == card) {
            Card temp = cards[i];
            cards.erase(cards.begin() + i);
            cards.push_back(temp);
            break;
        }
    }
}

void Game::startPickupAnimation(Card* card) {
    // Stop any existing animation
    if (animatingCard) {
        animatingCard->setAnimationOffset(0.0f);
        animatingCard->setState(CardState::IDLE);
    }
    
    // Start new animation
    animatingCard = card;
    animationTimer = 0.0f;
    card->setState(CardState::ANIMATING);
    
    DEBUG_ANIMATION("Starting animation for card type: %d\n", (int)card->getType());
}

void Game::renderDebugInfo(SDL_Renderer* renderer) {
#ifdef DEBUG_MODE
    // Draw red cross at last click position
    if (lastClickPos.x != 0 || lastClickPos.y != 0) {
        Color clickColor = colorManager.getClickIndicator();
        SDL_SetRenderDrawColor(renderer, clickColor.r, clickColor.g, clickColor.b, clickColor.a);
        
        // Draw a cross at click position
        for (int i = -5; i <= 5; i++) {
            SDL_RenderPoint(renderer, lastClickPos.x + i, lastClickPos.y);
            SDL_RenderPoint(renderer, lastClickPos.x, lastClickPos.y + i);
        }
    }
    
    // Highlight the currently animating card with a border
    if (animatingCard) {
        Color animColor = colorManager.getAnimationBorder();
        SDL_SetRenderDrawColor(renderer, animColor.r, animColor.g, animColor.b, animColor.a);
        Vector2 pos = animatingCard->getPosition();
        Vector2 size = animatingCard->getSize();
        float offset = animatingCard->getAnimationOffset();
        
        SDL_FRect rect = {pos.x - 2, pos.y - offset - 2, size.x + 4, size.y + 4};
        SDL_RenderRect(renderer, &rect);
    }
    
    // Highlight the currently dragging card with a border
    if (draggingCard) {
        Color dragColor = colorManager.getDragBorder();
        SDL_SetRenderDrawColor(renderer, dragColor.r, dragColor.g, dragColor.a);
        Vector2 pos = draggingCard->getPosition();
        Vector2 size = draggingCard->getSize();
        float offset = draggingCard->getAnimationOffset();
        
        SDL_FRect rect = {pos.x - 3, pos.y - offset - 3, size.x + 6, size.y + 6};
        SDL_RenderRect(renderer, &rect);
    }
#endif
}

void Game::startDrag(Card* card, Vector2 mousePos) {
    // Stop any existing animation
    if (animatingCard) {
        animatingCard->setAnimationOffset(0.0f);
        animatingCard->setState(CardState::IDLE);
        animatingCard = nullptr;
    }
    
    // Update card pointer after potential reordering in bringCardToFront
    card = &cards.back(); // The card is now at the end of the vector
    
    // Calculate drag offset (how far the mouse is from the card's top-left corner)
    Vector2 cardPos = card->getPosition();
    dragOffset = Vector2(mousePos.x - cardPos.x, mousePos.y - cardPos.y);
    
    // Set up drag state
    draggingCard = card;
    isDragging = true;
    
    // Set card to dragging state with lift effect
    card->setState(CardState::DRAGGING);
    card->setAnimationOffset(20.0f); // Lift the card up
    
    // Reset stacking state while starting drag
    isOverStackTarget = false;
    stackTargetIndex = -1;
    
    DEBUG_DRAG("Drag started with offset: (%.1f, %.1f)\n", dragOffset.x, dragOffset.y);
}

void Game::updateDrag(Vector2 mousePos) {
    if (draggingCard && isDragging) {
        // Calculate new position: mouse position minus the drag offset
        Vector2 newPos = Vector2(mousePos.x - dragOffset.x, mousePos.y - dragOffset.y);
        draggingCard->setPosition(newPos);

        // If stacking is enabled, check if we're overlapping another card enough to snap/stack
        if (designManager.getEnableCardStacking()) {
            int targetIdx = findOverlapTargetIndex(draggingCard, stackOverlapThreshold);
            if (targetIdx != -1) {
                // Check stack size limit
                Vector2 targetBase = cards[targetIdx].getBasePosition();
                int currentStack = getStackCountAtBasePosition(targetBase);
                if (currentStack < designManager.getMaxStackSize()) {
                    isOverStackTarget = true;
                    stackTargetIndex = targetIdx;

                    // Snap visually to top of stack (downwards and to the right)
                    int snapIndex = currentStack; // new card will be the topmost (0-based)
                    Vector2 snapPos = Vector2(targetBase.x + (snapIndex * stackVisualOffsetX), targetBase.y + (snapIndex * stackVisualOffsetY));
                    draggingCard->setPosition(snapPos);
                } else {
                    // Stack full - do not snap
                    isOverStackTarget = false;
                    stackTargetIndex = -1;
                }
            } else {
                isOverStackTarget = false;
                stackTargetIndex = -1;
            }
        }
    }
}

void Game::stopDrag() {
    if (draggingCard && isDragging) {
        // Drop the card down
        draggingCard->setAnimationOffset(0.0f);
        draggingCard->setState(CardState::IDLE);
        
        DEBUG_DRAG("Drag stopped for card type: %d\n", (int)draggingCard->getType());
        
        // If we were snapping to a stack target, finalize the stacking
        if (isOverStackTarget && stackTargetIndex != -1 && designManager.getEnableCardStacking()) {
            // Find source index of draggingCard in the vector
            int sourceIndex = -1;
            for (int i = 0; i < cards.size(); i++) {
                if (&cards[i] == draggingCard) { sourceIndex = i; break; }
            }
            if (sourceIndex != -1) {
                finalizeStacking(stackTargetIndex, sourceIndex);
            }
        }
        
        // Clear drag state
        draggingCard = nullptr;
        isDragging = false;
        dragOffset = Vector2(0, 0);
        isOverStackTarget = false;
        stackTargetIndex = -1;
    }
}

void Game::startHandCardDrag(Card* handCard, Vector2 mousePos) {
    if (!handCard) return;
    
    // Store original position to return to if cancelled
    handCardOriginalPos = handCard->getPosition();
    
    // Calculate drag offset (how far the mouse is from the card's top-left corner)
    Vector2 cardPos = handCard->getPosition();
    dragOffset = Vector2(mousePos.x - cardPos.x, mousePos.y - cardPos.y);
    
    // Set up hand drag state
    draggingHandCard = handCard;
    isDraggingFromHand = true;
    
    // Set card to dragging state with lift effect
    handCard->setState(CardState::DRAGGING);
    handCard->setAnimationOffset(20.0f); // Lift the card up
    
    DEBUG_DRAG("Hand card drag started with offset: (%.1f, %.1f)\n", dragOffset.x, dragOffset.y);
}

void Game::stopHandCardDrag() {
    if (draggingHandCard && isDraggingFromHand) {
        float mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Vector2 currentMousePos = Vector2(mouseX, mouseY);
        
        if (isOverPlaymat(currentMousePos)) {
            // Drop on playmat - create new card and reset hand card position
            Vector2 dropPos = draggingHandCard->getPosition();

            if (isOverStackTarget && stackTargetIndex != -1 && designManager.getEnableCardStacking()) {
                // If stacking target exists, and under limit, create new card and finalize stacking
                // Create the new card at dropPos and append to cards
                cards.push_back(Card(draggingHandCard->getType(), dropPos));
                int sourceIndex = (int)cards.size() - 1;
                finalizeStacking(stackTargetIndex, sourceIndex);
                DEBUG_DRAG("Hand card stacked on existing stack at index %d\n", stackTargetIndex);
            } else {
                playCardFromHand(draggingHandCard, dropPos);
                DEBUG_DRAG("Hand card dropped on playmat at (%.1f, %.1f)\n", dropPos.x, dropPos.y);
            }
        } else {
            DEBUG_DRAG("Hand card drag cancelled - returning to hand\n");
        }
        
        // Return hand card to original position and state
        draggingHandCard->setPosition(handCardOriginalPos);
        draggingHandCard->setAnimationOffset(0.0f);
        draggingHandCard->setState(CardState::IDLE);
        
        // Clear hand drag state
        draggingHandCard = nullptr;
        isDraggingFromHand = false;
        dragOffset = Vector2(0, 0);
        isOverStackTarget = false;
        stackTargetIndex = -1;
    }
}

bool Game::isOverPlaymat(Vector2 mousePos) {
    // Check if mouse position is over the playmat area (not over the hand)
    // Hand area is at the bottom, so anything above a certain Y threshold is playmat
    float handTop = handArea.y - 50.0f; // Some margin above the hand
    return mousePos.y < handTop;
}

Card* Game::getHandCardAt(Vector2 pos) {
    for (int i = handCards.size() - 1; i >= 0; i--) {
        if (handCards[i].containsPoint(pos)) {
            return &handCards[i];
        }
    }
    return nullptr;
}

void Game::updateHandHover(Vector2 mousePos) {
    Card* newHoveredCard = getHandCardAt(mousePos);
    
    if (newHoveredCard != hoveredHandCard) {
        // Reset previous hovered card
        if (hoveredHandCard) {
            hoveredHandCard->setAnimationOffset(0.0f);
            hoveredHandCard->setState(CardState::IDLE);
        }
        
        // Set new hovered card
        hoveredHandCard = newHoveredCard;
        if (hoveredHandCard) {
            hoveredHandCard->setAnimationOffset(66.0f); // Rise up by 66px (half the card height)
            hoveredHandCard->setState(CardState::ANIMATING);
            DEBUG_PRINT("Hand card hovered: type %d\n", (int)hoveredHandCard->getType());
        }
    }
}

void Game::renderHand(SDL_Renderer* renderer) {
    // Only set clipping rectangle if we're not dragging a hand card
    // This allows the dragged card to be visible anywhere on screen
    if (!isDraggingFromHand) {
        // Set clipping rectangle to allow full card visibility when hovered
        // Cards start at y=1014, hover to y=948, so clip from y=948
        SDL_Rect clipRect = {0, 948, 1920, 132}; // Show from y=948 to bottom of screen
        SDL_SetRenderClipRect(renderer, &clipRect);
    }
    
    // Render all hand cards
    for (const auto& card : handCards) {
        card.render(renderer, colorManager);
    }
    
    // Remove clipping for rest of the rendering (only if we set it)
    if (!isDraggingFromHand) {
        SDL_SetRenderClipRect(renderer, nullptr);
    }
}

void Game::playCardFromHand(Card* handCard, Vector2 position) {
    if (!handCard) return;
    
    // Create a new card on the playmat
    Card newCard(handCard->getType(), position);
    cards.push_back(newCard);
    
    // Note: In a full game, we might remove the card from hand or have limited uses
    // For now, we keep the hand card as an infinite source
    
    DEBUG_PRINT("Played card type %d from hand to playmat\n", (int)handCard->getType());
}

// Stacking helper implementations
int Game::getStackCountAtBasePosition(const Vector2& basePos) const {
    int count = 0;
    const float epsilon = 0.1f;
    for (const auto& c : cards) {
        Vector2 bp = c.getBasePosition();
        if (fabs(bp.x - basePos.x) < epsilon && fabs(bp.y - basePos.y) < epsilon) {
            count++;
        }
    }
    return count;
}

int Game::findOverlapTargetIndex(const Card* sourceCard, float requiredFraction) const {
    if (!sourceCard) return -1;

    Vector2 sPos = sourceCard->getPosition();
    Vector2 sSize = sourceCard->getSize();
    float sLeft = sPos.x, sTop = sPos.y, sRight = sPos.x + sSize.x, sBottom = sPos.y + sSize.y;

    // Search from topmost to bottom so we pick the visible top card first
    for (int i = (int)cards.size() - 1; i >= 0; i--) {
        const Card& target = cards[i];
        // If sourceCard is actually this same card (by address), skip
        if (&target == sourceCard) continue;

        Vector2 tPos = target.getPosition();
        Vector2 tSize = target.getSize();
        float tLeft = tPos.x, tTop = tPos.y, tRight = tPos.x + tSize.x, tBottom = tPos.y + tSize.y;

        float interLeft = fmax(sLeft, tLeft);
        float interTop = fmax(sTop, tTop);
        float interRight = fmin(sRight, tRight);
        float interBottom = fmin(sBottom, tBottom);

        float interW = interRight - interLeft;
        float interH = interBottom - interTop;

        if (interW > 0 && interH > 0) {
            float interArea = interW * interH;
            float targetArea = tSize.x * tSize.y;
            float fraction = interArea / targetArea;
            if (fraction >= requiredFraction) {
                return i;
            }
        }
    }
    return -1;
}

void Game::finalizeStacking(int targetIndex, int sourceIndex) {
    if (targetIndex < 0 || sourceIndex < 0 || targetIndex >= (int)cards.size() || sourceIndex >= (int)cards.size()) return;
    if (targetIndex == sourceIndex) return;

    // Determine the base position of the target stack
    Vector2 targetBase = cards[targetIndex].getBasePosition();

    // Count current cards in the stack
    int currentStack = getStackCountAtBasePosition(targetBase);
    if (currentStack >= designManager.getMaxStackSize()) {
        return; // Stack is full
    }

    // Extract the source card
    Card temp = cards[sourceIndex];

    // Remove the source card from the vector
    cards.erase(cards.begin() + sourceIndex);

    // If removing an element before the target, the target index shifts left by 1
    if (sourceIndex < targetIndex) targetIndex -= 1;

    // Anchor the source card's base position to the target base
    temp.setBasePosition(targetBase);

    // Append the card to the end so it becomes topmost
    cards.push_back(temp);

    // Recalculate stack ordering and visual offsets for all cards sharing the same base position
    std::vector<int> stackIndices;
    const float epsilon = 0.1f;
    for (int i = 0; i < (int)cards.size(); i++) {
        Vector2 bp = cards[i].getBasePosition();
        if (fabs(bp.x - targetBase.x) < epsilon && fabs(bp.y - targetBase.y) < epsilon) {
            stackIndices.push_back(i);
            // Ensure the base position is consistent
            cards[i].setBasePosition(targetBase);
        }
    }

    // Apply visual offsets based on stack order (bottom -> top)
    for (int k = 0; k < (int)stackIndices.size(); k++) {
        int idx = stackIndices[k];
        Vector2 pos = Vector2(targetBase.x + (k * stackVisualOffsetX), targetBase.y + (k * stackVisualOffsetY));
        cards[idx].setPosition(pos);
    }
}

void Game::updateHandCardDrag(Vector2 mousePos) {
    if (draggingHandCard && isDraggingFromHand) {
        // Calculate new position: mouse position minus the drag offset
        Vector2 newPos = Vector2(mousePos.x - dragOffset.x, mousePos.y - dragOffset.y);
        draggingHandCard->setPosition(newPos);

        // While dragging from hand, check for stack snap targets as well
        if (designManager.getEnableCardStacking()) {
            // Create a temporary card at the hand card position to test overlaps
            Card temp(draggingHandCard->getType(), draggingHandCard->getPosition());
            int targetIdx = findOverlapTargetIndex(&temp, stackOverlapThreshold);
            if (targetIdx != -1) {
                Vector2 targetBase = cards[targetIdx].getBasePosition();
                int currentStack = getStackCountAtBasePosition(targetBase);
                if (currentStack < designManager.getMaxStackSize()) {
                    isOverStackTarget = true;
                    stackTargetIndex = targetIdx;
                    Vector2 snapPos = Vector2(targetBase.x + (currentStack * stackVisualOffsetX), targetBase.y + (currentStack * stackVisualOffsetY));
                    draggingHandCard->setPosition(snapPos);
                } else {
                    isOverStackTarget = false;
                    stackTargetIndex = -1;
                }
            } else {
                isOverStackTarget = false;
                stackTargetIndex = -1;
            }
        }
    }
}
