#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

#define CHARACTER_WIDTH_ORIG    32 //Size of character width in sprite
#define CHARACTER_HEIGHT_ORIG   32 //Size of character height in sprite
#define CHARACTER_WIDTH_RENDER  64 //Size of character width in render
#define CHARACTER_HEIGHT_RENDER 64 //Size of character height in render
#define ANIMATION_SPEED         160
#define WALK_FRAME_COUNT        4
#define IDLE_FRAME_COUNT        2

enum batataState {IDLE, WALKING};

SDL_Window* window      = nullptr;
SDL_Renderer* renderer  = nullptr;

bool init() {
    // Inicializa o SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erro ao inicializar SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void init_window(int screenWidth, int screenHeight) {

    // Cria a janela
    window = SDL_CreateWindow("Hello SDL World",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Erro ao criar janela: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }
}

void init_renderer() {    // Cria o renderizador
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Erro ao criar renderizador: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}

SDL_Texture* create_texture(const char* image_path) {

    SDL_Surface* imageSurface = IMG_Load(image_path);
    if (!imageSurface) {
        std::cerr << "Erro ao carregar imagem: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }

    // Cria a textura a partir da superfície da imagem
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);  // Liberar a superfície da memória

    if (!texture) {
        std::cerr << "Erro ao criar textura: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }

    return texture;
}

void update_camera(int playerX, int playerY, SDL_Rect* camera, SDL_Rect batata_rect_dst, int bg_width, int bg_height, int screenWidth, int screenHeight) {
    // Centralizar a câmera no jogador
    camera->x = playerX + batata_rect_dst.w / 2 - screenWidth / 2;
    camera->y = playerY + batata_rect_dst.h / 2 - screenHeight / 2;

    // Limitar a câmera para não sair dos limites do mundo
    if (camera->x < 0) camera->x = 0;
    if (camera->y < 0) camera->y = 0;
    if (camera->x > bg_width - camera->w) camera->x = bg_width - camera->w;
    if (camera->y > bg_height - camera->h) camera->y = bg_height - camera->h;
}

void update_animation(batataState currentState, SDL_Rect& batata_scr_rect, int &frame, Uint32 &lastFrameTime) {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime > lastFrameTime + ANIMATION_SPEED) {
        if (currentState == WALKING) {
            frame = (frame + 1) % WALK_FRAME_COUNT;
            batata_scr_rect.y = 0;
        }
        else if (currentState == IDLE) {
            frame = (frame + 1) % IDLE_FRAME_COUNT;
            batata_scr_rect.y = CHARACTER_HEIGHT_ORIG;
        }
        batata_scr_rect.x = frame * CHARACTER_WIDTH_ORIG;
        lastFrameTime = currentTime;
    }
};

int main(int argc, char* argv[]) 
{

    if (!init()) {
        std::cerr << "Erro ao inicializar SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    //Get diplay size, maybe make this a func?
    SDL_DisplayMode displayMode;
    int screenWidth = 0, screenHeight = 0;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
        screenWidth = displayMode.w;
        screenHeight = displayMode.h;
    }
    else std::cerr << "Erro ao obter a resolução do monitor: " << SDL_GetError() << std::endl;

    init_window(screenWidth, screenHeight);
    init_renderer();

    SDL_Rect camera = { 0, 0, screenWidth, screenHeight };

    //Background
    SDL_Texture* bg_texture = create_texture("Assets/background.png");
    int bg_width, bg_height;
    SDL_QueryTexture(bg_texture, NULL, NULL, &bg_width, &bg_height);


    //Character
    int speed = 7;
    batataState currentBatataState = IDLE;
    SDL_Rect batata_rect_src = { 0, 0, CHARACTER_WIDTH_ORIG, CHARACTER_HEIGHT_ORIG };
    SDL_Rect batata_rect_dst = { bg_width / 2, bg_height / 2, CHARACTER_WIDTH_RENDER, CHARACTER_HEIGHT_RENDER };
    SDL_Texture* batataTexture = create_texture("Assets/batata_spritesheet.png");

    //Frame info
    int frame = 0;
    Uint32 lastFrameTime = 0;

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
        currentBatataState = IDLE;
        if (keyState[SDL_SCANCODE_UP] && batata_rect_dst.y > 0){
            batata_rect_dst.y -= speed;
            currentBatataState = WALKING;
        }

        if (keyState[SDL_SCANCODE_DOWN] && batata_rect_dst.y < bg_height - CHARACTER_HEIGHT_RENDER) {
            batata_rect_dst.y += speed;
            currentBatataState = WALKING;
        }

        if (keyState[SDL_SCANCODE_LEFT] && batata_rect_dst.x > 0) {
            batata_rect_dst.x -= speed;
            currentBatataState = WALKING;
        }

        if (keyState[SDL_SCANCODE_RIGHT] && batata_rect_dst.x < bg_width - CHARACTER_WIDTH_RENDER) {
            batata_rect_dst.x += speed;
            currentBatataState = WALKING;
        }



        update_animation(currentBatataState, batata_rect_src, frame, lastFrameTime);
        update_camera(batata_rect_dst.x, batata_rect_dst.y, &camera, batata_rect_dst, bg_width, bg_height, screenWidth, screenHeight);



        // Define a cor de fundo (preto)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Desenha um quadrado vermelho no centro da janela
        //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        //SDL_RenderFillRect(renderer, &rect);
        //render_texture(renderer, bg_texture, NULL, NULL);
        SDL_Rect bg_render_rect = { 0, 0, bg_width, bg_height };
        SDL_RenderCopy(renderer, bg_texture, &camera, nullptr);


        SDL_Rect player_render_rect = {
             batata_rect_dst.x - camera.x,
             batata_rect_dst.y - camera.y,
             batata_rect_dst.w, batata_rect_dst.h
        };
        SDL_RenderCopy(renderer, batataTexture, &batata_rect_src, &player_render_rect);


        // Apresenta o conteúdo renderizado
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Limpeza
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}