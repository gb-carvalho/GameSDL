#include "game_utils.hpp"

void LevelUp(SDL_Renderer* g_renderer, Character& character, int& current_game_state, TTF_Font* font, DynamicText *level_text) {
    character.exp = 0;
    character.level_to_update++;
    character.level++;
    level_text->Update(g_renderer, font, "Level: " + std::to_string(character.level), { 255, 255, 255 }, { 0, 0, 0 });
}