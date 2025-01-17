#include "game_utils.hpp"

Enemy enemies[MAX_ENEMIES];
std::vector<int> random_card_array(CARDS_TO_CHOSE);
Uint32 last_projectiles_times[] = { 0, 0, 0, 0 };
Uint32 last_enemy_time = 0;

void LevelUp(SDL_Renderer* g_renderer, Character& character, int& current_game_state, TTF_Font* font, DynamicText *level_text) {
    character.exp = 0;
    character.level_to_update++;
    character.level++;
    if (character.level == MAX_LEVEL)level_text->Update(g_renderer, font, "Level: MAX", { 255, 255, 255 }, { 0, 0, 0 });
    else level_text->Update(g_renderer, font, "Level: " + std::to_string(character.level), { 255, 255, 255 }, { 0, 0, 0 });
}

void UpdateEnemyPosition(Enemy* enemy, SDL_Rect player_rect) {

    if (enemy->is_dead) return;

    // Calcula a direção em que o inimigo deve se mover
    float diff_x = static_cast<float>(player_rect.x - enemy->rect_dst.x);
    float diff_y = static_cast<float>(player_rect.y - enemy->rect_dst.y);

    if (diff_x < 0) enemy->flip = SDL_FLIP_HORIZONTAL;
    else enemy->flip = SDL_FLIP_NONE; 

    // Calcula a magnitude (comprimento do vetor)
    float magnitude = sqrt(diff_x * diff_x + diff_y * diff_y);

    // Verifica se a magnitude é maior que zero antes de normalizar
    if (magnitude > 20) {
        // Normaliza a direção e move o inimigo
        enemy->rect_dst.x += static_cast<int>((diff_x / magnitude) * enemy->speed);
        enemy->rect_dst.y += static_cast<int>((diff_y / magnitude) * enemy->speed);
    }
}

void UpdateFlameballProjectilePosition(Projectile* projectile, SDL_Rect enemy_rect) {
    // Calcula a direção em que o inimigo deve se mover
    float diff_x = static_cast<float>(enemy_rect.x - projectile->rect_dst.x);
    float diff_y = static_cast<float>(enemy_rect.y - projectile->rect_dst.y);

    // Calcula a magnitude (comprimento do vetor)
    float magnitude = sqrt(diff_x * diff_x + diff_y * diff_y);

    // Verifica se a magnitude é maior que zero antes de normalizar

    if (magnitude > 20) {
        // Normaliza a direção e move o inimigo
        projectile->rect_dst.x += static_cast<int>((diff_x / magnitude) * projectile->speed);
        projectile->rect_dst.y += static_cast<int>((diff_y / magnitude) * projectile->speed);
    }
}

void UpdateAnimationCharacter(Character* character, int width, int height, int walk_frame_count, int idle_frame_count)
{
    Uint32 current_time = SDL_GetTicks();
    if (current_time > character->last_frame_time + (ANIMATION_SPEED-60)) {
        if (character->current_state == IDLE) {
            character->frame = (character->frame + 1) % walk_frame_count;
            character->rect_src.y = 0;
        }
        else if (character->current_state == WALKING) {
            character->frame = (character->frame + 1) % idle_frame_count;
            character->rect_src.y = height;
        }
        character->rect_src.x = character->frame * width;
        character->last_frame_time = current_time;
    }
};

void UpdateEnemyAnimation(Enemy* enemy)
{
    if (enemy->is_dead) return;

    Uint32 current_time = SDL_GetTicks();
    if (current_time > enemy->last_frame_time + ANIMATION_SPEED) {
        enemy->frame = (enemy->frame + 1) % enemy->total_frames;
        enemy->rect_src.y = 0;
        enemy->rect_src.x = enemy->frame * enemy->rect_src.w;
        enemy->last_frame_time = current_time;
    }
};

void UpdateProjectileAnimation(Projectile* projectile)
{
    Uint32 current_time = SDL_GetTicks();
    if (current_time > projectile->last_frame_time + projectile->animation_speed) {
        projectile->frame = (projectile->frame + 1) % projectile->total_frames;

        int frames_per_row = projectile->sprite_sheet_width / projectile->rect_src.w;
        projectile->rect_src.x = (projectile->frame % frames_per_row) * projectile->rect_src.w;
        projectile->rect_src.y = (projectile->frame / frames_per_row) * projectile->rect_src.h;

        projectile->last_frame_time = current_time;
    }
};

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

