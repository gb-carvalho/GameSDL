#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <algorithm>

#include "character.hpp"
#include "projectile.hpp"
#include "string"
#include "card.hpp"
#include "dynamic_text.hpp"
#include "render.hpp"
#include "game_utils.hpp"

extern SDL_Window* g_window ;
extern SDL_Renderer* g_renderer;

void RenderProjectiles(SDL_Rect camera);
void UpdateRenderStopwatchWave(int& start_time, int& time_left, int screen_width, int& elapsed_time, int& wave, int& current_game_state, 
                                TTF_Font* font, int total_pause_duration, DynamicText *stopwatch_text);
void RenderExpBar(int screen_width, float exp);
void DrawThickRect(SDL_Renderer* renderer, SDL_Rect* rect, int thickness);
void RenderCardSelection(int card_selected, TTF_Font* small_font, int screen_width, int screen_height, int level_to_update);
void RenderHeader(int& start_time, int& time_left, int screen_width, int& elapsed_time, int& wave, int& current_game_state, 
                  TTF_Font* small_font, Character character, int total_pause_duration,
                  DynamicText* stopwatch_text, DynamicText* life_text, DynamicText* kill_count_text, DynamicText* level_text);
SDL_Texture* CreateTextureImg(const char* image_path);
void UpdateCamera(int playerX, int playerY, SDL_Rect* camera, SDL_Rect character_rect_dst, int bg_width, int bg_height, int screen_width, int screen_height);