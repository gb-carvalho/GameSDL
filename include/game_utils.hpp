#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <fstream>
#include <vector>
#include <SDL_mixer.h>

#include "enemy.hpp"
#include "projectile.hpp"
#include "dynamic_text.hpp"
#include "character.hpp"
#include "card.hpp"
#include "render.hpp"
#include "collision.hpp"

#define ANIMATION_SPEED        160 //Isso deveria ter valor diferente por entidade talvez
#define FIRST_WAVE_TIME        30
#define SAVE_FILE              "sdl.dat"

extern Uint32 last_projectiles_times[];
extern Uint32 last_enemy_time;

extern std::vector<int> random_card_array;
extern Enemy enemies[MAX_ENEMIES];
enum gameState { TITLE_SCREEN, PLAYING, CARD_SELECTOR, GAME_OVER, PAUSE };

void LevelUp(SDL_Renderer* g_renderer, Character& character, int& current_game_state, TTF_Font* font, DynamicText* level_text);
void UpdateEnemyPosition(Enemy* enemy, SDL_Rect player_rect);
void UpdateAnimationCharacter(Character* character, int width, int height, int walk_frame_count, int idle_frame_count);
void UpdateEnemyAnimation(Enemy* enemy);
void UpdateProjectileAnimation(Projectile* projectile);
float CalculateMagnitude(SDL_Rect a, SDL_Rect b);
void CalculateDirection(SDL_Rect a, SDL_Rect b, Projectile* projectile);
void FireSingleProjectile(SDL_Rect player_rect, SDL_Texture* texture, int speed, int total_frames,
    SDL_Rect rect_src, SDL_Rect rect_dst,
    int delay, Uint32& last_projectile_time, Mix_Chunk* sound, projectileType type, int animation_speed);
Enemy* FindClosestEnemy(SDL_Rect player_rect, Enemy enemies[], int max_enemies);
void FireProjectiles(Character character, SDL_Rect camera, SDL_Texture* projectile_textures[], Mix_Chunk* projectile_sound);
void UpdateProjectiles(int width_limit, int height_limit, float multiplier, Character character);
void UpdateFlameballProjectilePosition(Projectile* projectile, SDL_Rect enemy_rect);
void SpawnEnemies(EnemyType enemy_type, SDL_Rect camera, int bg_width, int bg_height, SDL_Texture* enemy_texture, int wave, int width, int height, int frames, float size_percent);
std::string TimeFormatted(int time_in_seconds);
void ResetGame(int& kill_count, int& wave, Character* character, int bg_width, int bg_height, int& start_time, 
               int& elapsed_time, TTF_Font* font, int& total_pause_duration,
               DynamicText* stopwatch_text, DynamicText* life_text, DynamicText* kill_count_text, DynamicText* level_text);
void SelectCard(std::string card_name, Character& character, TTF_Font* font, DynamicText* life_text);
void randomizeCardArray();
void NewWave(int& current_game_state, int& wave, bool& skip);
void DamageColor(SDL_Texture* texture, Uint32 last_damage_time, bool& took_damage);
void MoveCharacter(Character* character, const Uint8* keyState, int bg_width, int bg_height);
void LoadGame(const std::string& file_name, int& kill_count, int& wave);
void SaveGame(const std::string& file_name, int kill_count, int wave);