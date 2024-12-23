#include "projectile.hpp"


Projectile projectiles[MAX_PROJECTILES];

Projectile::Projectile()
    : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), is_active(false), dir_x(0.0f), dir_y(0.0f) {}

Projectile::Projectile(int spd, int lfe, int frm, int lftime, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active, float dx, float dy)
    : Entity(spd, lfe, frm, lftime, src, dst, tex), is_active(active), dir_x(dx), dir_y(dy) {
}

void Projectile::UpdateHitbox() {
    hitbox.w = rect_dst.w;
    hitbox.h = rect_dst.h;

    hitbox.x = rect_dst.x;
    hitbox.y = rect_dst.y;
}

void Projectile::deactivate() {
    is_active = false;
}