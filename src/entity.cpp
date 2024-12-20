#include "entity.hpp"

Entity::Entity()
    : speed(0), life(0), frame(0), last_frame_time(0),
    rect_src{ 0, 0, 0, 0 }, rect_dst{ 0, 0, 0, 0 }, hitbox{ 0, 0, 0, 0 },
    texture(nullptr) {}

Entity::Entity(int spd, int lfe, int frm, int lftime, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex)
    : speed(spd), life(lfe), frame(frm), last_frame_time(lftime), rect_src(src), rect_dst(dst), texture(tex) {
    UpdateHitbox();
}

void Entity::UpdateHitbox() {
    hitbox = rect_dst;
}