#include "Croupier.h"

#include <algorithm>
#include <stdexcept>

Croupier::Croupier() : deck{}, pointer{0}, dev{}, engine{dev()} {}

Card Croupier::next_card() {
    if (pointer > 50) {
        throw std::runtime_error("No more cards in deck");
    }
    return deck[++pointer];
}

void Croupier::shuffle() {
    std::shuffle(deck.begin(), deck.end(), engine);
    pointer = 0;
}
