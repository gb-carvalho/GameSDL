#pragma once
#include "entity.hpp"

#define ENEMY_DELAY      900
#define MAX_ENEMIES      20
#define ENEMY_MAGE_WIDTH_ORIG  85
#define ENEMY_MAGE_HEIGHT_ORIG 94

class Enemy : public Entity {
public:
    bool is_active;

    Enemy();
    Enemy(int spd, int lfe, int frm, int lftime, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active);

    void UpdateHitbox() override;
    void deactivate();
};
