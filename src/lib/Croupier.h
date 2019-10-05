#ifndef POKER_CROUPIER_H
#define POKER_CROUPIER_H

#include <random>
#include "Deck.h"

class Croupier {
public:
    Croupier();

    [[nodiscard]] Card next_card();

    void shuffle();

private:
    Deck deck;
    size_t pointer;
    std::random_device dev;
    std::mt19937 engine;
};

#endif  // POKER_CROUPIER_H
