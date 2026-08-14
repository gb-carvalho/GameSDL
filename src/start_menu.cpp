#include "menu.hpp"

std::vector<MenuOption> menuOptions = {
    { {0, 0, OPTION_WIDTH, OPTION_HEIGHT}, {0, 0, 0, 0}, "Start Game", { 255, 255, 255 }, { 255, 214, 10 } },
    { {0, 0, OPTION_WIDTH, OPTION_HEIGHT}, {0, 0, 0, 0}, "Highest Score", { 255, 255, 255 }, { 255, 214, 10 } },
    { {0, 0, OPTION_WIDTH, OPTION_HEIGHT}, {0, 0, 0, 0}, "Exit", { 255, 255, 255 }, { 255, 214, 10 } },
};


MenuOption::MenuOption()
    : Entity(0, 0, 0, 0, { 0, 0, OPTION_WIDTH, OPTION_HEIGHT }, { 0, 0, OPTION_WIDTH, OPTION_HEIGHT }, nullptr),
      name(""), text_color({255, 255, 255}), selected_color({255, 214, 10}) {}

MenuOption::MenuOption(SDL_Rect src, SDL_Rect dst, std::string nme, SDL_Color text_color, SDL_Color selected_color)
    : Entity(0, 0, 0, 0, src, dst, nullptr), name(nme), text_color(text_color), selected_color(selected_color) {}
