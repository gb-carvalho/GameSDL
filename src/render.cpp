#include "render.hpp"

SDL_Window* g_window = nullptr;
SDL_Renderer* g_renderer = nullptr;

void RenderProjectiles(SDL_Rect camera)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].is_active) {
            SDL_Rect render_rect = {
                projectiles[i].rect_dst.x - camera.x, // Ajusta a posição X com base na câmera
                projectiles[i].rect_dst.y - camera.y, // Ajusta a posição Y com base na câmera
                projectiles[i].rect_dst.w,
                projectiles[i].rect_dst.h
            };
            SDL_RenderCopy(g_renderer, projectiles[i].texture, &projectiles[i].rect_src, &render_rect);
        }
    }
}

void UpdateRenderStopwatchWave(int& start_time, int& time_left, int screen_width, int& elapsed_time, int& wave,
                               int& current_game_state, TTF_Font* font, int total_pause_duration, DynamicText* stopwatch_text)
{
    int wave_time_legth = FIRST_WAVE_TIME + ((wave - 1) * 5);
    elapsed_time = (SDL_GetTicks() - start_time - total_pause_duration) / 1000;
    if (current_game_state == PLAYING) time_left = wave_time_legth - elapsed_time;
    else time_left = wave_time_legth;

    std::string time_formatted = TimeFormatted(time_left);
    stopwatch_text->Update(g_renderer, font, "Wave " + std::to_string(wave), { 255, 255, 255 }, { 0, 0, 0 });
    stopwatch_text->Render(g_renderer, screen_width / 2 - stopwatch_text->rect.w / 2, 20, true);
    stopwatch_text->Update(g_renderer, font, time_formatted, { 255, 255, 255 }, { 0, 0, 0 });
    stopwatch_text->Render(g_renderer, screen_width / 2 - stopwatch_text->rect.w / 2, 50, true);
}

void RenderExpBar(int screen_width, int exp)
{
    SDL_Rect exp_bar_rect = { 8, 8, screen_width - 20, 10 };
    SDL_SetRenderDrawColor(g_renderer, 230, 230, 230, 255);
    SDL_RenderDrawRect(g_renderer, &exp_bar_rect);

    int filled = (exp * (screen_width - 22)) / MAX_EXP;

    SDL_Rect filled_rect = { 9, 9, filled, 8 };
    SDL_SetRenderDrawColor(g_renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(g_renderer, &filled_rect);
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

    // Renderizar a descrição do card
    DynamicText card_description_text;
    card_description_text.Update(g_renderer, font, card.description, text_color, shadow_color);
    card_description_text.Render(g_renderer,
        card.rect_dst.x + (card.rect_dst.w / 2) - card_description_text.rect.w / 2,
        card.rect_dst.y + (card.rect_dst.h / 2), true);
}

void RenderCardSelection(int card_selected, TTF_Font* small_font, int screen_width, int screen_height, int level_to_update)
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

        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 100);
        SDL_RenderFillRect(g_renderer, &cards[card_number].rect_dst);
        RenderCardInfo(cards[card_number], small_font, screen_width, screen_height);
        if (i == card_selected) {
            SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 100);
            DrawThickRect(g_renderer, &cards[card_number].rect_dst, 5);
        }
    }
}

void RenderHeader(int& start_time, int& time_left, int screen_width, int& elapsed_time, int& wave, int& current_game_state, 
                  TTF_Font* small_font, Character character, int total_pause_duration, 
                  DynamicText* stopwatch_text, DynamicText* life_text, DynamicText* kill_count_text, DynamicText* level_text)
{
    UpdateRenderStopwatchWave(start_time, time_left, screen_width, elapsed_time, wave, current_game_state, small_font, total_pause_duration, stopwatch_text);
    life_text->Render(g_renderer, 50, 20, true);
    kill_count_text->Render(g_renderer, screen_width / 1.3 - (kill_count_text->rect.w), 20, true);
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

    // Cria a textura a partir da superfície da imagem
    SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, image_surface);
    SDL_FreeSurface(image_surface);  // Liberar a superfície da memória

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
    // Centralizar a câmera no jogador
    camera->x = playerX + character_rect_dst.w / 2 - screen_width / 2;
    camera->y = playerY + character_rect_dst.h / 2 - screen_height / 2;

    // Limitar a câmera para não sair dos limites do mundo
    if (camera->x < 0) camera->x = 0;
    if (camera->y < 0) camera->y = 0;
    if (camera->x > bg_width - camera->w) camera->x = bg_width - camera->w;
    if (camera->y > bg_height - camera->h) camera->y = bg_height - camera->h;
}
