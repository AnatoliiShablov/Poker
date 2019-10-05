#ifndef POKER_SIGNALS_H
#define POKER_SIGNALS_H

#include "SFML/Network.hpp"

struct Signal {
    enum Enum : sf::Uint8 {
        WaitForActionCheckRaise,
        WaitForActionCallRaiseFold,
        WaitForActionCallFold,
        NewGameInfo,
        ChangeHandCards,
        Win,
        Lost,
        ChangeTableCards,
        ChangeDealer,
        ChangeActivePlayer,
        ChangeBalance,
        ChangePot,
        ShowCards,
        ShowWinner,
        NaI
    };

    constexpr Signal(Enum value = Enum::NaI) noexcept;

    constexpr friend bool operator==(Signal const &lhs, Signal const &rhs) noexcept;

    constexpr friend bool operator!=(Signal const &lhs, Signal const &rhs) noexcept;

    friend sf::Packet &operator<<(sf::Packet &packet, Signal const &signal);

    friend sf::Packet &operator>>(sf::Packet &packet, Signal &signal);

    explicit operator bool() = delete;

    constexpr Enum &operator()() noexcept;

    constexpr Enum operator()() const noexcept;

private:
    Enum value_;
};

#endif  // POKER_SIGNALS_H
