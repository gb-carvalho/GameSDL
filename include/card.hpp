#pragma once
#include <string>
#include <vector>
#include <entity.hpp>

#define CARD_WIDTH       542
#define CARD_HEIGHT      809
#define CARDS_TO_CHOSE   3
#define MAX_CARD_LEVEL   5

class Card : public Entity {
public:
    std::string name, description;
    int level;

    Card();
    Card(SDL_Rect src, SDL_Rect dst, std::string nme, std::string dscrption, int lvl);
};

extern std::vector<Card> cards;
