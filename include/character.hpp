#pragma once
#include "entity.hpp"

#define DAMAGE_COOLDOWN         500
#define MAX_EXP                 20
#define WALK_FRAME_COUNT        4
#define IDLE_FRAME_COUNT        2
#define CHARACTER_WIDTH_ORIG    32 //Size of character width in sprite
#define CHARACTER_HEIGHT_ORIG   32 //Size of character height in sprite
#define CHARACTER_WIDTH_RENDER  64 //Size of character width in render
#define CHARACTER_HEIGHT_RENDER 64 //Size of character height in render
#define CHARACTER_PROJECTILE_DELAY 1000


enum characterState { IDLE, WALKING };

class Character : public Entity {
public:
    characterState current_state;
    int exp, level, level_to_update, projectile_delay, last_damage_time, damage;
    float pos_y, pos_x;
    bool took_damage;

    Character(int spd, int lfe, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, characterState state, int prjctle_delay);

    void UpdateHitbox() override;
    void reset(SDL_Rect rect_dst_new);
    void updateState(characterState newState);
};