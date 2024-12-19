#include <SDL.h>
#include <string>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <SDL_render.h>

#define CHARACTER_WIDTH_ORIG    32 //Size of character width in sprite
#define CHARACTER_HEIGHT_ORIG   32 //Size of character height in sprite
#define CHARACTER_WIDTH_RENDER  64 //Size of character width in render
#define CHARACTER_HEIGHT_RENDER 64 //Size of character height in render

#define ENEMY_MAGE_WIDTH_ORIG  85
#define ENEMY_MAGE_HEIGHT_ORIG 94
#define PROJECTILE_WIDTH_ORIG  13
#define PROJECTILE_HEIGTH_ORIG 13
#define CARD_WIDTH             542
#define CARD_HEIGHT            809

#define ANIMATION_SPEED  160
#define WALK_FRAME_COUNT 4
#define IDLE_FRAME_COUNT 2

#define PROJECTILE_DELAY 1000
#define PROJECTILE_SPEED 15
#define MAX_PROJECTILES  100

#define DAMAGE_COOLDOWN  500
#define ENEMY_DELAY      500
#define MAX_ENEMIES      20
#define MAX_EXP          20
#define MAX_CARDS        3
#define MAX_CARD_LEVEL   5

#define SAVE_FILE "sdl.dat"

enum characterState { IDLE, WALKING };
enum gameState { TITLE_SCREEN, PLAYING, CARD_SELECTOR, GAME_OVER, PAUSE };

class Entity {
public:
    int speed;
    int life;
    int frame;
    Uint32 last_frame_time;
    SDL_Rect rect_src, rect_dst, hitbox;
    SDL_Texture* texture;

    Entity()
        : speed(0), life(0), frame(0), last_frame_time(0),
        rect_src{ 0, 0, 0, 0 }, rect_dst{ 0, 0, 0, 0 }, hitbox{ 0, 0, 0, 0 },
        texture(nullptr) {}

    Entity(int spd, int lfe, int frm, int lftime, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex)
        : speed(spd), life(lfe), frame(frm), last_frame_time(lftime), rect_src(src), rect_dst(dst), texture(tex) {
        UpdateHitbox();
    }

    virtual void UpdateHitbox() {
        hitbox = rect_dst;
    }

    //virtual ~Entity() {
    //    SDL_DestroyTexture(texture);
    //}

    //void render(SDL_Renderer* renderer) {
    //    SDL_RenderCopy(renderer, texture, &rect_src, &rect_dst);
    //}
};

class Character : public Entity {
public:
    characterState current_state;
    int exp, level, level_to_update, projectile_delay, last_damage_time;
    bool took_damage;

    Character(int spd, int lfe, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, characterState state, int prjctle_delay)
        : Entity(spd, lfe, 0, 0, src, dst, tex), current_state(state), exp(0), level(0), 
            projectile_delay(prjctle_delay), last_damage_time(0), took_damage(false), level_to_update(0) {
        UpdateHitbox();
    }

    void UpdateHitbox() override{
        hitbox.w = rect_dst.w * 0.5;
        hitbox.h = rect_dst.h * 0.9;

        hitbox.x = rect_dst.x + (rect_dst.w - hitbox.w) / 2;
        hitbox.y = rect_dst.y + (rect_dst.h - hitbox.h) / 2;
    }
  
    void reset(SDL_Rect rect_dst_new) {
        life = 3;
        frame = 0;
        last_frame_time = 0;
        rect_dst = rect_dst_new;
        exp = 0;
        level = 1;
        level_to_update = 0;
        speed = 7;
        projectile_delay = PROJECTILE_DELAY;
    }

    void updateState(characterState newState) {
        current_state = newState;
    }
};

// Classe para inimigos (herda de Entity)
class Enemy : public Entity {
public:
    bool is_active;

