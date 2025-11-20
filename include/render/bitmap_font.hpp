#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <SDL.h>

namespace eclipse {

class BitmapFont {
public:
    static BitmapFont& instance();

    void drawText(SDL_Renderer* renderer, const std::string& text,
                  int x, int y, SDL_Color color, int scale = 2) const;

    int measureTextWidth(const std::string& text, int scale = 2) const;
    int lineHeight(int scale = 2) const;

private:
    BitmapFont();

    struct Glyph {
        int width;
        std::vector<uint8_t> rows;
    };

    const Glyph* glyphFor(char c) const;

    std::unordered_map<char, Glyph> glyphs_;
};

}  // namespace eclipse
