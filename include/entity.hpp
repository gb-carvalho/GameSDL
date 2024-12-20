#pragma once
#include <SDL.h>

class Entity {
public:
    int speed;
    int life;
    int frame;
    Uint32 last_frame_time;
    SDL_Rect rect_src, rect_dst, hitbox;
    SDL_Texture* texture;

    Entity();
    Entity(int spd, int lfe, int frm, int lftime, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex);
    virtual void UpdateHitbox();
};
