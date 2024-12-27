#include "game_utils.hpp"

Enemy enemies[MAX_ENEMIES];
std::vector<int> random_card_array(CARDS_TO_CHOSE);
Uint32 last_projectile_time = 0;
Uint32 last_enemy_time = 0;

void LevelUp(SDL_Renderer* g_renderer, Character& character, int& current_game_state, TTF_Font* font, DynamicText *level_text) {
    character.exp = 0;
    character.level_to_update++;
    character.level++;
    level_text->Update(g_renderer, font, "Level: " + std::to_string(character.level), { 255, 255, 255 }, { 0, 0, 0 });
}

void UpdateEnemyPosition(SDL_Rect* enemy_rect_dst, SDL_Rect player_rect, int enemy_speed) {
    // Calcula a direção em que o inimigo deve se mover
    float diff_x = static_cast<float>(player_rect.x - enemy_rect_dst->x);
    float diff_y = static_cast<float>(player_rect.y - enemy_rect_dst->y);

    // Calcula a magnitude (comprimento do vetor)
    float magnitude = sqrt(diff_x * diff_x + diff_y * diff_y);

    // Verifica se a magnitude é maior que zero antes de normalizar

    if (magnitude > 20) {
        // Normaliza a direção e move o inimigo
        enemy_rect_dst->x += static_cast<int>((diff_x / magnitude) * enemy_speed);
        enemy_rect_dst->y += static_cast<int>((diff_y / magnitude) * enemy_speed);
    }
}

void UpdateAnimation(characterState current_state, SDL_Rect& src_rect, int& frame, Uint32& last_frame_time, int width, int height,
    int walk_frame_count, int idle_frame_count)
{
    Uint32 current_time = SDL_GetTicks();
    if (current_time > last_frame_time + ANIMATION_SPEED) {
        if (current_state == WALKING || current_state == -1) {
            frame = (frame + 1) % walk_frame_count;
            src_rect.y = 0;
        }
        else if (current_state == IDLE) {
            frame = (frame + 1) % idle_frame_count;
            src_rect.y = height;
        }
        src_rect.x = frame * width;
        last_frame_time = current_time;
    }
};

float CalculateMagnitude(SDL_Rect a, SDL_Rect b)
{
    float a_center_x = static_cast<float>(a.x + a.w / 2);
    float a_center_y = static_cast<float>(a.y + a.h / 2);
    float b_center_x = static_cast<float>(b.x + b.w / 2);
    float b_center_y = static_cast<float>(b.y + b.h / 2);

    float diff_x = static_cast<float>(a_center_x - b_center_x);
    float diff_y = static_cast<float>(a_center_y - b_center_y);

    float magnitude = sqrt(diff_x * diff_x + diff_y * diff_y);

    return magnitude;
}

void CalculateDirection(SDL_Rect a, SDL_Rect b, Projectile* projectile)
{
    float a_center_x = static_cast<float>(a.x + a.w / 2);
    float a_center_y = static_cast<float>(a.y + a.h / 2);
    float b_center_x = static_cast<float>(b.x + b.w / 2);
    float b_center_y = static_cast<float>(b.y + b.h / 2);

    float diff_x = b_center_x - a_center_x;
    float diff_y = b_center_y - a_center_y;

    float magnitude = sqrt(diff_x * diff_x + diff_y * diff_y);

    if (magnitude > 0) {
        projectile->dir_x = diff_x / magnitude;
        projectile->dir_y = diff_y / magnitude;
    }
    else {
        projectile->dir_x = 1;
        projectile->dir_y = 1;
    }
}