    Enemy()
        : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), is_active(false) {}

    Enemy(int spd, int lfe, int frm, int lftime, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active)
        : Entity(spd, lfe, frm, lftime, src, dst, tex), is_active(active) {
        UpdateHitbox();
    }

    void UpdateHitbox() override {
        hitbox.w = rect_dst.w * 0.4;
        hitbox.h = rect_dst.h * 0.85;

        hitbox.x = rect_dst.x + (rect_dst.w - hitbox.w) / 2 + 5;
        hitbox.y = rect_dst.y + (rect_dst.h - hitbox.h) / 2 + 10;
    }

    void deactivate() {
        is_active = false;
    }
};

class Projectile : public Entity {
public:
    bool is_active;
    float dir_x, dir_y;

    Projectile()
        : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), is_active(false), dir_x(0.0f), dir_y(0.0f) {}

    Projectile(int spd, int lfe, int frm, int lftime, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active, float dx, float dy)
        : Entity(spd, lfe, frm, lftime, src, dst, tex), is_active(active), dir_x(dx), dir_y(dy) {
        //UpdateHitbox();
    }

    void UpdateHitbox() override {
        hitbox.w = rect_dst.w;
        hitbox.h = rect_dst.h;

        hitbox.x = rect_dst.x;
        hitbox.y = rect_dst.y;
    }

    void deactivate() {
        is_active = false;
    }
};


class Card : public Entity {
public:
    std::string name, description;
    int level;

    Card()
        : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), name(""), description(""), level(0) {}

    Card(SDL_Rect src, SDL_Rect dst, std::string nme, std::string dscrption, int lvl)
        : Entity(0, 0, 0, 0, src, dst, nullptr), name(nme), description(dscrption), level(lvl){
        //UpdateHitbox();
    }
};

std::vector<Card> cards = {
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Fire Rate", "Shoot faster", 0},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Heal", "Restore 1 health point.", 0},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Speed", "Gain 1 speed points.", 0}
};

struct DynamicText {
    SDL_Texture* texture;
    SDL_Texture* shadow_texture;
    SDL_Rect rect;
    std::string current_text;

    DynamicText() : texture(nullptr), shadow_texture(nullptr), rect{0,0,0,0}, current_text("") {}

    void Update(SDL_Renderer* renderer, TTF_Font* font, const std::string& new_text, SDL_Color color, SDL_Color shadow_color) {
        if (new_text != current_text) {
            current_text = new_text;

            if (texture) {
                SDL_DestroyTexture(texture); // Limpa a textura antiga
            }
            if (shadow_texture) {
                SDL_DestroyTexture(shadow_texture);
            }

            SDL_Surface* surface = TTF_RenderText_Solid(font, new_text.c_str(), color);
            texture = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Surface* shadow_surface = TTF_RenderText_Solid(font, new_text.c_str(), shadow_color);
            shadow_texture = SDL_CreateTextureFromSurface(renderer, shadow_surface);

            rect.w = surface->w;
            rect.h = surface->h;

            SDL_FreeSurface(surface);
            SDL_FreeSurface(shadow_surface);

        }
    }

    void Render(SDL_Renderer* renderer, int x, int y, bool shadow) {

        if (shadow && shadow_texture) {
            SDL_Rect shadow_rect = { x + 2, y + 2, rect.w, rect.h };
            SDL_RenderCopy(renderer, shadow_texture, nullptr, &shadow_rect);
        }
        
        rect.x = x;
        rect.y = y;
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    ~DynamicText() {
        if (texture) SDL_DestroyTexture(texture);
    }
};

SDL_Window* g_window = nullptr;
SDL_Renderer* g_renderer = nullptr;

Projectile projectiles[MAX_PROJECTILES];
Uint32 last_projectile_time = 0;

Enemy enemies[MAX_ENEMIES];
bool resolved_collision[MAX_ENEMIES][MAX_ENEMIES] = { false };
Uint32 last_enemy_time = 0;

DynamicText life_text;
DynamicText level_text;
DynamicText title_text;
DynamicText pause_text;
DynamicText kill_count_text;
DynamicText stopwatch_text;


bool Init() 
{
    // Inicializa o SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Erro ao init SDL: %s", SDL_GetError());
        return false;
    }
    return true;
}

