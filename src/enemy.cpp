#include "enemy.hpp"

Enemy::Enemy() : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), is_active(false) {}

Enemy::Enemy(int spd, int lfe, int frm, int lftime, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active)
    : Entity(spd, lfe, frm, lftime, src, dst, tex), is_active(active) {
    UpdateHitbox();
}

void Enemy::UpdateHitbox() {
    hitbox.w = rect_dst.w * 0.4;
    hitbox.h = rect_dst.h * 0.85;

    hitbox.x = rect_dst.x + (rect_dst.w - hitbox.w) / 2 + 5;
    hitbox.y = rect_dst.y + (rect_dst.h - hitbox.h) / 2 + 10;
}

void Enemy::deactivate() {
    is_active = false;
}

