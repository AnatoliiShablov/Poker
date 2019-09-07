#ifndef POKER_TABLE_H
#define POKER_TABLE_H

#include "Croupier.h"
#include "Player.h"

class Table {
    typedef int ID;
    Croupier croupier;
    std::vector<std::pair<Player, ID>> players;

public:
    size_t dealer{};
    size_t small_blind;
    std::array<Card, 5> table_cards;

    template <typename InputIterator>
    Table(InputIterator beginID, InputIterator endID, size_t start_balance, size_t small_blind)
        : small_blind{small_blind} {
        for (auto it = beginID; it != endID; ++it) {
            players.emplace_back(Player(start_balance), *it);
        }
        tournament();
    }

    void tournament() {
        while (players.size() > 1) {
            game();
            bool flag = false;
            for (size_t i = 0; i < players.size(); ++i) {
                if (players[i].first.balance == 0) {
                    players.erase(players.begin() + i);
                    if (i <= dealer) {
                        flag = true;
                    }
                }
            }
            if (flag) {
                ++dealer;
            }
            dealer %= players.size();
        }
    }

    void game() {
        croupier.shuffle();
        for (auto &player : players) {
            player.first.retake(croupier.next_card(), croupier.next_card());
        }
        std::vector<Pot> pots;
        if (first_cirkle(pots)) {
            return;
        }
        table_cards[0] = deck.next_card();
        table_cards[1] = deck.next_card();
        table_cards[2] = deck.next_card();
        if (cirkle(pot, players_in_game, players_left)) {
            return;
        }
        table_cards[3] = deck.next_card();
        if (cirkle(pot, players_in_game, players_left)) {
            return;
        }
        table_cards[4] = deck.next_card();
        if (cirkle(pot, players_in_game, players_left)) {
            return;
        }
        give_to_winner(pot, players_in_game);
    }

    void cirkle() {}
};

#endif  // POKER_TABLE_H
