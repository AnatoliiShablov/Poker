#ifndef POKER_DECK_H
#define POKER_DECK_H

#include "Card.h"
#include <array>
#include <random>
#include <algorithm>
#include <ctime>

class Deck {
    std::mt19937 engine;
    std::array<Card, 52> deck;
    size_t pointer;
public:
    Deck();

    void shuffle();

    Card next_card();
};


#endif //POKER_DECK_H
