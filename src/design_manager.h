#pragma once

#include <string>
#include <map>

class DesignManager {
private:
    std::map<std::string, bool> boolSettings;
    std::map<std::string, float> floatSettings;
    std::map<std::string, std::string> stringSettings;
    
    void loadDefaultSettings();
    bool parseBool(const std::string& value);
    float parseFloat(const std::string& value);
    
public:
    DesignManager();
    bool loadFromFile(const std::string& filename);
    
    // UI Settings
    bool getShowHand() const { return getBool("ui.showHand"); }
    bool getShowDebugInfo() const { return getBool("ui.showDebugInfo"); }
    bool getShowRecipeHints() const { return getBool("ui.showRecipeHints"); }
    std::string getHandPosition() const { return getString("ui.handPosition"); }
    std::string getHandSize() const { return getString("ui.handSize"); }
    
    // Animation Settings
    bool getEnableCardHover() const { return getBool("animations.enableCardHover"); }
    bool getEnablePickupAnimation() const { return getBool("animations.enablePickupAnimation"); }
    bool getEnableDragAnimation() const { return getBool("animations.enableDragAnimation"); }
    float getAnimationSpeed() const { return getFloat("animations.animationSpeed"); }
    
    // Gameplay Settings
    bool getEnableAutoRecipes() const { return getBool("gameplay.enableAutoRecipes"); }
    bool getEnableCardStacking() const { return getBool("gameplay.enableCardStacking"); }
    bool getShowCardTypes() const { return getBool("gameplay.showCardTypes"); }
    
    // Visual Settings
    std::string getTheme() const { return getString("visual.theme"); }
    bool getCardBorders() const { return getBool("visual.cardBorders"); }
    bool getPlaymatBorders() const { return getBool("visual.playmatBorders"); }
    bool getShadowEffects() const { return getBool("visual.shadowEffects"); }
    
    // Generic getters
    bool getBool(const std::string& key) const;
    float getFloat(const std::string& key) const;
    std::string getString(const std::string& key) const;
};
