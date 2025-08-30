#include "design_manager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

DesignManager::DesignManager() {
    loadDefaultSettings();
}

void DesignManager::loadDefaultSettings() {
    // Default UI Settings
    boolSettings["ui.showHand"] = true;
    boolSettings["ui.showDebugInfo"] = false;
    boolSettings["ui.showRecipeHints"] = true;
    stringSettings["ui.handPosition"] = "bottom";
    stringSettings["ui.handSize"] = "medium";
    
    // Default Animation Settings
    boolSettings["animations.enableCardHover"] = true;
    boolSettings["animations.enablePickupAnimation"] = true;
    boolSettings["animations.enableDragAnimation"] = true;
    floatSettings["animations.animationSpeed"] = 1.0f;
    
    // Default Gameplay Settings
    boolSettings["gameplay.enableAutoRecipes"] = false;
    boolSettings["gameplay.enableCardStacking"] = true;
    boolSettings["gameplay.showCardTypes"] = true;
    floatSettings["gameplay.maxStackSize"] = 30.0f; // Maximum number of cards allowed in a stack by default
    
    // Default Visual Settings
    stringSettings["visual.theme"] = "mtg";
    boolSettings["visual.cardBorders"] = true;
    boolSettings["visual.playmatBorders"] = true;
    boolSettings["visual.shadowEffects"] = true;
}

bool DesignManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[DESIGN] Could not open design file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || (line.length() >= 2 && line.substr(0, 2) == "//")) {
            continue;
        }
        
        // Parse section headers like "ui": { BEFORE checking for brackets
        if (line.find(':') != std::string::npos && line.find('{') != std::string::npos) {
            size_t colonPos = line.find(':');
            currentSection = line.substr(0, colonPos);
            // Remove quotes and whitespace
            currentSection.erase(std::remove(currentSection.begin(), currentSection.end(), '"'), currentSection.end());
            currentSection.erase(std::remove(currentSection.begin(), currentSection.end(), ' '), currentSection.end());
            continue;
        }
        
        // Handle other JSON-like structure (standalone brackets)
        if (line.find('{') != std::string::npos || line.find('}') != std::string::npos) {
            continue; // Skip brackets
        }
        
        // Parse key-value pairs
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Clean up key and value
            key.erase(std::remove(key.begin(), key.end(), '"'), key.end());
            key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t,") + 1);
            value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
            
            // Create full key with section
            std::string fullKey = currentSection + "." + key;
            
            // Determine value type and store
            if (value == "true" || value == "false") {
                boolSettings[fullKey] = parseBool(value);
            } else if (value.find('.') != std::string::npos || 
                      (value.find_first_not_of("0123456789.-") == std::string::npos && !value.empty())) {
                floatSettings[fullKey] = parseFloat(value);
            } else {
                stringSettings[fullKey] = value;
            }
        }
    }
    
    file.close();
    std::cout << "[DESIGN] Loaded settings from " << filename << std::endl;
    return true;
}

bool DesignManager::parseBool(const std::string& value) {
    std::string lowerValue = value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    return lowerValue == "true" || lowerValue == "1" || lowerValue == "yes";
}

float DesignManager::parseFloat(const std::string& value) {
    try {
        return std::stof(value);
    } catch (const std::exception&) {
        return 0.0f;
    }
}

bool DesignManager::getBool(const std::string& key) const {
    auto it = boolSettings.find(key);
    return (it != boolSettings.end()) ? it->second : false;
}

float DesignManager::getFloat(const std::string& key) const {
    auto it = floatSettings.find(key);
    return (it != floatSettings.end()) ? it->second : 0.0f;
}

std::string DesignManager::getString(const std::string& key) const {
    auto it = stringSettings.find(key);
    return (it != stringSettings.end()) ? it->second : "";
}
