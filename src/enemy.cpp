#include "enemy.hpp"

Enemy::Enemy() : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), is_active(false), total_frames(0), flip(SDL_FLIP_NONE) {}

Enemy::Enemy(int spd, int life, int t_frms, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active)
    : Entity(spd, life, 0, 0, src, dst, tex), is_active(active), total_frames(t_frms), flip(SDL_FLIP_NONE) {
    UpdateHitbox();
}

void Enemy::UpdateHitbox() {
    hitbox.w = static_cast<int>(rect_dst.w * 0.4);
    hitbox.h = static_cast<int>(rect_dst.h * 0.85);

    hitbox.x = rect_dst.x + (rect_dst.w - hitbox.w) / 2 + 5;
    hitbox.y = rect_dst.y + (rect_dst.h - hitbox.h) / 2 + 10;
}

void Enemy::deactivate() {
    is_active = false;
}

