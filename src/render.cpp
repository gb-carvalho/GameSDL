#include "render.hpp"

SDL_Window* g_window = nullptr;
SDL_Renderer* g_renderer = nullptr;

void RenderProjectiles(SDL_Rect camera)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].is_active) {
            SDL_Rect render_rect = {
                projectiles[i].rect_dst.x - camera.x, // Ajusta a posi��o X com base na c�mera
                projectiles[i].rect_dst.y - camera.y, // Ajusta a posi��o Y com base na c�mera
                projectiles[i].rect_dst.w,
                projectiles[i].rect_dst.h
            };

            if (projectiles[i].type == FLAMEPILLAR) {
                const int spawn_duration = 15; // Dura��o do efeito de spawn, em frames.
                int frames_active = projectiles[i].frames_active; // Contador de frames do proj�til.

                if (frames_active < spawn_duration) {
                    float progress = (float)frames_active / spawn_duration;

                    // Faz o proj�til surgir de baixo ajustando sua altura e posi��o Y.
                    render_rect.h = (int)(projectiles[i].rect_dst.h * progress);
                    render_rect.y += projectiles[i].rect_dst.h - render_rect.h;
                }
            }

            SDL_RenderCopy(g_renderer, projectiles[i].texture, &projectiles[i].rect_src, &render_rect);

            projectiles[i].frames_active++;

        }
    }
}

void RenderEnemy(Enemy* enemy, SDL_Rect camera) {

    SDL_Rect enemy_render_rect = {
    enemy->rect_dst.x - camera.x,
    enemy->rect_dst.y - camera.y,
    enemy->rect_dst.w,
    enemy->rect_dst.h
    };

    if (enemy->is_dead) {
        const int death_duration = 15;
        int death_frames = enemy->death_frames;

        if (death_frames >= 0) {
            float progress = (float)death_frames / death_duration;

            enemy_render_rect.h = (int)(enemy->rect_dst.h * progress);
            enemy_render_rect.y += enemy->rect_dst.h - enemy_render_rect.h;

            enemy->death_frames--;
        }
        if (death_frames == 0){
            enemy->deactivate();
            enemy->is_dead = 0;
            return;
        }
    }
    enemy->UpdateHitbox();
    SDL_RenderCopyEx(g_renderer, enemy->texture, &enemy->rect_src, &enemy_render_rect, 0, NULL, enemy->flip);
}

void UpdateRenderStopwatchWave(int& start_time, int& time_left, int screen_width, int& elapsed_time, int& wave,
                               int& current_game_state, TTF_Font* font, int total_pause_duration, DynamicText* stopwatch_text)
{
    int wave_time_legth = std::min(25 + ((wave/2) * 5), 60);
    elapsed_time = (SDL_GetTicks() - start_time - total_pause_duration) / 1000;
    if (current_game_state == PLAYING) time_left = wave_time_legth - elapsed_time;
    else time_left = wave_time_legth;

    std::string time_formatted = TimeFormatted(time_left);
    stopwatch_text->Update(g_renderer, font, "Wave " + std::to_string(wave), { 255, 255, 255 }, { 0, 0, 0 });
    stopwatch_text->Render(g_renderer, screen_width / 2 - stopwatch_text->rect.w / 2, 20, true);
    stopwatch_text->Update(g_renderer, font, time_formatted, { 255, 255, 255 }, { 0, 0, 0 });
    stopwatch_text->Render(g_renderer, screen_width / 2 - stopwatch_text->rect.w / 2, 50, true);
}

