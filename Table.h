#ifndef POKER_TABLE_H
#define POKER_TABLE_H

#include "Deck.h"
#include "Player.h"

class Table {
    typedef int ID;
    Deck deck{};
    std::vector<std::pair<Player, ID>> players;
public:
    size_t dealer{};
    size_t small_blind;
    std::array<Card, 5> table_cards;

    template<typename InputIterator>
    Table(InputIterator beginID, InputIterator endID, size_t start_balance, size_t small_blind) :
            small_blind{small_blind}, players(endID - beginID) {
        for (size_t i = 0; i < players.size(); ++i) {
            players[i] = *(beginID + i);
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
        deck.shuffle();
        for (auto &player:players) {
            player.first.retake(deck.next_card(), deck.next_card());
        }
        std::vector<bool> players_in_game(players.size(), true);
        size_t players_left = players.size();
        size_t pot = 0;
        if (first_cirkle(pot, players_in_game, players_left)) {
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

    void cirkle() {

    }

};

#endif //POKER_TABLE_H