void FireProjectile(SDL_Rect player_rect, SDL_Texture* projectile_texture, int projectile_delay)
{
    float magnitude;

    Uint32 current_time = SDL_GetTicks();
    if (current_time > last_projectile_time + projectile_delay) {
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (!projectiles[i].is_active) {

                float closest_distance = std::numeric_limits<float>::max();
                Enemy* closest_enemy = nullptr;

                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (enemies[j].is_active) {
                        magnitude = CalculateMagnitude(player_rect, enemies[j].rect_dst);
                        if (magnitude < closest_distance) {
                            closest_distance = magnitude;
                            closest_enemy = &enemies[j];
                        }
                    }
                }

                projectiles[i].texture = projectile_texture;
                projectiles[i].rect_src = { 0, 0, PROJECTILE_WIDTH_ORIG, PROJECTILE_HEIGTH_ORIG };
                projectiles[i].rect_dst = { player_rect.x + player_rect.w / 2, player_rect.y + player_rect.h / 2, 25, 25 };
                if (closest_enemy) CalculateDirection(player_rect, closest_enemy->rect_dst, &projectiles[i]);
                if (closest_enemy) projectiles[i].is_active = true;
                projectiles[i].last_frame_time = 0;
                last_projectile_time = current_time;
                break;
            }
        }
    }
}

void UpdateProjectiles(int width_limit, int height_limit, float multiplier)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].rect_dst.x += static_cast<int>(projectiles[i].dir_x * PROJECTILE_SPEED * multiplier);
        projectiles[i].rect_dst.y += static_cast<int>(projectiles[i].dir_y * PROJECTILE_SPEED * multiplier);
        projectiles[i].UpdateHitbox();
        if (projectiles[i].rect_dst.x < 0 || projectiles[i].rect_dst.x > width_limit ||
            projectiles[i].rect_dst.y < 0 || projectiles[i].rect_dst.y > height_limit) {
            projectiles[i].is_active = false;
        }
    }
}


void SpawnEnemies(int bg_width, int bg_height, SDL_Texture* enemy_texture, int wave)
{
    Uint32 current_time = SDL_GetTicks();
    if (current_time > last_enemy_time + ENEMY_DELAY) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].is_active) {
                enemies[i] = Enemy{ 6, 1 + (wave - 1), 0, 0,
                    { 0, 0, ENEMY_MAGE_WIDTH_ORIG, ENEMY_MAGE_HEIGHT_ORIG },  //rect_src
                    { rand() % bg_width, rand() % bg_height, ENEMY_MAGE_WIDTH_ORIG, ENEMY_MAGE_HEIGHT_ORIG }, //dest_dst
                    enemy_texture, //texture
                    true };

                last_enemy_time = current_time;
                break;
            }
        }
    }
}

