#include "card.hpp"

std::vector<Card> cards = {
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Heal", "Restore 1 health point.", 1},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Fire Rate", "Shoot faster", 3},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Speed", "Increases speed.", 4},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Damage", "Inceases damage.", 5},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Projectile Speed", "Increases projectile speed", 5},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "EXP", "Increases EXP gain.", 5},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Flameball", "Gain/Improves Flameball.", 5},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Vortex", "Gain/Improves Vortex.", 5},
    { {0, 0, CARD_WIDTH, CARD_HEIGHT}, {0, 0, 0, 0}, "Flame Pillar", "Gain/Improves Flame Pillar.", 5},
};

Card::Card()
    : Entity(0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, nullptr), name(""), description(""), level(0) {}

Card::Card(SDL_Rect src, SDL_Rect dst, std::string nme, std::string dscrption, int lvl)
    : Entity(0, 0, 0, 0, src, dst, nullptr), name(nme), description(dscrption), level(lvl) {
    //UpdateHitbox();
}