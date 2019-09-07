#ifndef POKER_DECK_H
#define POKER_DECK_H

#include "Card.h"
#include <array>

struct Deck {
    using iterator = std::array<Card, 52>::iterator;
    using const_iterator = std::array<Card, 52>::const_iterator;

    Deck() noexcept;

    [[nodiscard]] Card operator[](size_t pos) const;

    [[nodiscard]] iterator begin();

    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator cbegin() const;

    [[nodiscard]] iterator end();

    [[nodiscard]] const_iterator end() const;
    [[nodiscard]] const_iterator cend() const;

private:
    std::array<Card, 52> deck;
};


#endif //POKER_DECK_H
