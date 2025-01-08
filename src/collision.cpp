#include "collision.hpp"

bool resolved_collision[MAX_ENEMIES][MAX_ENEMIES] = { false };

bool CheckCollision(SDL_Rect a, SDL_Rect b, SDL_Rect camera)
{
    bool collision =
        a.x + a.w >= b.x &&
        b.x + b.w >= a.x &&
        a.y + a.h >= b.y &&
        b.y + b.h >= a.y;

    SDL_Rect adjusted_a = {
        a.x - camera.x, a.y - camera.y, a.w, a.h
    };
    SDL_Rect adjusted_b = {
        b.x - camera.x, b.y - camera.y, b.w, b.h
    };

    //DEBUG//
    SDL_SetRenderDrawColor(g_renderer, 0, 255, 0, 255);
    SDL_RenderDrawRect(g_renderer, &adjusted_a);
    //SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 255);
    //SDL_RenderDrawRect(g_renderer, &adjusted_b);

    return collision;
}

void CheckProjectileCollisionWithEnemy(SDL_Renderer* g_renderer, Character& character, SDL_Rect enemy_rect, float& enemy_life, bool& active, SDL_Rect camera, int& kill_count,
    TTF_Font* font, int& current_game_state, DynamicText *kill_count_text, DynamicText *level_text, Mix_Chunk* enemy_damage_sound)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].is_active && CheckCollision(projectiles[i].hitbox, enemy_rect, camera)) {
            if (projectiles[i].type == VORTEX || projectiles[i].type == FLAMEPILLAR) {
                enemy_life -= character.damage / 4.0f;
            }
            else {
                enemy_life -= character.damage;
                projectiles[i].deactivate();
            }
            Mix_PlayChannel(-1, enemy_damage_sound, 0);
            if (enemy_life <= 0) {
                kill_count++;
                kill_count_text->Update(g_renderer, font, "Enemies killed: " + std::to_string(kill_count), { 255, 255, 255 }, { 0, 0, 0 });
                character.exp += 1 * character.exp_multiplier;
                if (character.exp >= MAX_EXP) {
                    LevelUp(g_renderer, character, current_game_state, font, level_text);
                }
                active = 0;
            }
        }
    }
}

void resolveCollision(SDL_Rect* a, SDL_Rect* b)
{
    float dx = static_cast<float>((a->x + a->w / 2) - (b->x + b->w / 2));
    float dy = static_cast<float>((a->y + a->h / 2) - (b->y + b->h / 2));

    float half_sum_x = static_cast<float>((a->w / 2) + (b->w / 2));
    float half_sum_y = static_cast<float>((a->h / 2) + (b->h / 2));

    float overlap_x = half_sum_x - fabs(dx);
    float overlap_y = half_sum_y - fabs(dy);

    if (overlap_x > 0 && overlap_y > 0) {

        if (overlap_x < overlap_y) {
            // Resolve along the X-axis
            if (dx > 0) {
                b->x -= static_cast<int>(overlap_x / 5);
            }
            else {
                b->x += static_cast<int>(overlap_x / 5);
            }
        }
        else {
            // Resolve along the Y-axis
            if (dy > 0) {
                b->y -= static_cast<int>(overlap_y / 5);
            }
            else {
                b->y += static_cast<int>(overlap_y / 5);
            }
        }
    }
};