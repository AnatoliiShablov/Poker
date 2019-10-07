#ifndef POKER_PLAYER_H
#define POKER_PLAYER_H

#include <cstddef>
#include <utility>
#include "Card.h"
#include "SFML/System.hpp"

class Player {
public:
    struct Hand {
        Card lhs;
        Card rhs;

        friend sf::Packet &operator<<(sf::Packet &packet, Hand const &hand);

        friend sf::Packet &operator>>(sf::Packet &packet, Hand &hand);
    };

    sf::Uint64 balance;
    sf::Uint64 infront;

    explicit Player(sf::Uint64 money = 0);

    [[nodiscard]] Hand show_cards() const;

    void retake(Hand hand) noexcept;

private:
    Hand hand_cards;
};

#endif  // POKER_PLAYER_H
