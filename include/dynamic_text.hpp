#pragma once
#include "entity.hpp"
#include <SDL_ttf.h>
#include <string>

struct DynamicText {
    SDL_Texture* texture;
    SDL_Texture* shadow_texture;
    SDL_Rect rect;
    std::string current_text;

    DynamicText();

    void Update(SDL_Renderer* renderer, TTF_Font* font, const std::string& new_text, SDL_Color color, SDL_Color shadow_color);
    void Render(SDL_Renderer* renderer, int x, int y, bool shadow);
    ~DynamicText();
};