std::string TimeFormatted(int time_in_seconds) {
    int minutes = time_in_seconds / 60;
    int seconds = time_in_seconds % 60;

    std::string time_formatted =
        (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
        (seconds < 10 ? "0" : "") + std::to_string(seconds);

    return time_formatted;
}

void randomizeCardArray() {
    for (int i = 0; i < random_card_array.size(); i++) {
        random_card_array[i] = rand() % cards.size();
    }
}

void NewWave(int& current_game_state, int& wave, bool& skip) {
    randomizeCardArray();
    wave++;
    current_game_state = CARD_SELECTOR;
    skip = false;
}


void LoadGame(const std::string& file_name, int& kill_count, int& wave)
{
    std::ifstream save_file(file_name, std::ios::binary);
    if (!save_file.is_open()) {
        SDL_Log("Error opening save file\n");
        kill_count = 0;
        wave = 0;
        return;
    }

    save_file.read(reinterpret_cast<char*>(&kill_count), sizeof(kill_count));
    save_file.read(reinterpret_cast<char*>(&wave), sizeof(wave));

    save_file.close();
}

void SaveGame(const std::string& file_name, int kill_count, int wave)
{
    int kill_count_save_file, wave_save_file;

    LoadGame(file_name, kill_count_save_file, wave_save_file);

    std::ofstream save_file(file_name, std::ios::binary);
    if (!save_file.is_open()) {
        SDL_Log("Error opening save file\n");
        return;
    }

    if (kill_count > kill_count_save_file) save_file.write(reinterpret_cast<const char*>(&kill_count), sizeof(kill_count));
    else save_file.write(reinterpret_cast<const char*>(&kill_count_save_file), sizeof(kill_count_save_file));

    if (wave > wave_save_file) save_file.write(reinterpret_cast<const char*>(&wave), sizeof(wave));
    else save_file.write(reinterpret_cast<const char*>(&wave_save_file), sizeof(wave_save_file));

    save_file.close();
}

void ResetGame(int& kill_count, int& wave, Character* character, int bg_width, int bg_height, int& start_time, int& elapsed_time, TTF_Font* font, int& total_pause_duration, 
               DynamicText* stopwatch_text, DynamicText* life_text, DynamicText* kill_count_text, DynamicText* level_text)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].deactivate();
    }

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].deactivate();
    }

    wave = 1;
    kill_count = 0;
    total_pause_duration = 0;
    character->reset({ bg_width / 2, bg_height / 2, CHARACTER_WIDTH_RENDER, CHARACTER_HEIGHT_RENDER });
    character->UpdateHitbox();
    start_time = SDL_GetTicks();
    elapsed_time = (SDL_GetTicks() - start_time) / 1000;
    stopwatch_text->Update(g_renderer, font, std::to_string(elapsed_time), { 255, 255, 255 }, { 0, 0, 0 });
    kill_count_text->Update(g_renderer, font, "Enemies killed: " + std::to_string(kill_count), { 255, 255, 255 }, { 0, 0, 0 });
    life_text->Update(g_renderer, font, "Lifes: " + std::to_string(character->life), { 255, 255, 255 }, { 0, 0, 0 });
    level_text->Update(g_renderer, font, "Level: " + std::to_string(character->level), { 255, 255, 255 }, { 0, 0, 0 });
    for (int i = 0; i < cards.size(); ++i) {
        cards[i].level = 0;
    }
}

void SelectCard(std::string card_name, Character& character, TTF_Font* font, DynamicText* life_text)
{
    if (card_name == "Fire Rate") character.projectile_delay = static_cast<int>(character.projectile_delay * 0.90);
    else if (card_name == "Heal") { character.life++; life_text->Update(g_renderer, font, "Lifes: " + std::to_string(character.life), { 255, 255, 255 }, { 0, 0, 0 }); }
    else if (card_name == "Speed") character.speed++;
    else if (card_name == "Damage") character.damage++;
    else if (card_name == "Projectile Speed") character.projectile_speed_multiplier += 0.5;
    else if (card_name == "EXP") character.exp_multiplier += 0.5;
}

void DamageColor(SDL_Texture* texture, Uint32 last_damage_time, bool& took_damage)
{
    if (took_damage) {
        SDL_SetTextureColorMod(texture, 255, 0, 0);
        if (SDL_GetTicks() > last_damage_time + 200) {
            took_damage = false;
        }
    }
    else {
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }
}

void MoveCharacter(Character* character, const Uint8* keyState, int bg_width, int bg_height)
{
    float dy = 0, dx = 0;
    character->current_state = IDLE;

    if ((keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP]) && character->rect_dst.y > 0) {
        dy -= 1;
    }

    if ((keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN]) && character->rect_dst.y < bg_height - CHARACTER_HEIGHT_RENDER) {
        dy += 1;
    }

    if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && character->rect_dst.x > 0) {
        dx -= 1;
    }

    if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT]) && character->rect_dst.x < bg_width - CHARACTER_WIDTH_RENDER) {
        dx += 1;
    }

    float magnitude = sqrt(dx * dx + dy * dy);

    if (magnitude > 0) {
        dx = (dx / magnitude) * character->speed;
        dy = (dy / magnitude) * character->speed;
    }

    character->pos_x += dx;
    character->pos_y += dy;

    character->rect_dst.x = static_cast<int>(character->pos_x);
    character->rect_dst.y = static_cast<int>(character->pos_y);

    if (dx != 0 || dy != 0) {
        character->current_state = WALKING;
        character->UpdateHitbox();
    }
}