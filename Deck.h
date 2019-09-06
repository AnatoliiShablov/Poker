#ifndef POKER_DECK_H
#define POKER_DECK_H

#include "Card.h"
#include <array>

struct Deck {
    std::array<Card, 52> deck;

    Deck() noexcept;

    Card operator[](size_t pos) const;

    [[nodiscard]] std::array<Card, 52>::iterator begin();

    [[nodiscard]] std::array<Card, 52>::const_iterator begin() const;

    [[nodiscard]] std::array<Card, 52>::iterator end();

    [[nodiscard]] std::array<Card, 52>::const_iterator end() const;
};


#endif //POKER_DECK_H
