#include "Card.h"

Suit::Suit(Suit::Enum value) noexcept : value_{value} {}

bool operator==(Suit const &lhs, Suit const &rhs) noexcept {
    return lhs.value_ == rhs.value_;
}

bool operator!=(Suit const &lhs, Suit const &rhs) noexcept {
    return lhs.value_ != rhs.value_;
}

sf::Packet &operator<<(sf::Packet &packet, Suit const &suit) {
    return packet << static_cast<sf::Uint8>(suit.value_);
}

sf::Packet &operator>>(sf::Packet &packet, Suit &suit) {
    sf::Uint8 buf;
    packet >> buf;
    suit = static_cast<Suit::Enum>(buf);
    return packet;
}

Suit::Enum &Suit::operator()() noexcept {
    return value_;
}

Suit::Enum Suit::operator()() const noexcept {
    return value_;
}

sf::String Suit::to_string() const {
    switch (value_) {
    case diamonds:
        return "D";
    case clubs:
        return "C";
    case hearts:
        return "H";
    case spades:
        return "S";
    case NaS:
        return "N";
    }
    return "";
}

Rank::Rank(Rank::Enum value) noexcept : value_{value} {}

bool operator==(Rank const &lhs, Rank const &rhs) noexcept {
    return lhs.value_ == rhs.value_;
}

bool operator!=(Rank const &lhs, Rank const &rhs) noexcept {
    return lhs.value_ != rhs.value_;
}

bool operator<(Rank const &lhs, Rank const &rhs) noexcept {
    return static_cast<sf::Uint8>(lhs.value_) < static_cast<sf::Uint8>(rhs.value_);
}

bool operator>(Rank const &lhs, Rank const &rhs) noexcept {
    return static_cast<sf::Uint8>(lhs.value_) > static_cast<sf::Uint8>(rhs.value_);
}

bool operator<=(Rank const &lhs, Rank const &rhs) noexcept {
    return static_cast<sf::Uint8>(lhs.value_) <= static_cast<sf::Uint8>(rhs.value_);
}

bool operator>=(Rank const &lhs, Rank const &rhs) noexcept {
    return static_cast<sf::Uint8>(lhs.value_) >= static_cast<sf::Uint8>(rhs.value_);
}

sf::Packet &operator<<(sf::Packet &packet, Rank const &rank) {
    return packet << static_cast<sf::Uint8>(rank.value_);
}

sf::Packet &operator>>(sf::Packet &packet, Rank &rank) {
    sf::Uint8 buf;
    packet >> buf;
    rank = static_cast<Rank::Enum>(buf);
    return packet;
}

Rank::Enum &Rank::operator()() noexcept {
    return value_;
}

Rank::Enum Rank::operator()() const noexcept {
    return value_;
}

sf::String Rank::to_string() const {
    switch (value_) {
    case two:
        return "2";
    case three:
        return "3";
    case four:
        return "4";
    case five:
        return "5";
    case six:
        return "6";
    case seven:
        return "7";
    case eight:
        return "8";
    case nine:
        return "9";
    case ten:
        return "T";
    case jack:
        return "J";
    case queen:
        return "Q";
    case king:
        return "K";
    case ace:
        return "A";
    case NaR:
        return "N";
    }
    return "";
}

sf::Uint8 Rank::to_uint8() const noexcept {
    return static_cast<sf::Uint8>(value_);
}

bool operator==(Card const &lhs, Card const &rhs) noexcept {
    return (lhs.suit == rhs.suit) && (lhs.rank == rhs.rank);
}

bool operator!=(Card const &lhs, Card const &rhs) noexcept {
    return (lhs.suit != rhs.suit) || (lhs.rank != rhs.rank);
}

sf::Packet &operator<<(sf::Packet &packet, Card const &card) {
    return packet << card.suit << card.rank;
}

sf::Packet &operator>>(sf::Packet &packet, Card &card) {
    return packet >> card.suit >> card.rank;
}

sf::String Card::to_string() {
    return suit.to_string() + rank.to_string();
}
