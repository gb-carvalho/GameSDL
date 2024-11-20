#include <SDL.h>
#include <string>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

#define CHARACTER_WIDTH_ORIG    32 //Size of character width in sprite
#define CHARACTER_HEIGHT_ORIG   32 //Size of character height in sprite
#define CHARACTER_WIDTH_RENDER  64 //Size of character width in render
#define CHARACTER_HEIGHT_RENDER 64 //Size of character height in render

#define ENEMY_MAGE_WIDTH_ORIG  85
#define ENEMY_MAGE_HEIGHT_ORIG 94

#define PROJECTILE_WIDTH_ORIG  13
#define PROJECTILE_HEIGTH_ORIG 13

#define ANIMATION_SPEED  160
#define WALK_FRAME_COUNT 4
#define IDLE_FRAME_COUNT 2

#define PROJECTILE_DELAY 1000
#define PROJECTILE_SPEED 10
#define MAX_PROJECTILES  100

struct Projectile
{
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
    SDL_Texture* texture;
    float dir_x, dir_y;
    bool active;
};

Projectile projectiles[MAX_PROJECTILES];
Uint32 last_projectile_time = 0;

enum batataState { IDLE, WALKING };

SDL_Window* g_window = nullptr;
SDL_Renderer* g_renderer = nullptr;

class Entity {
public:
    int speed;
    int life;
    SDL_Rect rect_src;
    SDL_Rect rect_dst;
    SDL_Texture* texture;

    Entity(int spd, int lfe, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex)
        : speed(spd), life(lfe), rect_src(src), rect_dst(dst), texture(tex) {}

    virtual ~Entity() {
        SDL_DestroyTexture(texture);
    }

    //void render(SDL_Renderer* renderer) {
    //    SDL_RenderCopy(renderer, texture, &rect_src, &rect_dst);
    //}
};

class Character : public Entity {
public:
    batataState current_state;

    Character(int spd, int lfe, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, batataState state)
        : Entity(spd, lfe, src, dst, tex), current_state(state) {}

    void updateState(batataState newState) {
        current_state = newState;
    }
};

// Classe para inimigos (herda de Entity)
class Enemy : public Entity {
public:
    bool is_active;

    Enemy(int spd, int lfe, SDL_Rect src, SDL_Rect dst, SDL_Texture* tex, bool active)
        : Entity(spd, lfe, src, dst, tex), is_active(active) {}

    void deactivate() {
        is_active = false;
    }
};

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

bool CheckCollision(SDL_Rect a, SDL_Rect b) 
{
    return (a.x + a.w >= b.x &&
            b.x + b.w >= a.x &&
            a.y + a.h >= b.y &&
            b.y + b.h >= a.y
        );
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

void UpdateLife(int& life, SDL_Texture*& life_text_texture, TTF_Font* font, SDL_Rect& dest_text_life_text)
{
    // Atualiza o texto de vida
    std::string life_text_string = "Lifes: " + std::to_string(life);
    life_text_texture = UpdateTextTexture(font, dest_text_life_text, life_text_string);

    // Verifica se a vida acabou
    if (life == 0) {
        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        TTF_Quit();
        SDL_Quit();
        exit(0); // Encerra o programa
    }
}

void FireProjectile(SDL_Rect player_rect, SDL_Rect enemy_rect)
{
    Uint32 current_time = SDL_GetTicks();
    if ( current_time > last_projectile_time + PROJECTILE_DELAY) {
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (!projectiles[i].active) {
                projectiles[i].texture = CreateTextureImg("Assets/mage-bullet-13x13.png");
                //SDL_SetTextureColorMod(projectiles[i].texture, 255, 0, 0);
                projectiles[i].src_rect = { 0, 0, PROJECTILE_WIDTH_ORIG, PROJECTILE_HEIGTH_ORIG };
                projectiles[i].dst_rect = { player_rect.x + player_rect.w / 2, player_rect.y + player_rect.h / 2, 25, 25};
                float diff_x = static_cast<float>(enemy_rect.x - player_rect.x);
                float diff_y = static_cast<float>(enemy_rect.y - player_rect.y);
                float magnitude = sqrt(diff_x * diff_x + diff_y * diff_y);
                projectiles[i].dir_x = diff_x / magnitude;
                projectiles[i].dir_y = diff_y / magnitude;
                projectiles[i].active = true;
                last_projectile_time = current_time;
                break;
            }
        }
    }
}