Enemy* FindClosestEnemy(SDL_Rect player_rect, Enemy enemies[], int max_enemies) {
    float closest_distance = std::numeric_limits<float>::max();
    Enemy* closest_enemy = nullptr;

    for (int i = 0; i < max_enemies; i++) {
        if (enemies[i].is_active) {
            float magnitude = CalculateMagnitude(player_rect, enemies[i].rect_dst);
            if (magnitude < closest_distance) {
                closest_distance = magnitude;
                closest_enemy = &enemies[i];
            }
        }
    }

    return closest_enemy;
}

void FireSingleProjectile(SDL_Rect player_rect, SDL_Texture* texture, float speed, int total_frames,
    SDL_Rect rect_src, SDL_Rect rect_dst,
    int delay, Uint32& last_projectile_time, Mix_Chunk* sound, projectileType type, int animation_speed)
{
    Uint32 current_time = SDL_GetTicks();
    if (current_time > last_projectile_time + delay) {
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (!projectiles[i].is_active) {           
                
                projectiles[i].type = type;
                projectiles[i].texture = texture;
                projectiles[i].speed = speed;
                projectiles[i].total_frames = total_frames;
                projectiles[i].rect_src = rect_src;
                projectiles[i].rect_dst = rect_dst;
                projectiles[i].animation_speed = animation_speed;
                projectiles[i].frames_active = 0;
                projectiles[i].frame = 0;
                projectiles[i].GetSpriteSheetWidth();

                Enemy* closest_enemy = FindClosestEnemy(player_rect, enemies, MAX_ENEMIES);
                if (closest_enemy) {
                    if (type == MAGICBALL) CalculateDirection(player_rect, closest_enemy->hitbox, &projectiles[i]);
                    projectiles[i].is_active = true;
                    Mix_PlayChannel(-1, sound, 0);
                }

                projectiles[i].last_frame_time = 0;
                last_projectile_time = current_time;
                break;
            }
        }
    }
}

void FireProjectiles(Character character, SDL_Rect camera, SDL_Texture* projectile_textures[], Mix_Chunk* projectile_sound[])
{
    // Fire MagicBall
    FireSingleProjectile(
        character.rect_dst,
        projectile_textures[MAGICBALL],
        15,                    // Speed
        5,                     // Total frames
        { 0, 0, PROJECTILE_WIDTH_ORIG, PROJECTILE_HEIGTH_ORIG },  // rect_src
        { character.rect_dst.x + character.rect_dst.w / 4, character.rect_dst.y + character.rect_dst.h / 2, 25, 25 }, // rect_dst
        character.projectile_delay,
        last_projectiles_times[MAGICBALL],
        projectile_sound[MAGICBALL],
        MAGICBALL, ANIMATION_SPEED
    );

    // Fire flame pillar
    if (character.flamepillar) {
        FireSingleProjectile(
            character.rect_dst,
            projectile_textures[FLAMEPILLAR],
            0,                    // Speed
            61,                   // Total frames
            { 0, 0, PROJECTILE_FLAMEPILLAR_WIDTH_ORIG, PROJECTILE_FLAMEPILLAR_HEIGTH_ORIG },  // rect_src
            { camera.x + rand() % 1920, camera.y + rand() % 1080,
                PROJECTILE_FLAMEPILLAR_WIDTH_ORIG + 20, PROJECTILE_FLAMEPILLAR_WIDTH_ORIG + 20
            }, // rect_dst
            character.projectile_delay + (1500 / (1 + character.flamepillar)),
            last_projectiles_times[FLAMEPILLAR],
            projectile_sound[FLAMEPILLAR],
            FLAMEPILLAR, ANIMATION_SPEED / 5
        );
    }

    // Fire Vortex
    if (character.vortex) {
        int vortex_size = PROJECTILE_VORTEX_HEIGTH_ORIG / 2 + (PROJECTILE_VORTEX_HEIGTH_ORIG/2 * character.vortex / 5);
        FireSingleProjectile(
            character.rect_dst,
            projectile_textures[VORTEX],
            0,                    // Speed
            54,                   // Total frames
            { 0, 0, PROJECTILE_VORTEX_WIDTH_ORIG, PROJECTILE_VORTEX_HEIGTH_ORIG },  // rect_src
            { character.rect_dst.x + character.rect_dst.w / 2 - vortex_size / 2, character.rect_dst.y + character.rect_dst.h / 2 - vortex_size / 2,
                vortex_size, vortex_size
            }, // rect_dst
            character.projectile_delay + (3500 - (500 * character.vortex)),
            last_projectiles_times[VORTEX],
            projectile_sound[VORTEX],
            VORTEX, ANIMATION_SPEED / 10
        );
    }

    // Fire FlameBall
    if (character.flameball) {
        FireSingleProjectile(
            character.rect_dst,
            projectile_textures[FLAMEBALL],
            2,                      // Speed
            4,                      // Total frames
            { 0, 0, PROJECTILE_FLAMEBALL_WIDTH_ORIG, PROJECTILE_FLAMEBALL_HEIGTH_ORIG },  // rect_src
            { character.rect_dst.x + character.rect_dst.w / 4, character.rect_dst.y + character.rect_dst.h / 4, PROJECTILE_FLAMEBALL_WIDTH_ORIG, PROJECTILE_FLAMEBALL_HEIGTH_ORIG }, // rect_dst
            character.projectile_delay + (2000 / (1 + character.flameball)),
            last_projectiles_times[FLAMEBALL],
            projectile_sound[FLAMEBALL],
            FLAMEBALL, ANIMATION_SPEED
        );
    }
}

