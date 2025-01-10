#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "collision.hpp"
#include "projectile.hpp"
#include "dynamic_text.hpp"
#include "game_utils.hpp"
#include "dynamic_text.hpp"
#include "character.hpp"

extern bool resolved_collision[MAX_ENEMIES][MAX_ENEMIES];

bool CheckCollision(SDL_Rect a, SDL_Rect b, SDL_Rect camera);
void CheckProjectileCollisionWithEnemy(SDL_Renderer* g_renderer, Character& character, Enemy &enemy, SDL_Rect camera, int& kill_count,
                                       TTF_Font* font, int& current_game_state, DynamicText *kill_count_text, DynamicText *level_text, Mix_Chunk* enemy_damage_sound);
void resolveCollision(SDL_Rect* a, SDL_Rect* b);