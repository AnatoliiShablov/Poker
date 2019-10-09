#ifndef POKER_CARD_H
#define POKER_CARD_H

#include <type_traits>
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"

struct Suit {
    enum Enum : sf::Uint8 { diamonds, clubs, hearts, spades, NaS };

    Suit(Enum value = Enum::NaS) noexcept;

    friend bool operator==(Suit const &lhs, Suit const &rhs) noexcept;

    friend bool operator!=(Suit const &lhs, Suit const &rhs) noexcept;

    friend sf::Packet &operator<<(sf::Packet &packet, Suit const &suit);

    friend sf::Packet &operator>>(sf::Packet &packet, Suit &suit);

    explicit operator bool() = delete;

    Enum &operator()() noexcept;

    Enum operator()() const noexcept;

    [[nodiscard]] sf::String to_string() const;

private:
    Enum value_;
};

struct Rank {
    enum Enum : sf::Uint8 { two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace, NaR };

    Rank(Enum value = Enum::NaR) noexcept;

    friend bool operator==(Rank const &lhs, Rank const &rhs) noexcept;

    friend bool operator!=(Rank const &lhs, Rank const &rhs) noexcept;

    friend bool operator<(Rank const &lhs, Rank const &rhs) noexcept;

    friend bool operator>(Rank const &lhs, Rank const &rhs) noexcept;

    friend bool operator<=(Rank const &lhs, Rank const &rhs) noexcept;

    friend bool operator>=(Rank const &lhs, Rank const &rhs) noexcept;

    friend sf::Packet &operator<<(sf::Packet &packet, Rank const &rank);

    friend sf::Packet &operator>>(sf::Packet &packet, Rank &rank);

    explicit operator bool() = delete;

    Enum &operator()() noexcept;

    Enum operator()() const noexcept;

    [[nodiscard]] sf::String to_string() const;

    [[nodiscard]] sf::Uint8 to_uint8() const noexcept;

private:
    Enum value_;
};

struct Card {
    Suit suit;
    Rank rank;

    friend bool operator==(Card const &lhs, Card const &rhs) noexcept;

    friend bool operator!=(Card const &lhs, Card const &rhs) noexcept;

    friend sf::Packet &operator<<(sf::Packet &packet, Card const &card);

    friend sf::Packet &operator>>(sf::Packet &packet, Card &card);

    sf::Sprite getSprite();
};

#endif  // POKER_CARD_H
