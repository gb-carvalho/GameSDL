#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

bool init() {
    // Inicializa o SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erro ao inicializar SDL: " << SDL_GetError() << std::endl;
        return false;
    }
}

SDL_Window* init_window() {

    // Cria a janela
    SDL_Window* window = SDL_CreateWindow("Hello SDL World",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Erro ao criar janela: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return nullptr;
    }
   
    return window;
}

SDL_Renderer* init_renderer(SDL_Window* window) {    // Cria o renderizador
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Erro ao criar renderizador: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }

    return renderer;
}

SDL_Texture* create_texture(SDL_Window* window, SDL_Renderer* renderer, const char* image_path) {

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

void update_camera(int playerX, int playerY, SDL_Rect* camera, SDL_Rect player_rect, int bg_width, int bg_height) {
    // Centralizar a câmera no jogador
    camera->x = playerX + player_rect.w / 2 - SCREEN_WIDTH / 2;
    camera->y = playerY + player_rect.h / 2 - SCREEN_HEIGHT / 2;

    // Limitar a câmera para não sair dos limites do mundo
    if (camera->x < 0) camera->x = 0;
    if (camera->y < 0) camera->y = 0;
    if (camera->x > bg_width - camera->w) camera->x = bg_width - camera->w;
    if (camera->y > bg_height - camera->h) camera->y = bg_height - camera->h;
}

int main(int argc, char* argv[]) 
{

    if (!init()) {
        std::cerr << "Erro ao inicializar SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* window = init_window(); 
    if (!window) return 1;

    SDL_Renderer* renderer = init_renderer(window);
    if (!renderer) return 1;

    
    bool running = true;

    //Character
    int velocidade = 5;
    SDL_Rect player_rect = { 0, 200, 64, 64 }; 
    SDL_Texture* char_texture = create_texture(window, renderer, "Assets/batata.png");


    SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };



    //Background
    SDL_Texture* bg_texture = create_texture(window, renderer, "Assets/background.png");
    int bg_width, bg_height;
    SDL_QueryTexture(bg_texture, NULL, NULL, &bg_width, &bg_height);


    SDL_Event event;
    while (running) {

        while (SDL_PollEvent(&event)) {

            // Verificar se o jogo foi fechado
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        if (keyState[SDL_SCANCODE_UP]) player_rect.y -= velocidade;
        if (keyState[SDL_SCANCODE_DOWN]) player_rect.y += velocidade;
        if (keyState[SDL_SCANCODE_LEFT]) player_rect.x -= velocidade;
        if (keyState[SDL_SCANCODE_RIGHT]) player_rect.x += velocidade;

        update_camera(player_rect.x, player_rect.y, &camera, player_rect, bg_width, bg_height);


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
             player_rect.x - camera.x,
             player_rect.y - camera.y,
             player_rect.w, player_rect.h
        };
        SDL_RenderCopy(renderer, char_texture, nullptr, &player_render_rect);


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