void RenderProjectiles(SDL_Rect camera)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            SDL_Rect render_rect = {
                projectiles[i].dst_rect.x - camera.x, // Ajusta a posição X com base na câmera
                projectiles[i].dst_rect.y - camera.y, // Ajusta a posição Y com base na câmera
                projectiles[i].dst_rect.w,
                projectiles[i].dst_rect.h
            };
            SDL_RenderCopy(g_renderer, projectiles[i].texture, &projectiles[i].src_rect, &render_rect);
        }
    }
}

void UpdateProjectiles(int width_limit, int height_limit)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].dst_rect.x += projectiles[i].dir_x * PROJECTILE_SPEED;
        projectiles[i].dst_rect.y += projectiles[i].dir_y * PROJECTILE_SPEED;
        if (projectiles[i].dst_rect.x < 0 || projectiles[i].dst_rect.x > width_limit ||
            projectiles[i].dst_rect.y < 0 || projectiles[i].dst_rect.y > height_limit) {
            projectiles[i].active = false;
        }
    }
}

void CheckProjectileCollisionWithEnemy(SDL_Rect enemy_rect, int &enemy_life, bool &active)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active && CheckCollision(projectiles[i].dst_rect, enemy_rect)) {
            projectiles[i].active = false;
            enemy_life--;
        }

        if (enemy_life <= 0) {
            active = 0;
        }
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

    SDL_Rect camera = { 0, 0, screen_width, screen_height };

    //Background
    SDL_Texture* bg_texture = CreateTextureImg("Assets/background.png");
    int bg_width, bg_height;
    SDL_QueryTexture(bg_texture, NULL, NULL, &bg_width, &bg_height);


    Character batata{7, 3, 
        { 0, 0, CHARACTER_WIDTH_ORIG, CHARACTER_HEIGHT_ORIG }, //rect_src
        { bg_width / 2, bg_height / 2, CHARACTER_WIDTH_RENDER, CHARACTER_HEIGHT_RENDER }, //rect_dst
        CreateTextureImg("Assets/batata_spritesheet.png"), //texture
        IDLE };

    Enemy mage{5, 5,
        { 0, 0, ENEMY_MAGE_WIDTH_ORIG, ENEMY_MAGE_HEIGHT_ORIG },  //rect_src
        { bg_width / 2 - 300, bg_height / 2, ENEMY_MAGE_WIDTH_ORIG, ENEMY_MAGE_HEIGHT_ORIG }, //rect_dst
        CreateTextureImg("Assets/mage_spritesheet_85x94.png"), //texture
        1 };

    //Frame info
    //TODO: Melhora isso
    int frame = 0;
    Uint32 last_frame_time = 0;
    int frame_enemy = 0;
    Uint32 last_frame_time_enemy = 0;
    int frame_projectile = 0;
    Uint32 last_frame_time_projectile = 0;

    //Font
    TTF_Font* font = TTF_OpenFont("Assets/GeoSlab703 Md BT Medium.ttf",24);
    if (!font) {
        SDL_Log("Error loading font: %s", TTF_GetError());
        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        TTF_Quit();
        SDL_Quit();
    }

    SDL_Rect dest_tect_life_text;
    std::string life_text_string = "Lifes: " + std::to_string(batata.life);
    SDL_Texture* life_text_texture = UpdateTextTexture(font, dest_tect_life_text, life_text_string);

    Uint32 last_damage_time = 0;
    const Uint32 damage_cooldown = 1000;

    SDL_Event event;
    bool running = true;
    while (running) {

        while (SDL_PollEvent(&event)) {

            // Verificar se o jogo foi fechado
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        batata.current_state = IDLE;
        if ( (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP]) && batata.rect_dst.y > 0){
            batata.rect_dst.y -= batata.speed;
            batata.current_state = WALKING;
        }

        if ( (keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN]) && batata.rect_dst.y < bg_height - CHARACTER_HEIGHT_RENDER) {
            batata.rect_dst.y += batata.speed;
            batata.current_state = WALKING;
        }

        if ( (keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT]) && batata.rect_dst.x > 0) {
            batata.rect_dst.x -= batata.speed;
            batata.current_state = WALKING;
        }

        if ( (keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT]) && batata.rect_dst.x < bg_width - CHARACTER_WIDTH_RENDER) {
            batata.rect_dst.x += batata.speed;
            batata.current_state = WALKING;
        }

        UpdateAnimation(batata.current_state, batata.rect_src, frame, last_frame_time, CHARACTER_WIDTH_ORIG, CHARACTER_HEIGHT_ORIG, WALK_FRAME_COUNT, IDLE_FRAME_COUNT);
        UpdateAnimation(WALKING, mage.rect_src, frame_enemy, last_frame_time_enemy, ENEMY_MAGE_WIDTH_ORIG, ENEMY_MAGE_HEIGHT_ORIG, 8, 8);
        for(int i = 0; i < MAX_PROJECTILES; i++) {
            UpdateAnimation(WALKING, projectiles[i].src_rect, frame_projectile, last_frame_time_projectile, PROJECTILE_WIDTH_ORIG, PROJECTILE_HEIGTH_ORIG, 5, 5);
        }
        UpdateCamera(batata.rect_dst.x, batata.rect_dst.y, &camera, batata.rect_dst, bg_width, bg_height, screen_width, screen_height);

        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
        SDL_RenderClear(g_renderer);

        SDL_Rect bg_render_rect = { 0, 0, bg_width, bg_height };
        SDL_RenderCopy(g_renderer, bg_texture, &camera, nullptr);
        SDL_RenderCopy(g_renderer, life_text_texture, nullptr, &dest_tect_life_text);


        SDL_Rect player_render_rect = {
             batata.rect_dst.x - camera.x,
             batata.rect_dst.y - camera.y,
             batata.rect_dst.w, batata.rect_dst.h
        };
        SDL_RenderCopy(g_renderer, batata.texture, &batata.rect_src, &player_render_rect);

        if (mage.is_active) {
            UpdateEnemyPosition(&mage.rect_dst, batata.rect_dst, mage.speed);
            SDL_Rect enemy_render_rect = {
                mage.rect_dst.x - camera.x,
                mage.rect_dst.y - camera.y,
                mage.rect_dst.w + 10,
                mage.rect_dst.h + 10
            };

            SDL_RenderCopy(g_renderer, mage.texture, &mage.rect_src, &enemy_render_rect);

            if (CheckCollision(player_render_rect, enemy_render_rect)) {
                Uint32 current_time = SDL_GetTicks();
                if (current_time > last_damage_time + damage_cooldown) {
                    batata.life -= 1;
                    UpdateLife(batata.life, life_text_texture, font, dest_tect_life_text);
                    last_damage_time = current_time;
                }
            }
        }

        if (mage.is_active) {
            FireProjectile(batata.rect_dst, mage.rect_dst);
        }
        UpdateProjectiles(bg_width, bg_height);
        CheckProjectileCollisionWithEnemy(mage.rect_dst, mage.life, mage.is_active);
        RenderProjectiles(camera);

        // Apresenta o conteúdo renderizado
        SDL_RenderPresent(g_renderer);
        SDL_Delay(16);
    }

    // Limpeza
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}