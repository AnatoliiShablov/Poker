#include "Player.h"

#include <stdexcept>

Player::Player(sf::Uint64 money) : balance{money}, infront{0} {}

Player::Hand Player::show_cards() const {
    return hand_cards;
}

void Player::retake(Player::Hand hand) noexcept {
    hand_cards = hand;
}

sf::Packet &operator<<(sf::Packet &packet, const Player::Hand &hand) {
    return packet << hand.lhs << hand.rhs;
}

sf::Packet &operator>>(sf::Packet &packet, Player::Hand &hand) {
    return packet >> hand.lhs >> hand.rhs;
}
