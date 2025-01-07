#pragma once
#include "entity.hpp"

#define MAX_PROJECTILES  100
#define PROJECTILE_WIDTH_ORIG  13
#define PROJECTILE_HEIGTH_ORIG 13
#define PROJECTILE_FLAMEBALL_WIDTH_ORIG  32
#define PROJECTILE_FLAMEBALL_HEIGTH_ORIG 32
#define PROJECTILE_VORTEX_WIDTH_ORIG  429
#define PROJECTILE_VORTEX_HEIGTH_ORIG 429


enum projectileType { MAGICBALL, FLAMEBALL, VORTEX };

class Projectile : public Entity {
public:
    bool is_active;
    int total_frames, sprite_sheet_width, animation_speed;
    float dir_x, dir_y;
    enum projectileType type;

    Projectile();

    void GetSpriteSheetWidth();
    void UpdateHitbox() override;
    void deactivate();
};

extern Projectile projectiles[MAX_PROJECTILES];