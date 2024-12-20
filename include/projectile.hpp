#pragma once
#include "entity.hpp"

#define PROJECTILE_SPEED 15
#define MAX_PROJECTILES  100
#define PROJECTILE_WIDTH_ORIG  13
#define PROJECTILE_HEIGTH_ORIG 13

class Projectile : public Entity {
public:
    bool is_active;
    float dir_x, dir_y;

    Projectile();
    Projectile(int spd, int lfe, int frm, int lftime, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active, float dx, float dy);

    void UpdateHitbox() override;
    void deactivate();
};