void RenderExpBar(int screen_width, float exp)
{
    SDL_Rect exp_bar_rect = { 8, 8, screen_width - 20, 10 };
    SDL_SetRenderDrawColor(g_renderer, 230, 230, 230, 255);
    SDL_RenderDrawRect(g_renderer, &exp_bar_rect);

    int filled = static_cast<int>((exp * (screen_width - 22)) / MAX_EXP);

    SDL_Rect filled_rect = { 9, 9, filled, 8 };
    SDL_SetRenderDrawColor(g_renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(g_renderer, &filled_rect);
}

void RenderLifeBar(Character character, SDL_Rect camera)
{
    // altura e largura da barra
    const int barWidth = 51;
    const int barHeight = 6;
    const int offset = -20; 

    // centraliza a barra no personagem
    int barX = character.rect_dst.x + (character.rect_dst.w / 2) - (barWidth / 2);
    int barY = character.rect_dst.y + character.rect_dst.h + offset;

    // ret�ngulo da borda
    SDL_Rect barRect = {
        barX - camera.x,
        barY - camera.y,
        barWidth,
        barHeight
    };

    SDL_SetRenderDrawColor(g_renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(g_renderer, &barRect);

    // vida preenchida
    int filledWidth = (character.life * barWidth) / 3;

    SDL_Rect filledRect = {
        barX + 1 - camera.x,
        barY + 1 - camera.y,
        filledWidth - 2,
        barHeight - 2
    };

    SDL_SetRenderDrawColor(g_renderer, 139, 0, 0, 255);
    SDL_RenderFillRect(g_renderer, &filledRect);
}

int GetHoveredMenuIndex(const SDL_Point& mouse_pos, int start_x, int start_y, TTF_Font* font)
{
    int y = start_y;
    for (size_t index = 0; index < menuOptions.size(); ++index) {
        int text_width = 0;
        int text_height = 0;
        TTF_SizeText(font, menuOptions[index].name.c_str(), &text_width, &text_height);

        SDL_Rect item_rect = { start_x, y, text_width, text_height };
        if (mouse_pos.x >= item_rect.x && mouse_pos.x <= item_rect.x + item_rect.w &&
            mouse_pos.y >= item_rect.y && mouse_pos.y <= item_rect.y + item_rect.h) {
            return static_cast<int>(index);
        }

        y += TTF_FontLineSkip(font) + 12;
    }

    return -1;
}

int GetHoveredCardIndex(int mouse_x, int mouse_y, int screen_width, int screen_height)
{
    int spacing = CARD_WIDTH / 4;
    int total_width = CARDS_TO_CHOSE * (CARD_WIDTH / 2);
    int start_x = (screen_height / 2) - (total_width / 8);

    for (int i = 0; i < CARDS_TO_CHOSE; ++i) {
        SDL_Rect card_rect = {
            start_x + i * (CARD_WIDTH / 2 + spacing),
            (screen_height / 2) - (CARD_HEIGHT / 4),
            CARD_WIDTH / 2,
            CARD_HEIGHT / 2
        };

        if (mouse_x >= card_rect.x && mouse_x <= card_rect.x + card_rect.w &&
            mouse_y >= card_rect.y && mouse_y <= card_rect.y + card_rect.h) {
            return i;
        }
    }

    return -1;
}

void RenderMenu(SDL_Renderer* renderer, int x, int y, TTF_Font* font, int selected_index){
    int current_y = y;

    for (size_t i = 0; i < menuOptions.size(); ++i) {
        auto& item = menuOptions[i];
        SDL_Color color = (static_cast<int>(i) == selected_index) ? item.selected_color : item.text_color;

        DynamicText menu_text;
        menu_text.Update(renderer, font, item.name, color, { 0, 0, 0 });
        menu_text.Render(renderer, x, current_y, true);
        current_y += menu_text.rect.h + 16;
    }
    return;
}

void DrawThickRect(SDL_Renderer* renderer, SDL_Rect* rect, int thickness)
{
    for (int i = 0; i < thickness; i++) {
        SDL_Rect thick_rect = {
            rect->x + i,
            rect->y + i,
            rect->w - 2 * i,
            rect->h - 2 * i
        };
        SDL_RenderDrawRect(renderer, &thick_rect);
    }
}

void RenderCardInfo(const Card& card, TTF_Font* font, int screen_width, int screen_height)
{
    SDL_Color text_color = { 255, 255, 255 };
    SDL_Color shadow_color = { 0, 0, 0 };
    // Renderizar o nome do card
    DynamicText card_name_text;
    card_name_text.Update(g_renderer, font, card.name + "(" + std::to_string(card.level) + "/" + std::to_string(MAX_CARD_LEVEL) + ")", text_color, shadow_color);
    card_name_text.Render(g_renderer,
        card.rect_dst.x + (card.rect_dst.w / 2) - card_name_text.rect.w / 2,
        card.rect_dst.y + (card.rect_dst.h / 4), true);

    // Renderizar a descri��o do card
    DynamicText card_description_text;
    card_description_text.Update(g_renderer, font, card.description, text_color, shadow_color);
    card_description_text.Render(g_renderer,
        card.rect_dst.x + (card.rect_dst.w / 2) - card_description_text.rect.w / 2,
        card.rect_dst.y + (card.rect_dst.h / 2), true);
}

void RenderCardSelection(int card_selected, TTF_Font* small_font, int screen_width, int screen_height, int level_to_update, int hovered_card_index)
{
    SDL_Color text_color = { 255, 255, 255 };
    SDL_Color shadow_color = { 0, 0, 0 };
    DynamicText point_to_add_text;
    point_to_add_text.Update(g_renderer, small_font, "Points: " + std::to_string(level_to_update), text_color, shadow_color);
    point_to_add_text.Render(g_renderer,
        screen_width / 2 - point_to_add_text.rect.w / 2,
        screen_height / 2 - (CARD_WIDTH / 2), true);

    for (int i = 0; i < CARDS_TO_CHOSE; i++) {
        int card_number = random_card_array[i];
        int spacing = CARD_WIDTH / 4;
        int total_width = CARDS_TO_CHOSE * (CARD_WIDTH / 2);
        int start_x = (screen_height / 2) - (total_width / 8);

        cards[card_number].rect_src = { 0, 0, CARD_WIDTH, CARD_HEIGHT };
        cards[card_number].rect_dst = { start_x + i * (CARD_WIDTH / 2 + spacing),
                                (screen_height / 2) - (CARD_HEIGHT / 4),
                                CARD_WIDTH / 2,
                                CARD_HEIGHT / 2 };
        cards[card_number].texture = nullptr;

        if (i == hovered_card_index) {
            SDL_SetRenderDrawColor(g_renderer, 55, 70, 95, 180);
        }
        else if (i == card_selected) {
            SDL_SetRenderDrawColor(g_renderer, 85, 20, 20, 180);
        }
        else {
            SDL_SetRenderDrawColor(g_renderer, 20, 25, 35, 160);
        }
        SDL_RenderFillRect(g_renderer, &cards[card_number].rect_dst);
        RenderCardInfo(cards[card_number], small_font, screen_width, screen_height);

        if (i == hovered_card_index || i == card_selected) {
            SDL_SetRenderDrawColor(g_renderer, 120, 120, 120, 180);
            DrawThickRect(g_renderer, &cards[card_number].rect_dst, 4);
        }
    }
}

void RenderHeader(int& start_time, int& time_left, int screen_width, int& elapsed_time, int& wave, int& current_game_state, 
                  TTF_Font* small_font, Character character, int total_pause_duration, 
                  DynamicText* stopwatch_text, DynamicText* life_text, DynamicText* kill_count_text, DynamicText* level_text)
{
    UpdateRenderStopwatchWave(start_time, time_left, screen_width, elapsed_time, wave, current_game_state, small_font, total_pause_duration, stopwatch_text);
    life_text->Render(g_renderer, 50, 20, true);
    kill_count_text->Render(g_renderer, static_cast<int>(screen_width / 1.3 - (kill_count_text->rect.w)), 20, true);
    level_text->Render(g_renderer, screen_width - 30 - (level_text->rect.w), 20, true);
    RenderExpBar(screen_width, character.exp);
}

SDL_Texture* CreateTextureImg(const char* image_path)
{
    SDL_Surface* image_surface = IMG_Load(image_path);
    if (!image_surface) {
        SDL_Log("Error loading image: %s", IMG_GetError());
        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return NULL;
    }

    // Cria a textura a partir da superf�cie da imagem
    SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, image_surface);
    SDL_FreeSurface(image_surface);  // Liberar a superf�cie da mem�ria

    if (!texture) {
        SDL_Log("Error creating texture: %s", SDL_GetError());
        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return NULL;
    }

    return texture;
}

void UpdateCamera(int playerX, int playerY, SDL_Rect* camera, SDL_Rect character_rect_dst, int bg_width, int bg_height, int screen_width, int screen_height)
{
    // Centralizar a c�mera no jogador
    camera->x = playerX + character_rect_dst.w / 2 - screen_width / 2;
    camera->y = playerY + character_rect_dst.h / 2 - screen_height / 2;

    // Limitar a c�mera para n�o sair dos limites do mundo
    if (camera->x < 0) camera->x = 0;
    if (camera->y < 0) camera->y = 0;
    if (camera->x > bg_width - camera->w) camera->x = bg_width - camera->w;
    if (camera->y > bg_height - camera->h) camera->y = bg_height - camera->h;
}
