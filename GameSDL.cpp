#include <SDL.h>
#include <string>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <fstream>
#include <vector>

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

#define PROJECTILE_DELAY 10000
#define PROJECTILE_SPEED 15
#define MAX_PROJECTILES  100

#define DAMAGE_COOLDOWN  500
#define ENEMY_DELAY      500
#define MAX_ENEMIES      20
#define MAX_EXP          10
#define MAX_CARDS        3

#define SAVE_FILE "sdl.dat"

enum batataState { IDLE, WALKING };
enum gameState { TITLE_SCREEN, PLAYING, CARD_SELECTOR, GAME_OVER };

class Entity {
public:
    int speed;
    int life;
    int frame;
    Uint32 last_frame_time;
    SDL_Rect rect_src;
    SDL_Rect rect_dst;
    SDL_Rect hitbox;
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
    batataState current_state;
    int exp, level, projectile_delay, last_damage_time;
    bool took_damage;

    Character(int spd, int lfe, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, batataState state, int prjctle_delay)
        : Entity(spd, lfe, 0, 0, src, dst, tex), current_state(state), exp(0), level(0), projectile_delay(prjctle_delay), last_damage_time(0), took_damage(false) {
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
        level = 0;
    }

    void updateState(batataState newState) {
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
    std::string name;
    std::string description;

    Card()
        : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), name(""), description("") {}

    Card(SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, std::string nme, std::string dscrption)
        : Entity(0, 0, 0, 0, src, dst, tex), name(nme), description(dscrption){
        //UpdateHitbox();
    }
};

std::vector<Card> cards = {
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, nullptr, "Fire Rate", "Shoot faster" },
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, nullptr, "Heal", "Restore 1 health point." },
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, nullptr, "Speed", "Gain 1 speed points." }
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
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
    if (!g_renderer) {
        SDL_Log("Error creating render: %s", SDL_GetError());
        SDL_DestroyWindow(g_window);
        SDL_Quit();
    }
}

Mix_Music* InitMusic()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("Erro ao inicializar o SDL: %s", SDL_GetError());
        return nullptr;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Erro ao inicializar SDL_mixer : %s", Mix_GetError());
        return nullptr;
    }

    Mix_Music* music = Mix_LoadMUS("Assets/music.mp3");
    if (!music) {
        SDL_Log("Erro ao carregar música: %s", Mix_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return nullptr;
    }
    Mix_VolumeMusic(MIX_MAX_VOLUME / 30);
    Mix_PlayMusic(music, -1);

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

void UpdateCamera(int playerX, int playerY, SDL_Rect* camera, SDL_Rect batata_rect_dst, int bg_width, int bg_height, int screen_width, int screen_height) 
{
    // Centralizar a câmera no jogador
    camera->x = playerX + batata_rect_dst.w / 2 - screen_width / 2;
    camera->y = playerY + batata_rect_dst.h / 2 - screen_height / 2;

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

    if (magnitude > 30) {
        // Normaliza a direção e move o inimigo
        enemy_rect_dst->x += static_cast<int>((diff_x / magnitude) * enemy_speed);
        enemy_rect_dst->y += static_cast<int>((diff_y / magnitude) * enemy_speed);
    }
};

void UpdateAnimation(batataState current_state, SDL_Rect& src_rect, int &frame, Uint32 &last_frame_time, int width, int height, 
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
    if ( current_time > last_projectile_time + projectile_delay) {
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
    character.level++;
    current_game_state = CARD_SELECTOR;
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
                enemies[i] = Enemy{ 7, 1, 0, 0,
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

void UpdateRenderStopwatch(int start_time, TTF_Font* font, int screen_width, int &elapsed_time) 
{
    elapsed_time = (SDL_GetTicks() - start_time) / 1000;

    std::string time_formatted = TimeFormatted(elapsed_time);

    stopwatch_text.Update(g_renderer, font, time_formatted, { 255, 255, 255 }, { 0, 0, 0 });
    stopwatch_text.Render(g_renderer, screen_width/1.3 - (stopwatch_text.rect.w), 20, true);
}

void LoadGame(const std::string& file_name, int& kill_count, int& elapsed_time) 
{
    std::ifstream save_file(file_name, std::ios::binary);
    if (!save_file.is_open()) {
        SDL_Log("Error opening save file\n");
        kill_count = 0;
        elapsed_time = 0;
        return;
    }

    save_file.read(reinterpret_cast<char*>(&kill_count), sizeof(kill_count));
    save_file.read(reinterpret_cast<char*>(&elapsed_time), sizeof(elapsed_time));

    save_file.close();   
}

void SaveGame(const std::string& file_name, int kill_count, int elapsed_time)
{
    int kill_count_save_file, elapsed_time_save_file;

    LoadGame(file_name, kill_count_save_file, elapsed_time_save_file);
  
    std::ofstream save_file(file_name, std::ios::binary);
    if (!save_file.is_open()) {
        SDL_Log("Error opening save file\n");
        return;
    }
    
    if (kill_count > kill_count_save_file) save_file.write(reinterpret_cast<const char*>(&kill_count), sizeof(kill_count));   
    else save_file.write(reinterpret_cast<const char*>(&kill_count_save_file), sizeof(kill_count_save_file));
    
    if (elapsed_time > elapsed_time_save_file) save_file.write(reinterpret_cast<const char*>(&elapsed_time), sizeof(elapsed_time));
    else save_file.write(reinterpret_cast<const char*>(&elapsed_time_save_file), sizeof(elapsed_time_save_file));

    save_file.close();
}

void ResetGame(int &kill_count, Character* batata, int bg_width, int bg_height, int &start_time, int &elapsed_time, TTF_Font* font) 
{  
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].deactivate();
    }

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].deactivate();
    }

    kill_count = 0;
    batata->reset({ bg_width / 2, bg_height / 2, CHARACTER_WIDTH_RENDER, CHARACTER_HEIGHT_RENDER });
    batata->UpdateHitbox();
    start_time = SDL_GetTicks();
    elapsed_time = (SDL_GetTicks() - start_time) / 1000;
    stopwatch_text.Update(g_renderer, font, std::to_string(elapsed_time), { 255, 255, 255 }, { 0, 0, 0 });
    kill_count_text.Update(g_renderer, font, "Enemies killed: " + std::to_string(kill_count), { 255, 255, 255 }, { 0, 0, 0 });
    life_text.Update(g_renderer, font, "Lifes: " + std::to_string(batata->life), { 255, 255, 255 }, { 0, 0, 0 });
    level_text.Update(g_renderer, font, "Level: " + std::to_string(batata->level), { 255, 255, 255 }, { 0, 0, 0 });
}

