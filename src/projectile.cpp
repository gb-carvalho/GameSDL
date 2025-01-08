#include "projectile.hpp"


Projectile projectiles[MAX_PROJECTILES];

Projectile::Projectile()
    : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), is_active(false), dir_x(0.0f), dir_y(0.0f), 
             type(MAGICBALL), total_frames(0), animation_speed(0), sprite_sheet_width(0), frames_active(0) {}

void Projectile::GetSpriteSheetWidth() {
    int texture_width, texture_height;
    SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);

    sprite_sheet_width = texture_width; // Guarda a largura do sprite sheet
}

void Projectile::UpdateHitbox() {

    switch (type) {
        case VORTEX:{
            int half_of_animation = total_frames / 2 - 6;

            if (frame < half_of_animation) {
                // Crescimento na primeira metade da animação
                hitbox.w = static_cast<int>(rect_dst.w * ((float)frame / half_of_animation) * 1.10);
                hitbox.h = static_cast<int>(rect_dst.h * ((float)frame / half_of_animation) * 1.10);
            }
            else if (frame < 2 * half_of_animation) {
                // Diminuição na segunda metade da animação
                int adjusted_frame = frame - half_of_animation; // Frame relativo à segunda metade
                hitbox.w = static_cast<int>(rect_dst.w * (1.0f - ((float)adjusted_frame / half_of_animation)));
                hitbox.h = static_cast<int>(rect_dst.h * (1.0f - ((float)adjusted_frame / half_of_animation)));
            }


            hitbox.x = rect_dst.x + rect_dst.w / 2 - hitbox.w / 2;
            hitbox.y = rect_dst.y + rect_dst.h / 2 - hitbox.h / 2;
            break;
        }
        case FLAMEPILLAR:
            hitbox.w = rect_dst.w / 4;
            hitbox.h = rect_dst.h / 2;

            hitbox.x = rect_dst.x + rect_dst.w / 2 - hitbox.w / 2;
            hitbox.y = rect_dst.y + rect_dst.h / 2 - hitbox.h / 2;
            break;
        default:
            hitbox.w = rect_dst.w;
            hitbox.h = rect_dst.h;

            hitbox.x = rect_dst.x;
            hitbox.y = rect_dst.y;
            break;
    }
}

void Projectile::deactivate() {
    is_active = false;
}