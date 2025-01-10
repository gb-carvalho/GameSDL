#include "enemy.hpp"

Enemy::Enemy() : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), is_active(false), total_frames(0), flip(SDL_FLIP_NONE), type(MAGE), last_damage_time(0) {}

Enemy::Enemy(float spd, float life, int t_frms, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active, EnemyType enemy_type)
    : Entity(spd, life+10000, 0, 0, src, dst, tex), is_active(active), total_frames(t_frms), flip(SDL_FLIP_NONE), type(enemy_type), last_damage_time(0) {
    UpdateHitbox();
}

void Enemy::UpdateHitbox()  {

    switch (type) {
    case MAGE:
        hitbox.w = static_cast<int>(rect_dst.w * 0.4f);
        hitbox.h = static_cast<int>(rect_dst.h * 0.8f);
        break;
    case MAGE2:
        hitbox.w = static_cast<int>(rect_dst.w * 0.7f);
        hitbox.h = static_cast<int>(rect_dst.h * 0.65f);

        hitbox.x = rect_dst.x + ((rect_dst.w - hitbox.w) / 2) + (flip ? -5 : 5);
        hitbox.y = rect_dst.y + ((rect_dst.h - hitbox.h) / 2);

        return;
    case MAGE3:
        hitbox.w = static_cast<int>(rect_dst.w * 0.8f);
        hitbox.h = static_cast<int>(rect_dst.h * 0.75f);
        break;
    case WOLF:
        hitbox.w = static_cast<int>(rect_dst.w * 0.4f);
        hitbox.h = static_cast<int>(rect_dst.h * 0.8f);
        break;
    case BAT:
        hitbox.w = static_cast<int>(rect_dst.w * 0.45);
        hitbox.h = static_cast<int>(rect_dst.h * 0.45);
        break;
    case GOLEM:
        hitbox.w = static_cast<int>(rect_dst.w * 0.4f);
        hitbox.h = static_cast<int>(rect_dst.h * 0.7f);

        hitbox.x = rect_dst.x + ((rect_dst.w - hitbox.w) / 2) + (flip ? 10 : -10);
        hitbox.y = rect_dst.y + ((rect_dst.h - hitbox.h) / 2) + 10;

        return;
    case ANDROMALIUS:
        hitbox.w = static_cast<int>(rect_dst.w * 0.8f);
        hitbox.h = static_cast<int>(rect_dst.h * 0.85f);
        break;
    }

    hitbox.x = rect_dst.x + (rect_dst.w - hitbox.w) / 2;
    hitbox.y = rect_dst.y + (rect_dst.h - hitbox.h) / 2;
}


void Enemy::deactivate() {
    is_active = false;
}