void RenderExpBar(int screen_width, int exp) 
{
    SDL_Rect exp_bar_rect = { 8, 8, screen_width - 20, 10 };
    SDL_SetRenderDrawColor(g_renderer, 230, 230, 230, 0);
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
    SDL_Color text_color = { 255, 255, 255};
    SDL_Color shadow_color = { 0, 0, 0 };
    // Renderizar o nome do card
    DynamicText card_name_text;
    card_name_text.Update(g_renderer, font, card.name, text_color, shadow_color);
    card_name_text.Render(g_renderer, 
                            card.rect_dst.x + ( card.rect_dst.w / 2 ) - card_name_text.rect.w / 2, 
                            card.rect_dst.y + ( card.rect_dst.h / 4 ), true);

    // Renderizar a descrição do card
    DynamicText card_description_text;
    card_description_text.Update(g_renderer, font, card.description, text_color, shadow_color);
    card_description_text.Render(g_renderer, 
                                    card.rect_dst.x + ( card.rect_dst.w / 2 ) - card_description_text.rect.w / 2,
                                    card.rect_dst.y + ( card.rect_dst.h / 2 ), true);
}

void RenderCardSelection(int card_selected, SDL_Texture* card_texture, TTF_Font* small_font, int screen_width, int screen_height) 
{
    for (int i = 0; i < MAX_CARDS; i++) {
        int spacing = CARD_WIDTH / 4;
        int total_width = MAX_CARDS * (CARD_WIDTH / 2);
        int start_x = (screen_height / 2) - (total_width / 8);

        cards[i].rect_src = { 0, 0, CARD_WIDTH, CARD_HEIGHT };
        cards[i].rect_dst = { start_x + i * (CARD_WIDTH / 2 + spacing),
                                (screen_height / 2) - (CARD_HEIGHT / 4),
                                CARD_WIDTH / 2,
                                CARD_HEIGHT / 2 };
        cards[i].texture = card_texture;
        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
        DrawThickRect(g_renderer, &cards[i].rect_dst, 5);
        SDL_RenderCopy(g_renderer, cards[i].texture, &cards[i].rect_src, &cards[i].rect_dst);
        RenderCardInfo(cards[i], small_font, screen_width, screen_height);
        if (i == card_selected) {
            SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 255);
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
    Mix_Music* music = InitMusic();

    SDL_Rect camera = { 0, 0, screen_width, screen_height };

    //Background
    SDL_Texture* bg_texture = CreateTextureImg("Assets/background.png");
    int bg_width, bg_height;
    SDL_QueryTexture(bg_texture, NULL, NULL, &bg_width, &bg_height);

    Character batata{7, 3,
        { 0, 0, CHARACTER_WIDTH_ORIG, CHARACTER_HEIGHT_ORIG }, //rect_src
        { bg_width / 2, bg_height / 2, CHARACTER_WIDTH_RENDER, CHARACTER_HEIGHT_RENDER }, //rect_dst
        CreateTextureImg("Assets/batata_spritesheet.png"), //texture
        IDLE, PROJECTILE_DELAY};

    SDL_Texture* projectile_texture = CreateTextureImg("Assets/mage-bullet-13x13.png");
    SDL_Texture* enemy_texture = CreateTextureImg("Assets/mage_spritesheet_85x94.png");
    SDL_Texture* card_texture = CreateTextureImg("Assets/generic_card.png");

    //Font
    TTF_Font* font = TTF_OpenFont("Assets/GeoSlab703 Md BT Medium.ttf",48);
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

    int card_selected = 1;
    SDL_Event event;
    bool running = true;
    int current_game_state = TITLE_SCREEN;
    int kill_count;
    int start_time;
    int elapsed_time;
    bool key_pressed = false;

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

            title_text.Update(g_renderer, font, "Batata Game", { 238, 173, 45 }, { 255, 255, 255 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 4 - title_text.rect.h / 2, false);
            title_text.Update(g_renderer, small_font, "Press Enter to start", { 255, 255, 255 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 2 - title_text.rect.h / 2, false);

            LoadGame(SAVE_FILE, kill_count, elapsed_time);
            title_text.Update(g_renderer, small_font, "Kill record: " + std::to_string(kill_count), { 255, 50, 50 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 2 + 80, true);
            title_text.Update(g_renderer, small_font, "Time record: " + TimeFormatted(elapsed_time), { 255, 50, 50 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 2 + 120, true);

            if (keyState[SDL_SCANCODE_RETURN]) {
                ResetGame(kill_count, &batata, bg_width, bg_height, start_time, elapsed_time, small_font);
                current_game_state = PLAYING;
            }


            break;
        case PLAYING: {

            batata.current_state = IDLE;
            if ((keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP]) && batata.rect_dst.y > 0) {
                batata.rect_dst.y -= batata.speed;
                batata.current_state = WALKING;
                batata.UpdateHitbox();
            }

            if ((keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN]) && batata.rect_dst.y < bg_height - CHARACTER_HEIGHT_RENDER) {
                batata.rect_dst.y += batata.speed;
                batata.current_state = WALKING;
                batata.UpdateHitbox();
            }

            if ((keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && batata.rect_dst.x > 0) {
                batata.rect_dst.x -= batata.speed;
                batata.current_state = WALKING;
                batata.UpdateHitbox();
            }

            if ((keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT]) && batata.rect_dst.x < bg_width - CHARACTER_WIDTH_RENDER) {
                batata.rect_dst.x += batata.speed;
                batata.current_state = WALKING;
                batata.UpdateHitbox();
            }

            UpdateAnimation(batata.current_state, batata.rect_src, batata.frame, batata.last_frame_time, CHARACTER_WIDTH_ORIG, CHARACTER_HEIGHT_ORIG, WALK_FRAME_COUNT, IDLE_FRAME_COUNT);
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].is_active) UpdateAnimation(WALKING, enemies[i].rect_src, enemies[i].frame, enemies[i].last_frame_time, ENEMY_MAGE_WIDTH_ORIG, ENEMY_MAGE_HEIGHT_ORIG, 8, 8);
            }
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].is_active) UpdateAnimation(WALKING, projectiles[i].rect_src, projectiles[i].frame, projectiles[i].last_frame_time, PROJECTILE_WIDTH_ORIG, PROJECTILE_HEIGTH_ORIG, 5, 5);
            }

            UpdateCamera(batata.rect_dst.x, batata.rect_dst.y, &camera, batata.rect_dst, bg_width, bg_height, screen_width, screen_height);

            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
            SDL_RenderClear(g_renderer);

            SDL_Rect bg_render_rect = { 0, 0, bg_width, bg_height };
            SDL_RenderCopy(g_renderer, bg_texture, &camera, nullptr);

            
            DamageColor(batata.texture, batata.last_damage_time, batata.took_damage);
            SDL_Rect player_render_rect = {
                 batata.rect_dst.x - camera.x,
                 batata.rect_dst.y - camera.y,
                 batata.rect_dst.w, batata.rect_dst.h
            };
            SDL_RenderCopy(g_renderer, batata.texture, &batata.rect_src, &player_render_rect);

            SpawnEnemies(bg_width, bg_height, enemy_texture);
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].is_active) {
                    UpdateEnemyPosition(&enemies[i].rect_dst, batata.rect_dst, enemies[i].speed);
                    SDL_Rect enemy_render_rect = {
                        enemies[i].rect_dst.x - camera.x,
                        enemies[i].rect_dst.y - camera.y,
                        enemies[i].rect_dst.w + 10,
                        enemies[i].rect_dst.h + 10
                    };
                    enemies[i].UpdateHitbox();
                    SDL_RenderCopy(g_renderer, enemies[i].texture, &enemies[i].rect_src, &enemy_render_rect);

                    if (CheckCollision(batata.hitbox, enemies[i].hitbox, camera)) {
                        Uint32 current_time = SDL_GetTicks();
                        if (current_time > batata.last_damage_time + DAMAGE_COOLDOWN) {
                            batata.life -= 1;
                            batata.took_damage = true;
                            life_text.Update(g_renderer, small_font, "Lifes: " + std::to_string(batata.life), { 255, 255, 255 }, { 0, 0, 0 });
                            batata.last_damage_time = current_time;

                            if (batata.life <= 0) {
                                current_game_state = GAME_OVER;
                            }
                        }
                    }

                    CheckProjectileCollisionWithEnemy(batata, enemies[i].hitbox, enemies[i].life, enemies[i].is_active, camera, kill_count, small_font, current_game_state);
                    for (int j = i + 1; j < MAX_ENEMIES; j++) {
                        if (!enemies[j].is_active || resolved_collision[i][j]) continue;
                        resolveCollision(&enemies[i].rect_dst, &enemies[j].rect_dst);
                        resolved_collision[i][j] = true;
                        resolved_collision[j][i] = true;
                    }
                }
            }

            UpdateRenderStopwatch(start_time, small_font, screen_width, elapsed_time);
            
            life_text.Render(g_renderer, 50, 20, true);
            kill_count_text.Render(g_renderer, screen_width / 2 - kill_count_text.rect.w / 2, 20, true);
            level_text.Render(g_renderer, screen_width - 30 - (level_text.rect.w), 20, true);
            RenderExpBar(screen_width, batata.exp);

            memset(resolved_collision, 0, sizeof(resolved_collision));

            FireProjectile(batata.rect_dst, projectile_texture, batata.projectile_delay);
            UpdateProjectiles(bg_width, bg_height);
            RenderProjectiles(camera);
            break;
        }
        case CARD_SELECTOR:
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                else if (event.type == SDL_KEYDOWN && !key_pressed) {
                    key_pressed = true; 

                    switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_RETURN: 
                        SelectCard(cards[card_selected].name, batata, small_font);
                        current_game_state = PLAYING;
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
            RenderCardSelection(card_selected, card_texture, small_font, screen_width, screen_height);

            break;
        case GAME_OVER:

            int kill_count_save_file, elapsed_time_save_file;
            LoadGame("sdl.dat", kill_count_save_file, elapsed_time_save_file);

            title_text.Update(g_renderer, font, "You died", { 238, 173, 45 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 1.5 - title_text.rect.h / 2, true);
            title_text.Update(g_renderer, small_font, "Press Enter to restart", { 255, 255, 255 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 1.5 - title_text.rect.h / 2 + 60, true);
            title_text.Update(g_renderer, small_font, "Press ESC to go to the title screen.", { 255, 255, 255 }, { 0, 0, 0 });
            title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 1.5 - title_text.rect.h / 2 + 90, true);

            if (kill_count > kill_count_save_file || elapsed_time > elapsed_time_save_file) {
                title_text.Update(g_renderer, small_font, "Congratulations! You've set a new record!", { 255, 50, 50 }, { 0, 0, 0 });
                title_text.Render(g_renderer, screen_width / 2 - title_text.rect.w / 2, screen_height / 1.5 - title_text.rect.h / 2 + 120, true);
            }

            SaveGame(SAVE_FILE,kill_count,elapsed_time);

            if (keyState[SDL_SCANCODE_RETURN] || keyState[SDL_SCANCODE_ESCAPE]) {
                ResetGame(kill_count, &batata, bg_width, bg_height, start_time, elapsed_time, small_font);
                if (keyState[SDL_SCANCODE_ESCAPE]) current_game_state = TITLE_SCREEN;
                else if (keyState[SDL_SCANCODE_RETURN]) current_game_state = PLAYING;
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