void UpdateProjectiles(int width_limit, int height_limit, float multiplier, Character character)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].is_active) {

            switch (projectiles[i].type) {
            case FLAMEBALL: {
                Enemy* closest_enemy = FindClosestEnemy(projectiles[i].rect_dst, enemies, MAX_ENEMIES);
                if (closest_enemy) UpdateFlameballProjectilePosition(&projectiles[i], closest_enemy->rect_dst);
                break;
            }
            case VORTEX: {
                int vortex_size = PROJECTILE_VORTEX_HEIGTH_ORIG / 2 + (PROJECTILE_VORTEX_HEIGTH_ORIG / 2 * character.vortex / 5);

                projectiles[i].rect_dst.x = character.rect_dst.x + character.rect_dst.w / 2 - vortex_size / 2;
                projectiles[i].rect_dst.y = character.rect_dst.y + character.rect_dst.h / 2 - vortex_size / 2;

                if (projectiles[i].frame >= projectiles[i].total_frames - 1) {
                    projectiles[i].deactivate();
                    return;
                }
                break;
            }
            case FLAMEPILLAR:
                if (projectiles[i].frames_active > 200 + (50 * character.flamepillar)) projectiles[i].deactivate();
            case MAGICBALL:
                projectiles[i].rect_dst.x += static_cast<int>(projectiles[i].dir_x * projectiles[i].speed * multiplier);
                projectiles[i].rect_dst.y += static_cast<int>(projectiles[i].dir_y * projectiles[i].speed * multiplier);

                if (projectiles[i].rect_dst.x < 0 || projectiles[i].rect_dst.x > width_limit ||
                    projectiles[i].rect_dst.y < 0 || projectiles[i].rect_dst.y > height_limit) {
                    projectiles[i].is_active = false;
                }

                break;
            }

            projectiles[i].UpdateHitbox();

        }
    }
}

void SpawnEnemies(EnemyType enemy_type, SDL_Rect camera, int bg_width, int bg_height, SDL_Texture* enemy_texture, int wave, int width, int height, int frames, float size_percent)
{
    Uint32 current_time = SDL_GetTicks();
    if (current_time > last_enemy_time + ENEMY_DELAY) {

        int spawn_x = rand() % bg_width;
        SDL_Rect dst_rect = { spawn_x, rand() % bg_height, static_cast<int>(width * (1 * size_percent / 100)), static_cast<int>(height * (1 * size_percent / 100)) };

        if (CheckCollision(camera, dst_rect, camera)) {
            if (dst_rect.x > camera.x + (camera.w / 2)) dst_rect.x = camera.x + camera.w;
            else dst_rect.x = camera.x;
        }

        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].is_active) {
                float life_multiplier = (1 << (wave / 8)) / 1.85f;
                double life_additional = (wave - 1) * 1.15;
                float speed_additional = std::min((0.35f) * (wave / 8), 2.25f);

                enemies[i] = Enemy{ 6 + speed_additional, static_cast<float>(life_multiplier * (1 + life_additional)), frames,
                    { 0, 0, width, height },  //rect_src
                    dst_rect, //dest_dst
                    enemy_texture, //texture
                    true, enemy_type, false };

                last_enemy_time = current_time;
                break;
            }
        }
    }
}

