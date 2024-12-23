#pragma once
#include <SDL_mixer.h>
#include <SDL.h>

bool Init();
void InitWindow(int screen_width, int screen_height, SDL_Window** g_window);
void InitRenderer(SDL_Window** g_window, SDL_Renderer** g_renderer);
void InitSDLMusic();
Mix_Chunk* InitSoundEffect(const char* sound_path);
Mix_Music* InitMusic(const char* music_path, int loops);