#include "character.hpp"

Character::Character(int spd, int lfe, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, characterState state, int prjctle_delay)
    : Entity(spd, lfe, 0, 0, src, dst, tex), current_state(state), exp(0), level(0),
    projectile_delay(prjctle_delay), last_damage_time(0), took_damage(false), level_to_update(0), damage(1) {
    UpdateHitbox();
}

void Character::UpdateHitbox(){
    hitbox.w = rect_dst.w * 0.5;
    hitbox.h = rect_dst.h * 0.9;

    hitbox.x = rect_dst.x + (rect_dst.w - hitbox.w) / 2;
    hitbox.y = rect_dst.y + (rect_dst.h - hitbox.h) / 2;
}

void Character::reset(SDL_Rect rect_dst_new) {
    life = 3;
    damage = 1;
    frame = 0;
    exp = 0;
    level = 1;
    speed = 7;
    rect_dst = rect_dst_new;
    last_frame_time = 0;
    level_to_update = 0;
    projectile_delay = CHARACTER_PROJECTILE_DELAY;
}

void Character::updateState(characterState newState) {
    current_state = newState;
}