#include "Player.h"

#include <stdexcept>

Player::Player(sf::Uint64 money) : balance{money}, infront{0} {}

Player::Hand Player::show_cards() const {
    return hand_cards;
}

void Player::retake(Card lhs, Card rhs) {
    hand_cards = std::make_pair(lhs, rhs);
}
