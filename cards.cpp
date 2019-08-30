#include "cards.h"

Deck::Deck() : engine{static_cast<std::mt19937::result_type>(std::time(nullptr))},
               deck{{
                            {Suit::diamonds, Rank::two},
                            {Suit::diamonds, Rank::three},
                            {Suit::diamonds, Rank::four},
                            {Suit::diamonds, Rank::five},
                            {Suit::diamonds, Rank::six},
                            {Suit::diamonds, Rank::seven},
                            {Suit::diamonds, Rank::eight},
                            {Suit::diamonds, Rank::nine},
                            {Suit::diamonds, Rank::ten},
                            {Suit::diamonds, Rank::jack},
                            {Suit::diamonds, Rank::queen},
                            {Suit::diamonds, Rank::king},
                            {Suit::diamonds, Rank::ace},

                            {Suit::clubs, Rank::two},
                            {Suit::clubs, Rank::three},
                            {Suit::clubs, Rank::four},
                            {Suit::clubs, Rank::five},
                            {Suit::clubs, Rank::six},
                            {Suit::clubs, Rank::seven},
                            {Suit::clubs, Rank::eight},
                            {Suit::clubs, Rank::nine},
                            {Suit::clubs, Rank::ten},
                            {Suit::clubs, Rank::jack},
                            {Suit::clubs, Rank::queen},
                            {Suit::clubs, Rank::king},
                            {Suit::clubs, Rank::ace},

                            {Suit::hearts, Rank::two},
                            {Suit::hearts, Rank::three},
                            {Suit::hearts, Rank::four},
                            {Suit::hearts, Rank::five},
                            {Suit::hearts, Rank::six},
                            {Suit::hearts, Rank::seven},
                            {Suit::hearts, Rank::eight},
                            {Suit::hearts, Rank::nine},
                            {Suit::hearts, Rank::ten},
                            {Suit::hearts, Rank::jack},
                            {Suit::hearts, Rank::queen},
                            {Suit::hearts, Rank::king},
                            {Suit::hearts, Rank::ace},

                            {Suit::spades, Rank::two},
                            {Suit::spades, Rank::three},
                            {Suit::spades, Rank::four},
                            {Suit::spades, Rank::five},
                            {Suit::spades, Rank::six},
                            {Suit::spades, Rank::seven},
                            {Suit::spades, Rank::eight},
                            {Suit::spades, Rank::nine},
                            {Suit::spades, Rank::ten},
                            {Suit::spades, Rank::jack},
                            {Suit::spades, Rank::queen},
                            {Suit::spades, Rank::king},
                            {Suit::spades, Rank::ace}
                    }},
               pointer{0} {
    shuffle();
}

void Deck::shuffle() {
    pointer = 0;
    std::shuffle(deck.begin(), deck.end(), engine);
}

template<>
Card Deck::next_card<false>() {
    if (pointer > 50) {
        throw std::runtime_error("Deck is empty");
    }
    return deck[++pointer];
}

template<>
Card Deck::next_card<true>() {
    ++pointer;
    return Deck::next_card();
}

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
    if (amount > balance) {
        throw std::runtime_error("You don't have enough money");
    }
    infront += amount;
    balance -= amount;
}

std::pair<Card, Card> Player::show_cards() {
    return hand_cards;
}

void Player::retake(Card lhs, Card rhs) {
    hand_cards = std::make_pair(lhs, rhs);
}
