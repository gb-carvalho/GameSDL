#pragma once
#include "entity.hpp"

#define MAX_PROJECTILES  100
#define PROJECTILE_WIDTH_ORIG  13
#define PROJECTILE_HEIGTH_ORIG 13
#define PROJECTILE_FLAMEBALL_WIDTH_ORIG  32
#define PROJECTILE_FLAMEBALL_HEIGTH_ORIG 32

enum projectileType { MAGICBALL, FLAMEBALL };

class Projectile : public Entity {
public:
    bool is_active;
    int total_frames;
    float dir_x, dir_y;
    enum projectileType type;

    Projectile();
    Projectile(int spd, int lfe, int frm, int lftime, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active, float dx, float dy);

    void UpdateHitbox() override;
    void deactivate();
};

extern Projectile projectiles[MAX_PROJECTILES];