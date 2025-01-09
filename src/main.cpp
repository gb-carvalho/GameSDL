#include <SDL.h>
#include <string>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <SDL_render.h>

#include "entity.hpp"
#include "character.hpp"
#include "enemy.hpp"
#include "card.hpp"
#include "projectile.hpp"
#include "dynamic_text.hpp"
#include "init.hpp"
#include "collision.hpp"
#include "game_utils.hpp"
#include "render.hpp"

int main(int argc, char* argv[]) 
{

    Init();

    if (TTF_Init() == -1) {
        SDL_Log("Error init SDL_ttf: %s", TTF_GetError());
        return -1;
    }

    //Get diplay size, maybe make this a func?
    SDL_DisplayMode displayMode;
    int screen_width = 0, screen_height = 0;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
        screen_width = displayMode.w;
        screen_height = displayMode.h;
    }
    else SDL_Log("Error getting monitor resolution: %s", SDL_GetError());

    InitWindow(screen_width, screen_height, &g_window);
    InitRenderer(&g_window, &g_renderer);

    InitSDLMusic();
    Mix_Music* music = InitMusic("Assets/music.mp3", -1);
    Mix_Chunk* damage_sound =   InitSoundEffect("Assets/character_damage.wav", MIX_MAX_VOLUME / 7);
    Mix_Chunk* enemy_damage_sound = InitSoundEffect("Assets/enemy_damage.wav", MIX_MAX_VOLUME / 15);
    Mix_Chunk* projectile_sound = InitSoundEffect("Assets/projectile.wav", MIX_MAX_VOLUME / 5);
    Mix_Chunk* fire1_sound = InitSoundEffect("Assets/fire1.wav", MIX_MAX_VOLUME / 8);
    Mix_Chunk* fire2_sound = InitSoundEffect("Assets/fire2.wav", MIX_MAX_VOLUME / 8);
    Mix_Chunk* vortex_sound = InitSoundEffect("Assets/whoosh1.wav", MIX_MAX_VOLUME / 14);
    Mix_Chunk* projetiles_sound[] = {projectile_sound, fire1_sound, vortex_sound, fire2_sound };


    SDL_Rect camera = { 0, 0, screen_width, screen_height };

    //Background
    SDL_Texture* bg_texture = CreateTextureImg("Assets/background.png");
    int bg_width, bg_height;
    SDL_QueryTexture(bg_texture, NULL, NULL, &bg_width, &bg_height);

    Character character{7, 3,
        { 0, 0, CHARACTER_WIDTH_ORIG, CHARACTER_HEIGHT_ORIG }, //rect_src
        { bg_width / 2, bg_height / 2, CHARACTER_WIDTH_RENDER, CHARACTER_HEIGHT_RENDER }, //rect_dst
        CreateTextureImg("Assets/batata_spritesheet.png"), //texture
        IDLE, CHARACTER_PROJECTILE_DELAY};

    SDL_Texture* projectile_texture = CreateTextureImg("Assets/mage-bullet-13x13.png");
    SDL_Texture* projectile_flameball_texture = CreateTextureImg("Assets/flameball-32x32.png");
    SDL_Texture* projectile_vortex_texture = CreateTextureImg("Assets/Effect_TheVortex_1_429x429.png");
    SDL_Texture* projectile_flame_pillar_texture = CreateTextureImg("Assets/flame_pillar.png");
    SDL_Texture* projectile_textures[] = { projectile_texture, projectile_flameball_texture, projectile_vortex_texture, projectile_flame_pillar_texture };

    SDL_Texture* mage_texture = CreateTextureImg("Assets/mage.png");
    SDL_Texture* mage2_texture = CreateTextureImg("Assets/mage2.png");
    SDL_Texture* mage3_texture = CreateTextureImg("Assets/mage3.png");
    SDL_Texture* wolf_texture = CreateTextureImg("Assets/wolf.png");
    SDL_Texture* bat_texture = CreateTextureImg("Assets/bat.png");
    SDL_Texture* golem_texture = CreateTextureImg("Assets/golem.png");
    SDL_Texture* andromalius_texture = CreateTextureImg("Assets/andromalius2.png");
    SDL_Texture* title_texture = CreateTextureImg("Assets/title.png");

    //Font
    TTF_Font* font = TTF_OpenFont("Assets/GeoSlab703 Md BT Medium.ttf", 128);
    if (!font) {
        SDL_Log("Error loading font: %s", TTF_GetError());
        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        TTF_Quit();
        SDL_Quit();
    }
    TTF_Font* small_font = TTF_OpenFont("Assets/GeoSlab703 Md BT Medium.ttf", 24);
    if (!small_font) {
        SDL_Log("Error loading font: %s", TTF_GetError());
        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        TTF_Quit();
        SDL_Quit();
    }

    //Init seed for random number
    srand(static_cast<unsigned int>(time(nullptr)));

    int card_selected = 1;
    SDL_Event event;
    bool running = true;
    int current_game_state = TITLE_SCREEN;
    int kill_count, start_time, elapsed_time, wave, time_left, pause_start_time = 0, total_pause_duration = 0;
    bool key_pressed = false, skip = false, allLevelsAreFive;
    DynamicText life_text, level_text, title_text, pause_text, kill_count_text, stopwatch_text;

    while (running) {

        const Uint8* keyState = SDL_GetKeyboardState(NULL);

        if (current_game_state != CARD_SELECTOR) {
            while (SDL_PollEvent(&event)) {
                // Verificar se o jogo foi fechado
                if (event.type == SDL_QUIT) {
                    running = false;
                }
            }
        }

        switch (current_game_state) {
        case TITLE_SCREEN:
            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
            SDL_RenderClear(g_renderer);

            SDL_RenderCopy(g_renderer, title_texture, nullptr, nullptr);

            title_text.Update(g_renderer, font, "Batata Game", { 0, 0, 0 }, { 255, 255, 255 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 4 - title_text.rect.h / 2, true);
            title_text.Update(g_renderer, small_font, "Press Enter to start", { 255, 255, 255 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 2 - title_text.rect.h / 2 + 20, true);

            LoadGame(SAVE_FILE, kill_count, wave);
            title_text.Update(g_renderer, small_font, "Kill record: " + std::to_string(kill_count), { 0, 0, 0 }, { 255, 50, 50 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 2 + 360, true);
            title_text.Update(g_renderer, small_font, "Wave record: " + std::to_string(wave), { 0, 0, 0 }, { 255, 50, 50 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 2 + 390, true);

            if (keyState[SDL_SCANCODE_RETURN]) {
                ResetGame(kill_count, wave, &character, bg_width, bg_height, start_time, elapsed_time, small_font, total_pause_duration, 
                          &stopwatch_text, &life_text, &kill_count_text, &level_text);
                current_game_state = PLAYING;
            }
            break;

        case PLAYING: {

            if (keyState[SDL_SCANCODE_ESCAPE]) current_game_state = PAUSE;

            MoveCharacter(&character, keyState, bg_width, bg_height);
            UpdateAnimationCharacter(&character, CHARACTER_WIDTH_ORIG, CHARACTER_HEIGHT_ORIG, WALK_FRAME_COUNT, IDLE_FRAME_COUNT);
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].is_active) UpdateEnemyAnimation(&enemies[i]);
            }
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].is_active) UpdateProjectileAnimation(&projectiles[i]);
            }

            UpdateCamera(character.rect_dst.x, character.rect_dst.y, &camera, character.rect_dst, bg_width, bg_height, screen_width, screen_height);

            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
            SDL_RenderClear(g_renderer);

            SDL_Rect bg_render_rect = { 0, 0, bg_width, bg_height };
            SDL_RenderCopy(g_renderer, bg_texture, &camera, nullptr);

            
            DamageColor(character.texture, character.last_damage_time, character.took_damage);
            SDL_Rect player_render_rect = {
                 character.rect_dst.x - camera.x,
                 character.rect_dst.y - camera.y,
                 character.rect_dst.w, character.rect_dst.h
            };
            
            int wave_type = (wave - 1) % 7 + 1;
            if (wave_type == 1)      SpawnEnemies(BAT, camera, bg_width, bg_height, bat_texture, wave, ENEMY_BAT_WIDTH, ENEMY_BAT_HEIGHT, ENEMY_BAT_FRAMES, 100);
            else if (wave_type == 2) SpawnEnemies(WOLF, camera, bg_width, bg_height, wolf_texture, wave, ENEMY_WOLF_WIDTH, ENEMY_WOLF_HEIGHT, ENEMY_WOLF_FRAMES, 150);
            else if (wave_type == 3) SpawnEnemies(GOLEM, camera, bg_width, bg_height, golem_texture, wave, ENEMY_GOLEM_WIDTH, ENEMY_GOLEM_HEIGHT, ENEMY_GOLEM_FRAMES, 140);
            else if (wave_type == 4) SpawnEnemies(ANDROMALIUS, camera, bg_width, bg_height, andromalius_texture, wave, ENEMY_ANDROMALIUS_WIDTH, ENEMY_ANDROMALIUS_HEIGHT, ENEMY_ANDROMALIUS_FRAMES, 100);
            else if (wave_type == 5) SpawnEnemies(MAGE, camera, bg_width, bg_height, mage_texture, wave, ENEMY_MAGE_WIDTH, ENEMY_MAGE_HEIGHT, ENEMY_MAGE_FRAMES, 100);
            else if (wave_type == 6) SpawnEnemies(MAGE2, camera, bg_width, bg_height, mage2_texture, wave, ENEMY_MAGE2_WIDTH, ENEMY_MAGE2_HEIGHT, ENEMY_MAGE2_FRAMES, 100);
            else if (wave_type == 7) SpawnEnemies(MAGE3, camera, bg_width, bg_height, mage3_texture, wave, ENEMY_MAGE3_WIDTH, ENEMY_MAGE3_HEIGHT, ENEMY_MAGE3_FRAMES, 100);

            FireProjectiles(character, camera, projectile_textures, projetiles_sound);
            UpdateProjectiles(bg_width, bg_height, character.projectile_speed_multiplier, character);

            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].is_active) {
                    UpdateEnemyPosition(&enemies[i], character.rect_dst);
                    SDL_Rect enemy_render_rect = {
                        enemies[i].rect_dst.x - camera.x,
                        enemies[i].rect_dst.y - camera.y,
                        enemies[i].rect_dst.w,
                        enemies[i].rect_dst.h
                    };
                    enemies[i].UpdateHitbox();
                    SDL_RenderCopyEx(g_renderer, enemies[i].texture, &enemies[i].rect_src, &enemy_render_rect, 0, NULL, enemies[i].flip);

                    if (CheckCollision(character.hitbox, enemies[i].hitbox, camera)) {
                        Uint32 current_time = SDL_GetTicks();
                        if (current_time > character.last_damage_time + DAMAGE_COOLDOWN) {
                            character.life -= 1;
                            character.took_damage = true;
                            life_text.Update(g_renderer, small_font, "Lifes: " + std::to_string(static_cast<int>(character.life)), { 255, 255, 255 }, { 0, 0, 0 });
                            character.last_damage_time = current_time;
                            Mix_PlayChannel(-1, damage_sound, 0);
                            if (character.life <= 0) {
                                current_game_state = GAME_OVER;
                            }
                        }
                    }

                    CheckProjectileCollisionWithEnemy(g_renderer, character, enemies[i].hitbox, enemies[i].life, enemies[i].is_active, camera, kill_count, 
                        small_font, current_game_state, &kill_count_text, &level_text, enemy_damage_sound);

                    for (int j = i + 1; j < MAX_ENEMIES; j++) {
                        if (!enemies[j].is_active || resolved_collision[i][j]) continue;
                        resolveCollision(&enemies[i].rect_dst, &enemies[j].rect_dst);
                        resolved_collision[i][j] = true;
                        resolved_collision[j][i] = true;
                    }
                }
            }

            RenderHeader(start_time, time_left, screen_width, elapsed_time, wave, current_game_state, small_font, character, total_pause_duration,
                         &stopwatch_text, &life_text, &kill_count_text, &level_text);
            if (time_left <= 0) NewWave(current_game_state, wave, skip);  

            memset(resolved_collision, 0, sizeof(resolved_collision));

            RenderProjectiles(camera);
            SDL_RenderCopy(g_renderer, character.texture, &character.rect_src, &player_render_rect);
            break;
        }
        case CARD_SELECTOR: {
            if (character.level_to_update == 0 || skip) {
                current_game_state = PLAYING;
                total_pause_duration = 0;
                character.rect_dst = { bg_width / 2, bg_height / 2, CHARACTER_WIDTH_RENDER, CHARACTER_HEIGHT_RENDER };
                character.pos_x = static_cast<float>(character.rect_dst.x);
                character.pos_y = static_cast<float>(character.rect_dst.y);
                start_time = SDL_GetTicks();

                for (int i = 0; i < MAX_ENEMIES; i++) {
                    enemies[i].deactivate();
                }
                for (int i = 0; i < MAX_PROJECTILES; i++) {
                    projectiles[i].deactivate();
                }
            }
            else {
                SDL_RenderCopy(g_renderer, bg_texture, &camera, nullptr);
                RenderHeader(start_time, time_left, screen_width, elapsed_time, wave, current_game_state, small_font, character, total_pause_duration,
                             &stopwatch_text, &life_text, &kill_count_text, &level_text);
                RenderCardSelection(card_selected, small_font, screen_width, screen_height, character.level_to_update);
            }

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                else if (event.type == SDL_KEYDOWN && !key_pressed) {
                    key_pressed = true;

                    switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_RETURN:

                        allLevelsAreFive = true;
                        for (int i = 0; i < random_card_array.size(); ++i) {
                            if (cards[random_card_array[i]].level != 5) {
                                allLevelsAreFive = false;
                                break;
                            }
                        }

                        if (allLevelsAreFive) {
                            skip = true;
                            break;
                        }

                        if (skip) current_game_state = PLAYING;

                        if (cards[random_card_array[card_selected]].level < 5) {
                            SelectCard(cards[random_card_array[card_selected]].name, character, small_font, &life_text);
                            cards[random_card_array[card_selected]].level++;
                            character.level_to_update--;
                            randomizeCardArray();
                        }
                        break;

                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_LEFT:
                        card_selected--;
                        if (card_selected < 0) card_selected = CARDS_TO_CHOSE - 1;
                        break;

                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
                        card_selected++;
                        if (card_selected >= CARDS_TO_CHOSE) card_selected = 0;
                        break;

                    default:
                        break;
                    }
                }
                else if (event.type == SDL_KEYUP) {
                    key_pressed = false;
                }
            }

            break;
        }
        case GAME_OVER: {


            int kill_count_save_file, wave_save_file;
            LoadGame("sdl.dat", kill_count_save_file, wave_save_file);

            SDL_Rect dark_rect_dst = { 0, 0, screen_width, screen_height };
            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 2);
            SDL_RenderFillRect(g_renderer, &dark_rect_dst);

            title_text.Update(g_renderer, font, "You died", { 238, 173, 45 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, static_cast<int>(screen_height / 1.5 - title_text.rect.h / 2), true);
            title_text.Update(g_renderer, small_font, "Press Enter to restart", { 255, 255, 255 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, static_cast<int>(screen_height / 1.5 - title_text.rect.h / 2 + 90), true);
            title_text.Update(g_renderer, small_font, "Press ESC to go to the title screen.", { 255, 255, 255 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, static_cast<int>(screen_height / 1.5 - title_text.rect.h / 2 + 120), true);

            if (kill_count > kill_count_save_file || wave > wave_save_file) {
                title_text.Update(g_renderer, small_font, "Congratulations! You've set a new record!", { 255, 50, 50 }, { 0, 0, 0 });
                title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, static_cast<int>(screen_height / 1.5 - title_text.rect.h / 2 + 150), true);
            }

            SaveGame(SAVE_FILE, kill_count, wave);

            if (keyState[SDL_SCANCODE_RETURN] || keyState[SDL_SCANCODE_ESCAPE]) {
                ResetGame(kill_count, wave, &character, bg_width, bg_height, start_time, elapsed_time, small_font, total_pause_duration,
                          &stopwatch_text, &life_text, &kill_count_text, &level_text);
                if (keyState[SDL_SCANCODE_ESCAPE]) current_game_state = TITLE_SCREEN;
                else if (keyState[SDL_SCANCODE_RETURN]) current_game_state = PLAYING;
            }
            break;
        }
        case PAUSE:

            if (pause_start_time == 0) pause_start_time = SDL_GetTicks();

            title_text.Update(g_renderer, font, "Paused", { 238, 173, 45 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 2 - title_text.rect.h / 2, true);
            title_text.Update(g_renderer, small_font, "Press Enter to resume", { 255, 255, 255 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 2 - title_text.rect.h / 2 + 90, true);

            SDL_Rect dark_rect_dst = { 0, 0, screen_width, screen_height };
            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 2);
            SDL_RenderFillRect(g_renderer, &dark_rect_dst);

            if (keyState[SDL_SCANCODE_RETURN]) {
                total_pause_duration += SDL_GetTicks() - pause_start_time;
                pause_start_time = 0;
                last_projectiles_times[MAGICBALL] = SDL_GetTicks();
                last_projectiles_times[FLAMEBALL] = SDL_GetTicks();
                last_projectiles_times[VORTEX] = SDL_GetTicks();
                last_projectiles_times[FLAMEPILLAR] = SDL_GetTicks();
                current_game_state = PLAYING;
            }
            break;
        }
        // Apresenta o conteúdo renderizado
        SDL_RenderPresent(g_renderer);
        SDL_Delay(16);
    }

    // Limpeza
    Mix_CloseAudio();
    Mix_HaltMusic();
    Mix_FreeMusic(music);
    TTF_CloseFont(font);
    TTF_CloseFont(small_font);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}