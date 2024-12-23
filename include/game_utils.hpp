#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <dynamic_text.hpp>

#include "character.hpp"

void LevelUp(SDL_Renderer* g_renderer, Character& character, int& current_game_state, TTF_Font* font, DynamicText* level_text);