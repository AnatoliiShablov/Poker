#include "Signals.h"

constexpr Signal::Signal(Signal::Enum value) noexcept : value_{value} {}

constexpr bool operator==(Signal const &lhs, Signal const &rhs) noexcept {
    return lhs.value_ == rhs.value_;
}

constexpr bool operator!=(Signal const &lhs, Signal const &rhs) noexcept {
    return lhs.value_ != rhs.value_;
}

sf::Packet &operator<<(sf::Packet &packet, Signal const &signal) {
    return packet << static_cast<sf::Uint8>(signal.value_);
}

sf::Packet &operator>>(sf::Packet &packet, Signal &signal) {
    sf::Uint8 buf;
    packet >> buf;
    signal = static_cast<Signal::Enum>(buf);
    return packet;
}

constexpr Signal::Enum &Signal::operator()() noexcept {
    return value_;
}

constexpr Signal::Enum Signal::operator()() const noexcept {
    return value_;
}