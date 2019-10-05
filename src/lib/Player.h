#ifndef POKER_PLAYER_H
#define POKER_PLAYER_H

#include <cstddef>
#include <utility>
#include "Card.h"
#include "SFML/System.hpp"

class Player {
public:
    using Hand = std::pair<Card, Card>;
    sf::Uint64 balance;
    sf::Uint64 infront;

    explicit Player(sf::Uint64 money = 0);

    [[nodiscard]] Hand show_cards() const;

    void retake(Card lhs, Card rhs);

private:
    Hand hand_cards;
};

#endif  // POKER_PLAYER_H
