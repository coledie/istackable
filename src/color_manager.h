#pragma once

#include "common.h"
#include <map>
#include <string>

struct Color {
    Uint8 r, g, b, a;
    
    Color() : r(0), g(0), b(0), a(255) {}
    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) : r(r), g(g), b(b), a(a) {}
    
    SDL_Color toSDL() const {
        return {r, g, b, a};
    }
};

class ColorManager {
private:
    std::map<std::string, Color> colors;
    std::map<CardType, std::string> cardTypeColors;
    
    Color parseColor(const std::string& colorString);
    void loadDefaultColors();
    
public:
    ColorManager();
    bool loadFromFile(const std::string& filename);
    
    Color getColor(const std::string& name) const;
    Color getCardColor(CardType type) const;
    
    // Quick access to common colors
    Color getBackgroundColor() const { return getColor("playmat"); }
    Color getBorderLight() const { return getColor("border_light"); }
    Color getBorderDark() const { return getColor("border_dark"); }
    Color getSelectedBorder() const { return getColor("border_selected"); }
    Color getDragBorder() const { return getColor("drag_border"); }
    Color getAnimationBorder() const { return getColor("animation_border"); }
    Color getClickIndicator() const { return getColor("click_indicator"); }
};
