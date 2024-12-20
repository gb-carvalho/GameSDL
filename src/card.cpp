#include "card.hpp"

std::vector<Card> cards = {
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Fire Rate", "Shoot faster", 0},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Heal", "Restore 1 health point.", 0},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Speed", "Gain 1 speed point.", 0},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Damage", "Gain 1 damage point.", 0},
};

Card::Card()
    : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), name(""), description(""), level(0) {}

Card::Card(SDL_Rect src, SDL_Rect dst, std::string nme, std::string dscrption, int lvl)
    : Entity(0, 0, 0, 0, src, dst, nullptr), name(nme), description(dscrption), level(lvl) {
    //UpdateHitbox();
}