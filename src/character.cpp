#include "character.hpp"

Character::Character(int spd, float lfe, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, characterState state, int prjctle_delay)
    : Entity(spd, lfe, 0, 0, src, dst, tex), current_state(state), exp(0), level(0), projectile_delay(prjctle_delay), 
    last_damage_time(0), took_damage(false), level_to_update(0), damage(1), pos_x(static_cast<float>(dst.x)), pos_y(static_cast<float>(dst.y)),
    projectile_speed_multiplier(1), exp_multiplier(1){
    UpdateHitbox();
}

void Character::UpdateHitbox(){
    hitbox.w = static_cast<int>(rect_dst.w * 0.5);
    hitbox.h = static_cast<int>(rect_dst.h * 0.9);

    hitbox.x = rect_dst.x + (rect_dst.w - hitbox.w) / 2;
    hitbox.y = rect_dst.y + (rect_dst.h - hitbox.h) / 2;
}

void Character::reset(SDL_Rect rect_dst_new) {
    life     = 3;
    damage   = 1;
    frame    = 0;   
    exp      = 0;
    level    = 1;
    speed    = 7;
    rect_dst = rect_dst_new;
    pos_x    = static_cast<float>(rect_dst_new.x);
    pos_y    = static_cast<float>(rect_dst_new.y);
    flameball        = 0;
    vortex           = 0;
    last_frame_time  = 0;
    level_to_update  = 0;
    projectile_delay = CHARACTER_PROJECTILE_DELAY;
    exp_multiplier   = 1;
    projectile_speed_multiplier = 1;
}

void Character::updateState(characterState newState) {
    current_state = newState;
}