void InitWindow(int screen_width, int screen_height) 
{
    // Cria a janela
    g_window = SDL_CreateWindow("Hello SDL World",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screen_width, screen_height, SDL_WINDOW_SHOWN);
    if (!g_window) {
        SDL_Log("Error ao creating window: %s", SDL_GetError());
        SDL_Quit();
    }
}

void InitRenderer() 
{
    // Cria o renderizador
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_renderer) {
        SDL_Log("Error creating render: %s", SDL_GetError());
        SDL_DestroyWindow(g_window);
        SDL_Quit();
    }

    if (SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND) != 0) {
        SDL_Log("Erro ao configurar modo de blending: %s", SDL_GetError());
    }
}

void InitSDLMusic()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("Erro ao inicializar o SDL: %s", SDL_GetError());
        SDL_Quit();
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Erro ao inicializar SDL_mixer : %s", Mix_GetError());
        SDL_Quit();
    }
}

Mix_Chunk* InitSoundEffect(const char* sound_path)
{
    Mix_Chunk* sound = Mix_LoadWAV(sound_path);
    if (!sound) {
        SDL_Log("Erro ao carregar som: %s", Mix_GetError());
        return nullptr;
    }

    Mix_VolumeChunk(sound, MIX_MAX_VOLUME / 30);
    return sound;
}

Mix_Music* InitMusic(const char* music_path, int loops)
{
    Mix_Music* music = Mix_LoadMUS(music_path);
    if (!music) {
        SDL_Log("Erro ao carregar música: %s", Mix_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return nullptr;
    }
    Mix_VolumeMusic(MIX_MAX_VOLUME / 30);
    Mix_PlayMusic(music, loops);

    return music;
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

SDL_Surface* CreateTextSurface(TTF_Font* font, const char* text, int r, int g, int b) 
{
    SDL_Color color = { r, g, b };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    return surface;
}

SDL_Texture* UpdateTextTexture(TTF_Font* font, SDL_Rect& dest_rect_life_text, std::string text) 
{
    SDL_Surface* life_text_surface = CreateTextSurface(font, text.c_str(), 255, 255, 255);
    SDL_Texture* life_text_texture = SDL_CreateTextureFromSurface(g_renderer, life_text_surface);
    dest_rect_life_text = { 100, 100, life_text_surface->w, life_text_surface->h };
    SDL_FreeSurface(life_text_surface);
    return life_text_texture;
}

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
    //SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 255); // Vermelho
    //SDL_RenderDrawRect(g_renderer, &adjusted_a);
    //SDL_RenderDrawRect(g_renderer, &adjusted_b);

    return collision;
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

void UpdateEnemyPosition(SDL_Rect* enemy_rect_dst, SDL_Rect player_rect,int enemy_speed) {
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

void UpdateAnimation(characterState current_state, SDL_Rect& src_rect, int &frame, Uint32 &last_frame_time, int width, int height, 
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
    float a_center_x = a.x + a.w / 2;
    float a_center_y = a.y + a.h / 2;
    float b_center_x = b.x + b.w / 2;
    float b_center_y = b.y + b.h / 2;

    float diff_x = static_cast<float>(a_center_x - b_center_x);
    float diff_y = static_cast<float>(a_center_y - b_center_y);

    float magnitude = sqrt(diff_x * diff_x + diff_y * diff_y);

    return magnitude;
}

void CalculateDirection(SDL_Rect a, SDL_Rect b, Projectile* projectile)
{
    float a_center_x = a.x + a.w / 2;
    float a_center_y = a.y + a.h / 2;
    float b_center_x = b.x + b.w / 2;
    float b_center_y = b.y + b.h / 2;

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
    if ( current_time > last_projectile_time + projectile_delay ) {
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
                projectiles[i].rect_dst = { player_rect.x + player_rect.w / 2, player_rect.y + player_rect.h / 2, 25, 25};
                if(closest_enemy) CalculateDirection(player_rect, closest_enemy->rect_dst, &projectiles[i]);
                if(closest_enemy) projectiles[i].is_active = true;
                projectiles[i].last_frame_time = 0;
                last_projectile_time = current_time;
                break;
            }
        }
    }
}

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

void UpdateProjectiles(int width_limit, int height_limit)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].rect_dst.x += projectiles[i].dir_x * PROJECTILE_SPEED;
        projectiles[i].rect_dst.y += projectiles[i].dir_y * PROJECTILE_SPEED;
        projectiles[i].UpdateHitbox();
        if (projectiles[i].rect_dst.x < 0 || projectiles[i].rect_dst.x > width_limit ||
            projectiles[i].rect_dst.y < 0 || projectiles[i].rect_dst.y > height_limit) {
            projectiles[i].is_active = false;
        }
    }
}

