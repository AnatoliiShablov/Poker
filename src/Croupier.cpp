#include "Croupier.h"

#include <algorithm>
#include <ctime>
#include <stdexcept>

Croupier::Croupier()
  : deck{}
  , pointer{0}
  , engine{static_cast<std::mt19937::result_type>(std::time(nullptr))} {}

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
