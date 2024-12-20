#include "dynamic_text.hpp"

DynamicText::DynamicText() : texture(nullptr), shadow_texture(nullptr), rect{ 0,0,0,0 }, current_text("") {}

void DynamicText::Update(SDL_Renderer* renderer, TTF_Font* font, const std::string& new_text, SDL_Color color, SDL_Color shadow_color) {
    if (new_text != current_text) {
        current_text = new_text;

        if (texture) {
            SDL_DestroyTexture(texture); // Limpa a textura antiga
        }
        if (shadow_texture) {
            SDL_DestroyTexture(shadow_texture);
        }

        SDL_Surface* surface = TTF_RenderText_Solid(font, new_text.c_str(), color);
        texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Surface* shadow_surface = TTF_RenderText_Solid(font, new_text.c_str(), shadow_color);
        shadow_texture = SDL_CreateTextureFromSurface(renderer, shadow_surface);

        rect.w = surface->w;
        rect.h = surface->h;

        SDL_FreeSurface(surface);
        SDL_FreeSurface(shadow_surface);

    }
}

void DynamicText::Render(SDL_Renderer* renderer, int x, int y, bool shadow) {

    if (shadow && shadow_texture) {
        SDL_Rect shadow_rect = { x + 2, y + 2, rect.w, rect.h };
        SDL_RenderCopy(renderer, shadow_texture, nullptr, &shadow_rect);
    }

    rect.x = x;
    rect.y = y;
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

DynamicText::~DynamicText() {
    if (texture) SDL_DestroyTexture(texture);
}