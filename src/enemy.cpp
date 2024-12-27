#include "enemy.hpp"

Enemy::Enemy() : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), is_active(false), total_frames(0) {}

Enemy::Enemy(int spd, int lftime, int t_frms, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active)
    : Entity(spd, 0, 0, lftime, src, dst, tex), is_active(active), total_frames(t_frms) {
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

