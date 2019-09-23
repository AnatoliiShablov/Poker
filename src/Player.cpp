#include "Player.h"

#include <stdexcept>

Player::Player(size_t money) : balance{money}, infront{0} {}

void Player::from_pot(size_t pot) {
    balance += pot + infront;
    infront = 0;
}

void Player::add_to_infront(size_t amount) {
    if (amount > balance) {
        throw std::runtime_error("You don't have enough money");
    }
    infront += amount;
    balance -= amount;
}

void Player::to_pot(size_t amount) {
    if (amount > infront) {
        throw std::runtime_error("You don't have enough money in front of you");
    }
    infront -= amount;
}

Player::Hand Player::show_cards() {
    return hand_cards;
}

void Player::retake(Card lhs, Card rhs) {
    hand_cards = std::make_pair(lhs, rhs);
}
