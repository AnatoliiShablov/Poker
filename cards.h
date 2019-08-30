#ifndef POKER_DECK_H
#define POKER_DECK_H

#include <stdexcept>
#include <array>
#include <algorithm>
#include <random>
#include <ctime>

enum class Suit {
    diamonds,
    clubs,
    hearts,
    spades
};
enum class Rank {
    two,
    three,
    four,
    five,
    six,
    seven,
    eight,
    nine,
    ten,
    jack,
    queen,
    king,
    ace
};

struct Card {
    Suit suit;
    Rank rank;
};

class Deck {
    std::mt19937 engine;
    std::array<Card, 52> deck;
    size_t pointer;
public:
    Deck();

    void shuffle();

    template<bool skip = false>
    Card next_card();
};

class Player {
    std::pair<Card, Card> hand_cards;
public:
    size_t balance;
    size_t infront;

    explicit Player(size_t money);

    void from_pot(size_t pot);

    void add_to_infront(size_t amount);

    void to_pot(size_t amount);

    std::pair<Card, Card> show_cards();

    void retake(Card lhs, Card rhs);

};


class Table {
    Deck deck{};
    size_t players_left{};
    size_t position{};
    size_t dealer{};
    size_t small_blind;
    size_t max_bet{};
    std::vector<Player> players;
public:
    Table(size_t players_amount, size_t start_balance, size_t small_blind) : small_blind{small_blind},
                                                                             players(players_amount,
                                                                                     Player{start_balance}) { tournament(); }

    void tournament() {
        while (players.size() > 1) {
            game();
            for (size_t i = 0; i < players.size(); ++i) {
                if (players[i].balance == 0) {
                    players.erase(players.begin() + i); // Question about player id and position in array;
                }
            }
            ++position;
        }
    }

    void game() {
        deck.shuffle();
        for (auto &player:players) {
            player.retake(deck.next_card(), deck.next_card());
        }
        cirkle();
    }

    void cirkle() {

    }

};

#endif //POKER_DECK_H
