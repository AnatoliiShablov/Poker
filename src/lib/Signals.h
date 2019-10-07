#ifndef POKER_SIGNALS_H
#define POKER_SIGNALS_H

#include "SFML/Network.hpp"

struct Signal {
    enum Enum : sf::Uint8 {
        WaitForActionCheckRaise,
        WaitForActionCallRaiseFold,
        WaitForActionCallFold,
        NewGameInfo,//++
        Win,//++
        Lost,//++
        ChangeTableCards,//++
        ChangeActivePlayer,//+
        ChangeBalances,//++
        ChangePot,//+
        ShowCards,//+
        ShowWinners,//+
        NaI
    };

    Signal(Enum value = Enum::NaI) noexcept;

    friend bool operator==(Signal const &lhs, Signal const &rhs) noexcept;

    friend bool operator!=(Signal const &lhs, Signal const &rhs) noexcept;

    friend sf::Packet &operator<<(sf::Packet &packet, Signal const &signal);

    friend sf::Packet &operator>>(sf::Packet &packet, Signal &signal);

    explicit operator bool() = delete;

    Enum &operator()() noexcept;

    Enum operator()() const noexcept;

private:
    Enum value_;
};

struct Action {
    enum Enum : sf::Uint8 {
        call, raise, fold, check, NaA
    };
    Enum type;
    sf::Uint64 additional_info;

    Action(Enum value = Enum::NaA) noexcept : type{value}, additional_info{0} {}

    friend bool operator==(Action const &lhs, Action const &rhs) noexcept { return lhs.type == rhs.type; }

    friend bool operator!=(Action const &lhs, Action const &rhs) noexcept { return lhs.type != rhs.type; }

    friend sf::Packet &operator<<(sf::Packet &packet, Action const &action) {
        if (action == Enum::raise) {
            return packet << static_cast<sf::Uint8>(Enum::raise) << action.additional_info;
        }
        return packet << static_cast<sf::Uint8>(action.type);
    }

    friend sf::Packet &operator>>(sf::Packet &packet, Action &action) {
        sf::Uint8 buf;
        packet >> buf;
        action = static_cast<Enum>(buf);
        if (action == Enum::raise) {
            return packet >> action.additional_info;
        }
        return packet;
    }

    explicit operator bool() = delete;
};

#endif  // POKER_SIGNALS_H
