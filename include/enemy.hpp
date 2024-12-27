#pragma once
#include "entity.hpp"

#define ENEMY_DELAY      900
#define MAX_ENEMIES      20

#define ENEMY_MAGE_WIDTH  85
#define ENEMY_MAGE_HEIGHT 94
#define ENEMY_MAGE_FRAMES 8

#define ENEMY_BAT_WIDTH  64
#define ENEMY_BAT_HEIGHT 64
#define ENEMY_BAT_FRAMES 4

class Enemy : public Entity {
public:
    bool is_active;
    int total_frames;

    Enemy();
    Enemy(int spd, int lftime, int t_frms, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active);

    void UpdateHitbox() override;
    void deactivate();
};
