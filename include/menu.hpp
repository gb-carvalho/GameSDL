#pragma once
#include <string>
#include <vector>
#include <entity.hpp>

#define OPTION_WIDTH       600
#define OPTION_HEIGHT      200

class MenuOption : public Entity {
public:
    std::string name;
    SDL_Color text_color;
    SDL_Color selected_color;

    MenuOption();
    MenuOption(SDL_Rect src, SDL_Rect dst, std::string name, SDL_Color text_color, SDL_Color selected_color);
};

extern std::vector<MenuOption> menuOptions;