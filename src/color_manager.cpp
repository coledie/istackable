#include "color_manager.h"
#include "debug.h"
#include <fstream>
#include <sstream>
#include <iostream>

ColorManager::ColorManager() {
    loadDefaultColors();
}

bool ColorManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        DEBUG_PRINT("Could not open color config file: %s, using defaults\n", filename.c_str());
        return false;
    }
    
    std::string line;
    std::string currentSection;
    std::vector<std::pair<std::string, std::string>> cardTypeMappings; // Store for later processing
    
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Check for section headers
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Parse key = value pairs
        size_t equalPos = line.find('=');
        if (equalPos != std::string::npos) {
            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);
            
            // Remove whitespace around key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            
            // Remove inline comments from value
            size_t commentPos = value.find('#');
            if (commentPos != std::string::npos) {
                value = value.substr(0, commentPos);
            }
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (currentSection == "Card_Types") {
                // Store card type mappings for later processing
                cardTypeMappings.push_back({key, value});
                DEBUG_PRINT("Card type mapping stored: %s -> %s\n", key.c_str(), value.c_str());
            } else {
                // Check if value is a reference to another color
                if (colors.find(value) != colors.end()) {
                    colors[key] = colors[value];
                    DEBUG_PRINT("Color reference: %s = %s\n", key.c_str(), value.c_str());
                } else {
                    colors[key] = parseColor(value);
                    DEBUG_PRINT("Color parsed: %s = %s\n", key.c_str(), value.c_str());
                }
            }
        }
    }
    
    // Now process card type mappings after all colors are loaded
    for (const auto& mapping : cardTypeMappings) {
        const std::string& key = mapping.first;
        const std::string& value = mapping.second;
        
        if (key == "villager") cardTypeColors[CardType::VILLAGER] = value;
        else if (key == "wood") cardTypeColors[CardType::WOOD] = value;
        else if (key == "rock") cardTypeColors[CardType::ROCK] = value;
        else if (key == "berry") cardTypeColors[CardType::BERRY] = value;
        else if (key == "branch") cardTypeColors[CardType::BRANCH] = value;
        else if (key == "log") cardTypeColors[CardType::LOG] = value;
        else if (key == "plank") cardTypeColors[CardType::PLANK] = value;
        else if (key == "stick") cardTypeColors[CardType::STICK] = value;
        
        DEBUG_PRINT("Card type mapped: %s -> %s\n", key.c_str(), value.c_str());
    }
    
    DEBUG_PRINT("Loaded color configuration from %s\n", filename.c_str());
    return true;
}

Color ColorManager::parseColor(const std::string& colorString) {
    std::stringstream ss(colorString);
    std::string token;
    Color color;
    int values[4] = {0, 0, 0, 255}; // Default alpha to 255
    int index = 0;
    
    while (std::getline(ss, token, ',') && index < 4) {
        values[index] = std::stoi(token);
        index++;
    }
    
    return Color(values[0], values[1], values[2], values[3]);
}

void ColorManager::loadDefaultColors() {
    // Background colors
    colors["playmat"] = Color(139, 119, 101, 255);
    colors["border_light"] = Color(180, 160, 140, 255);
    colors["border_dark"] = Color(100, 85, 70, 255);
    
    // MTG colors
    colors["white"] = Color(249, 250, 244, 255);
    colors["blue"] = Color(14, 104, 171, 255);
    colors["black"] = Color(21, 11, 0, 255);
    colors["red"] = Color(211, 32, 42, 255);
    colors["green"] = Color(0, 115, 62, 255);
    colors["colorless"] = Color(204, 194, 192, 255);
    colors["multicolor"] = Color(255, 189, 0, 255);
    
    // UI colors
    colors["border_normal"] = Color(50, 50, 50, 255);
    colors["border_selected"] = Color(255, 215, 0, 255);
    colors["border_dragging"] = Color(100, 149, 237, 255);
    colors["click_indicator"] = Color(255, 0, 0, 255);
    colors["animation_border"] = Color(255, 255, 0, 255);
    colors["drag_border"] = Color(0, 100, 255, 255);
    
    // Default card type mappings
    cardTypeColors[CardType::VILLAGER] = "white";
    cardTypeColors[CardType::WOOD] = "green";
    cardTypeColors[CardType::ROCK] = "colorless";
    cardTypeColors[CardType::BERRY] = "green";
    cardTypeColors[CardType::BRANCH] = "green";
    cardTypeColors[CardType::LOG] = "green";
    cardTypeColors[CardType::PLANK] = "colorless";
    cardTypeColors[CardType::STICK] = "colorless";
}

Color ColorManager::getColor(const std::string& name) const {
    auto it = colors.find(name);
    if (it != colors.end()) {
        return it->second;
    }
    DEBUG_PRINT("Color not found: %s, using white\n", name.c_str());
    return Color(255, 255, 255, 255); // Default to white
}

Color ColorManager::getCardColor(CardType type) const {
    auto it = cardTypeColors.find(type);
    if (it != cardTypeColors.end()) {
        return getColor(it->second);
    }
    DEBUG_PRINT("Card type color not found, using colorless\n");
    return getColor("colorless");
}