void LevelUp(Character& character, int& current_game_state, TTF_Font* font) {
    character.exp = 0;
    character.level_to_update++;
    character.level++;
    level_text.Update(g_renderer, font, "Level: " + std::to_string(character.level), { 255, 255, 255 }, { 0, 0, 0 });
}

void CheckProjectileCollisionWithEnemy(Character &character, SDL_Rect enemy_rect, int &enemy_life, bool &active, SDL_Rect camera, int &kill_count, 
                                        TTF_Font* font, int &current_game_state)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].is_active && CheckCollision(projectiles[i].hitbox, enemy_rect, camera)) {
            projectiles[i].deactivate();
            enemy_life--;
            if (enemy_life <= 0) {
                kill_count++;
                kill_count_text.Update(g_renderer, font, "Enemies killed: " + std::to_string(kill_count), { 255, 255, 255 }, { 0, 0, 0 });
                character.exp++;
                if (character.exp >= MAX_EXP) {
                    LevelUp(character, current_game_state, font);
                }
                active = 0;
            }
        }
    }
}

void SpawnEnemies(int bg_width, int bg_height, SDL_Texture* enemy_texture) 
{
    Uint32 current_time = SDL_GetTicks();
    if(current_time > last_enemy_time + ENEMY_DELAY) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].is_active) {
                enemies[i] = Enemy{ 6, 1, 0, 0,
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

void resolveCollision(SDL_Rect* a, SDL_Rect* b) 
{
    float dx = (a->x + a->w / 2) - (b->x + b->w / 2);
    float dy = (a->y + a->h / 2) - (b->y + b->h / 2);

    float half_sum_x = (a->w / 2) + (b->w / 2);
    float half_sum_y = (a->h / 2) + (b->h / 2);

    float overlap_x = half_sum_x - fabs(dx);
    float overlap_y = half_sum_y - fabs(dy);

    if (overlap_x > 0 && overlap_y > 0) {
    
        if (overlap_x < overlap_y) {
            // Resolve along the X-axis
            if (dx > 0) {
                b->x -= overlap_x/5;
            }
            else {
                b->x += overlap_x/5;
            }
        }
        else {
            // Resolve along the Y-axis
            if (dy > 0) {
                b->y -= overlap_y/5;
            }
            else {
                b->y += overlap_y/5;
            }
        }
    }
};

std::string TimeFormatted(int time_in_seconds){
    int minutes = time_in_seconds / 60;
    int seconds = time_in_seconds % 60;

    std::string time_formatted =
        (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
        (seconds < 10 ? "0" : "") + std::to_string(seconds);

    return time_formatted;
}

void NewWave(int &current_game_state, int &wave) {
    wave++;
    current_game_state = CARD_SELECTOR;
}

void UpdateRenderStopwatchWave(int &start_time, int &time_left ,int screen_width, int &elapsed_time, int &wave, int &current_game_state, TTF_Font* font, int total_pause_duration)
{
    int wave_time_legth = 30 + ((wave - 1) * 5);
    elapsed_time = (SDL_GetTicks() - start_time - total_pause_duration) / 1000;
    if (current_game_state == PLAYING) time_left = wave_time_legth - elapsed_time;
    else time_left = wave_time_legth;

    std::string time_formatted = TimeFormatted(time_left);
    stopwatch_text.Update(g_renderer, font, "Wave " + std::to_string(wave), {255, 255, 255}, {0, 0, 0});
    stopwatch_text.Render(g_renderer, screen_width / 2 - stopwatch_text.rect.w / 2, 20, true);
    stopwatch_text.Update(g_renderer, font, time_formatted, { 255, 255, 255 }, { 0, 0, 0 });
    stopwatch_text.Render(g_renderer, screen_width / 2 - stopwatch_text.rect.w / 2, 50, true);
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

void ResetGame(int &kill_count, int &wave, Character* character, int bg_width, int bg_height, int &start_time, int &elapsed_time, TTF_Font* font, int &total_pause_duration)
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
    stopwatch_text.Update(g_renderer, font, std::to_string(elapsed_time), { 255, 255, 255 }, { 0, 0, 0 });
    kill_count_text.Update(g_renderer, font, "Enemies killed: " + std::to_string(kill_count), { 255, 255, 255 }, { 0, 0, 0 });
    life_text.Update(g_renderer, font, "Lifes: " + std::to_string(character->life), { 255, 255, 255 }, { 0, 0, 0 });
    level_text.Update(g_renderer, font, "Level: " + std::to_string(character->level), { 255, 255, 255 }, { 0, 0, 0 });
    for (int i = 0; i < cards.size(); ++i) {
        cards[i].level = 0;
    }
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
                                    card.rect_dst.x + ( card.rect_dst.w / 2 ) - card_description_text.rect.w / 2,
                                    card.rect_dst.y + ( card.rect_dst.h / 2 ), true);
}

void RenderCardSelection(int card_selected, TTF_Font* small_font, int screen_width, int screen_height, int level_to_update)
{
    SDL_Color text_color = { 255, 255, 255 };
    SDL_Color shadow_color = { 0, 0, 0 };
    DynamicText point_to_add_text;
    point_to_add_text.Update(g_renderer, small_font, "Points: " + std::to_string(level_to_update), text_color, shadow_color);
    point_to_add_text.Render(g_renderer,
        screen_width/2 - point_to_add_text.rect.w/2,
        screen_height / 2 - (CARD_WIDTH / 2), true);

    for (int i = 0; i < MAX_CARDS; i++) {
        int spacing = CARD_WIDTH / 4;
        int total_width = MAX_CARDS * (CARD_WIDTH / 2);
        int start_x = (screen_height / 2) - (total_width / 8);

        cards[i].rect_src = { 0, 0, CARD_WIDTH, CARD_HEIGHT };
        cards[i].rect_dst = { start_x + i * (CARD_WIDTH / 2 + spacing),
                                (screen_height / 2) - (CARD_HEIGHT / 4),
                                CARD_WIDTH / 2,
                                CARD_HEIGHT / 2 };
        cards[i].texture = nullptr;

        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 100);
        SDL_RenderFillRect(g_renderer, &cards[i].rect_dst);
        RenderCardInfo(cards[i], small_font, screen_width, screen_height);
        if (i == card_selected) {
            SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 100);
            DrawThickRect(g_renderer, &cards[i].rect_dst, 5);
        }
    }
}

