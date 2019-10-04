#ifndef POKER_PLAYER_H
#define POKER_PLAYER_H

#include <cstddef>
#include <utility>
#include "Card.h"

class Player {
public:
    using Hand = std::pair<Card, Card>;
    size_t balance;
    size_t infront;

    explicit Player(size_t money);

    void from_pot(size_t pot);

    void add_to_infront(size_t amount);

    void to_pot(size_t amount);

    Hand show_cards();

    void retake(Card lhs, Card rhs);

private:
    Hand hand_cards;
};

#endif  // POKER_PLAYER_H