void CharacterTookDamage(Character *character, DynamicText* life_text, TTF_Font* small_font, Mix_Chunk* damage_sound, int& current_game_state) {
    Uint32 current_time = SDL_GetTicks();
    if (current_time > character->last_damage_time + DAMAGE_COOLDOWN) {
        character->life -= 1;
        character->took_damage = true;
        life_text->Update(g_renderer, small_font, "Lifes: " + std::to_string(static_cast<int>(character->life)), { 255, 255, 255 }, { 0, 0, 0 });
        character->last_damage_time = current_time;
        Mix_PlayChannel(-1, damage_sound, 0);
        if (character->life <= 0) {
            current_game_state = GAME_OVER;
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
    size_t MAX_SIZE = cards.size();
    std::vector<int> eligible_cards;
    std::vector<int> not_eligible_cards;

    // Reserva espaço para evitar realocações
    eligible_cards.reserve(MAX_SIZE);
    not_eligible_cards.reserve(MAX_SIZE);

    // Classifica as cartas em elegíveis e não elegíveis
    for (int i = 0; i < cards.size(); i++) {
        if (cards[i].level < 5)
            eligible_cards.push_back(i);
        else
            not_eligible_cards.push_back(i);
    }

    // Preenche com cartas de nível máximo caso necessário
    while (eligible_cards.size() < random_card_array.size()) {
        size_t random_index = rand() % not_eligible_cards.size();
        eligible_cards.push_back(not_eligible_cards[random_index]);

        // Remove a carta de not_eligible_cards
        not_eligible_cards.erase(not_eligible_cards.begin() + random_index);
    }

    // Preenche random_card_array com cartas aleatórias
    for (int i = 0; i < random_card_array.size(); i++) {
        size_t random_index = rand() % eligible_cards.size();
        random_card_array[i] = eligible_cards[random_index];

        // Remove a carta de eligible_cards para evitar repetições
        eligible_cards.erase(eligible_cards.begin() + random_index);
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
    life_text->Update(g_renderer, font, "Lifes: " + std::to_string(static_cast<int>(character->life)), { 255, 255, 255 }, { 0, 0, 0 });
    level_text->Update(g_renderer, font, "Level: " + std::to_string(character->level), { 255, 255, 255 }, { 0, 0, 0 });
    for (int i = 0; i < cards.size(); ++i) {
        cards[i].level = 0;
    }
}

void SelectCard(std::string card_name, Character& character, TTF_Font* font, DynamicText* life_text)
{
    if      (card_name == "Fire Rate")  character.projectile_delay = static_cast<int>(character.projectile_delay * 0.90);
    else if (card_name == "Heal") {     character.life++; life_text->Update(g_renderer, font, "Lifes: " + std::to_string(static_cast<int>(character.life)), { 255, 255, 255 }, { 0, 0, 0 }); }
    else if (card_name == "Speed")      character.speed += 0.25f;
    else if (card_name == "Damage")     character.damage++;
    else if (card_name == "EXP")        character.exp_multiplier += 0.5;
    else if (card_name == "Flameball")  character.flameball += 1;
    else if (card_name == "Vortex")     character.vortex += 1;
    else if (card_name == "Flame Pillar")     character.flamepillar += 1;
    else if (card_name == "Projectile Speed") character.projectile_speed_multiplier += 0.5;
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

        if (dx < 0) character->flip = SDL_FLIP_HORIZONTAL;
        else character->flip = SDL_FLIP_NONE;

        character->current_state = WALKING;
        character->UpdateHitbox();
    }
}