void SelectCard(std::string card_name, Character &character, TTF_Font* font) 
{
    if (card_name == "Fire Rate") character.projectile_delay = character.projectile_delay * 0.90;
    if (card_name == "Heal") { character.life++; life_text.Update(g_renderer, font, "Lifes: " + std::to_string(character.life), { 255, 255, 255 }, { 0, 0, 0 });}
    if (card_name == "Speed") character.speed = character.speed + 1;
}

void DamageColor(SDL_Texture *texture, int last_damage_time, bool &took_damage) 
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

void MoveCharacter(Character *character, const Uint8* keyState, int bg_width, int bg_height)
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

    character->rect_dst.x += dx;
    character->rect_dst.y += dy;

    if (dx != 0 || dy != 0) {
        character->current_state = WALKING;
        character->UpdateHitbox();
    }
}

void RenderHeader(int &start_time, int &time_left, int screen_width, int &elapsed_time, int &wave, int &current_game_state, TTF_Font* small_font, Character character, int total_pause_duration) {
    UpdateRenderStopwatchWave(start_time, time_left, screen_width, elapsed_time, wave, current_game_state, small_font, total_pause_duration);
    life_text.Render(g_renderer, 50, 20, true);
    kill_count_text.Render(g_renderer, screen_width / 1.3 - (kill_count_text.rect.w), 20, true);
    level_text.Render(g_renderer, screen_width - 30 - (level_text.rect.w), 20, true);
    RenderExpBar(screen_width, character.exp);
}

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

    InitWindow(screen_width, screen_height);
    InitRenderer();

    InitSDLMusic();
    Mix_Music* music = InitMusic("Assets/music.mp3", -1);
    Mix_Chunk* damage_sound = InitSoundEffect("Assets/character_damage.wav");

    SDL_Rect camera = { 0, 0, screen_width, screen_height };

    //Background
    SDL_Texture* bg_texture = CreateTextureImg("Assets/background.png");
    int bg_width, bg_height;
    SDL_QueryTexture(bg_texture, NULL, NULL, &bg_width, &bg_height);

    Character character{7, 3,
        { 0, 0, CHARACTER_WIDTH_ORIG, CHARACTER_HEIGHT_ORIG }, //rect_src
        { bg_width / 2, bg_height / 2, CHARACTER_WIDTH_RENDER, CHARACTER_HEIGHT_RENDER }, //rect_dst
        CreateTextureImg("Assets/batata_spritesheet.png"), //texture
        IDLE, PROJECTILE_DELAY};

    SDL_Texture* projectile_texture = CreateTextureImg("Assets/mage-bullet-13x13.png");
    SDL_Texture* enemy_texture = CreateTextureImg("Assets/mage_spritesheet_85x94.png");
    SDL_Texture* card_texture = CreateTextureImg("Assets/generic_card.png");
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
    bool key_pressed = false, skip = false;


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
                ResetGame(kill_count, wave, &character, bg_width, bg_height, start_time, elapsed_time, small_font, total_pause_duration);
                current_game_state = PLAYING;
            }
            break;

        case PLAYING: {

            if (keyState[SDL_SCANCODE_ESCAPE]) current_game_state = PAUSE;

            MoveCharacter(&character, keyState, bg_width, bg_height);
            UpdateAnimation(character.current_state, character.rect_src, character.frame, character.last_frame_time, CHARACTER_WIDTH_ORIG, CHARACTER_HEIGHT_ORIG, WALK_FRAME_COUNT, IDLE_FRAME_COUNT);
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].is_active) UpdateAnimation(WALKING, enemies[i].rect_src, enemies[i].frame, enemies[i].last_frame_time, ENEMY_MAGE_WIDTH_ORIG, ENEMY_MAGE_HEIGHT_ORIG, 8, 8);
            }
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].is_active) UpdateAnimation(WALKING, projectiles[i].rect_src, projectiles[i].frame, projectiles[i].last_frame_time, PROJECTILE_WIDTH_ORIG, PROJECTILE_HEIGTH_ORIG, 5, 5);
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
            SDL_RenderCopy(g_renderer, character.texture, &character.rect_src, &player_render_rect);

            SpawnEnemies(bg_width, bg_height, enemy_texture);
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].is_active) {
                    UpdateEnemyPosition(&enemies[i].rect_dst, character.rect_dst, enemies[i].speed);
                    SDL_Rect enemy_render_rect = {
                        enemies[i].rect_dst.x - camera.x,
                        enemies[i].rect_dst.y - camera.y,
                        enemies[i].rect_dst.w + 10,
                        enemies[i].rect_dst.h + 10
                    };
                    enemies[i].UpdateHitbox();
                    SDL_RenderCopy(g_renderer, enemies[i].texture, &enemies[i].rect_src, &enemy_render_rect);

                    if (CheckCollision(character.hitbox, enemies[i].hitbox, camera)) {
                        Uint32 current_time = SDL_GetTicks();
                        if (current_time > character.last_damage_time + DAMAGE_COOLDOWN) {
                            character.life -= 1;
                            character.took_damage = true;
                            life_text.Update(g_renderer, small_font, "Lifes: " + std::to_string(character.life), { 255, 255, 255 }, { 0, 0, 0 });
                            character.last_damage_time = current_time;
                            Mix_PlayChannel(-1, damage_sound, 0);
                            if (character.life <= 0) {
                                current_game_state = GAME_OVER;
                            }
                        }
                    }

                    CheckProjectileCollisionWithEnemy(character, enemies[i].hitbox, enemies[i].life, enemies[i].is_active, camera, kill_count, small_font, current_game_state);
                    for (int j = i + 1; j < MAX_ENEMIES; j++) {
                        if (!enemies[j].is_active || resolved_collision[i][j]) continue;
                        resolveCollision(&enemies[i].rect_dst, &enemies[j].rect_dst);
                        resolved_collision[i][j] = true;
                        resolved_collision[j][i] = true;
                    }
                }
            }

            RenderHeader(start_time, time_left, screen_width, elapsed_time, wave, current_game_state, small_font, character, total_pause_duration);
            if (time_left <= 0) NewWave(current_game_state, wave);         

            memset(resolved_collision, 0, sizeof(resolved_collision));

            FireProjectile(character.rect_dst, projectile_texture, character.projectile_delay);
            UpdateProjectiles(bg_width, bg_height);
            RenderProjectiles(camera);
            break;
        }
        case CARD_SELECTOR:
            if (character.level_to_update == 0 || skip) { 
                current_game_state = PLAYING;
                total_pause_duration = 0;
                character.rect_dst = { bg_width / 2, bg_height / 2, CHARACTER_WIDTH_RENDER, CHARACTER_HEIGHT_RENDER };
                start_time = SDL_GetTicks();

                for (int i = 0; i < MAX_ENEMIES; i++) {
                    enemies[i].deactivate();
                }
                for (int i = 0; i < MAX_PROJECTILES; i++) {
                    projectiles[i].deactivate();
                }
            }else{
                SDL_RenderCopy(g_renderer, bg_texture, &camera, nullptr);
                RenderHeader(start_time, time_left, screen_width, elapsed_time, wave, current_game_state, small_font, character, total_pause_duration);
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

                        //check if all cards are max level
                        skip = true;
                        for (int i = 0; i < cards.size(); i++) {
                            if (cards[i].level < 5) {
                                skip = false;
                                break;
                            }
                        }
                        if (skip) current_game_state = PLAYING;

                        if (cards[card_selected].level < 5) {
                            SelectCard(cards[card_selected].name, character, small_font);
                            cards[card_selected].level++;
                            character.level_to_update--;
                        } 
                        break;

                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_LEFT:
                        card_selected--;
                        if (card_selected < 0) card_selected = MAX_CARDS - 1;
                        break;

                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
                        card_selected++;
                        if (card_selected >= MAX_CARDS) card_selected = 0;
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
        case GAME_OVER: {


            int kill_count_save_file, elapsed_time_save_file;
            LoadGame("sdl.dat", kill_count_save_file, elapsed_time_save_file);

            SDL_Rect dark_rect_dst = { 0, 0, screen_width, screen_height };
            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 2);
            SDL_RenderFillRect(g_renderer, &dark_rect_dst);

            title_text.Update(g_renderer, font, "You died", { 238, 173, 45 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 1.5 - title_text.rect.h / 2, true);
            title_text.Update(g_renderer, small_font, "Press Enter to restart", { 255, 255, 255 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 1.5 - title_text.rect.h / 2 + 90, true);
            title_text.Update(g_renderer, small_font, "Press ESC to go to the title screen.", { 255, 255, 255 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 1.5 - title_text.rect.h / 2 + 120, true);

            if (kill_count > kill_count_save_file || elapsed_time > elapsed_time_save_file) {
                title_text.Update(g_renderer, small_font, "Congratulations! You've set a new record!", { 255, 50, 50 }, { 0, 0, 0 });
                title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 1.5 - title_text.rect.h / 2 + 150, true);
            }

            SaveGame(SAVE_FILE, kill_count, wave);

            if (keyState[SDL_SCANCODE_RETURN] || keyState[SDL_SCANCODE_ESCAPE]) {
                ResetGame(kill_count, wave, &character, bg_width, bg_height, start_time, elapsed_time, small_font, total_pause_duration);
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
                last_projectile_time = SDL_GetTicks();
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