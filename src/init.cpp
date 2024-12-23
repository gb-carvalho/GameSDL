#include <init.hpp>

bool Init()
{
    // Inicializa o SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Erro ao init SDL: %s", SDL_GetError());
        return false;
    }
    return true;
}

void InitWindow(int screen_width, int screen_height, SDL_Window** g_window)
{
    // Cria a janela
    *g_window = SDL_CreateWindow("Hello SDL World",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screen_width, screen_height, SDL_WINDOW_SHOWN);
    if (!(*g_window)) {
        SDL_Log("Error ao creating window: %s", SDL_GetError());
        SDL_Quit();
    }
}

void InitRenderer(SDL_Window** g_window, SDL_Renderer** g_renderer)
{
    // Cria o renderizador
    *g_renderer = SDL_CreateRenderer(*g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!(*g_renderer)) {
        SDL_Log("Error creating render: %s", SDL_GetError());
        SDL_DestroyWindow(*g_window);
        SDL_Quit();
        return;
    }

    if (SDL_SetRenderDrawBlendMode(*g_renderer, SDL_BLENDMODE_BLEND) != 0) {
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