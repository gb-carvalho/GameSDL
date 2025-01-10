#pragma once
#include "entity.hpp"

#define ENEMY_DELAY      850
#define MAX_ENEMIES      75

#define ENEMY_MAGE_WIDTH  85
#define ENEMY_MAGE_HEIGHT 94
#define ENEMY_MAGE_FRAMES 8

#define ENEMY_BAT_WIDTH  64
#define ENEMY_BAT_HEIGHT 64
#define ENEMY_BAT_FRAMES 4

#define ENEMY_MAGE2_WIDTH  122
#define ENEMY_MAGE2_HEIGHT 110
#define ENEMY_MAGE2_FRAMES 8

#define ENEMY_MAGE3_WIDTH  87
#define ENEMY_MAGE3_HEIGHT 110
#define ENEMY_MAGE3_FRAMES 8

#define ENEMY_WOLF_WIDTH  52
#define ENEMY_WOLF_HEIGHT 45
#define ENEMY_WOLF_FRAMES 4

#define ENEMY_GOLEM_WIDTH  64
#define ENEMY_GOLEM_HEIGHT 72
#define ENEMY_GOLEM_FRAMES 6

#define ENEMY_ANDROMALIUS_WIDTH  57
#define ENEMY_ANDROMALIUS_HEIGHT 88
#define ENEMY_ANDROMALIUS_FRAMES 8

enum EnemyType { MAGE, MAGE2, MAGE3, WOLF, BAT, GOLEM, ANDROMALIUS };

class Enemy : public Entity {
public:
    bool is_active;
    int total_frames;
    SDL_RendererFlip flip;
    EnemyType type;
    Uint32 last_damage_time;

    Enemy();
    Enemy(float spd, float life, int t_frms, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active, EnemyType enemy_type);

    void UpdateHitbox() override;
    void deactivate();
};
