#ifndef POKER_PLAYER_H
#define POKER_PLAYER_H

#include "Card.h"
#include <utility>
#include <cstddef>
#include <stdexcept>


class Player {
    std::pair<Card, Card> hand_cards;
public:
    size_t balance;
    size_t infront;

    explicit Player(size_t money);

    void from_pot(size_t pot);

    void add_to_infront(size_t amount);

    void to_pot(size_t amount);

    std::pair<Card, Card> show_cards();

    void retake(Card lhs, Card rhs);

};

#endif //POKER_PLAYER_H
