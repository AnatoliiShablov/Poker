#ifndef POKER_TABLE_H
#define POKER_TABLE_H

#include <algorithm>
#include "Croupier.h"
#include "Player.h"

enum class ActionType { call, raise, fold, check };

struct Action {
    ActionType type;
    size_t additional_info;
};

class Table {
    typedef int ID;
    struct Pot {
        std::vector<bool> players_for_pot;
        size_t money;
    };
    struct InGame {
        std::vector<bool> players_in_game;
        size_t amount;
    };
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
        InGame in_game;
        if (cirkle(pots, in_game, true)) {
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

    bool cirkle(std::vector<Pot> &pots, InGame &in_game, bool first = false) {
        size_t player_now = dealer;
        size_t max = 0;
        if (first) {
            in_game.players_in_game.resize(players.size(), true);
            in_game.amount = players.size();
            player_now = (player_now + 1) % players.size();
            players[player_now].first.add_to_infront(std::min(small_blind, players[player_now].first.balance));
            player_now = (player_now + 1) % players.size();
            players[player_now].first.add_to_infront(std::min(small_blind * 2, players[player_now].first.balance));
            max = small_blind * 2;
        }
        size_t last_bet = player_now;
        player_now = (player_now + 1) % players.size();
        while (player_now != last_bet) {
            if (players[player_now].first.balance > 0 && in_game.players_in_game[player_now]) {
                Action action = get_action(players[player_now.second], max);
                if (action.type == ActionType::call) {
                    players[player_now].first.add_to_infront(std::min(max, players[player_now].first.balance));
                }
                /*if (action.type == ActionType::check) {
                   Do nothing
                }*/

                if (action.type == ActionType::fold) {
                    in_game.players_in_game[player_now] = false;
                    --in_game.amount;
                    if (in_game.amount == 1) {
                        break;
                    }
                }

                if (action.type == ActionType::raise) {
                    players[player_now].first.add_to_infront(
                        std::min(action.additional_info, players[player_now].second.balance()));
                    last_bet = player_now;
                }
            }
            player_now = (player_now + 1) % players.size();
        }

        if (in_game.amount == 1) {
            size_t pos =
                std::find(in_game.players_in_game.begin(), in_game.players_in_game.end(), [](auto &in) { return in; }) -
                in_game.players_in_game.begin();
            for (size_t i = 0; i < players.size(); ++i) {
                if (i == pos) {
                    continue;
                }
                players[pos].first.from_pot(players[i].first.infront());
                players[i].first.to_pot(players[i].first.infront());
            }
            for (size_t i = 0; i < pots.size(); ++i) {
                players[pos].first.from_pot(pots[i].money);
            }
            return true;
        }

        while (std::any_of(players.begin(), players.end(), [](auto &player) { return player.first.balance > 0; })) {
            std::vector<bool> players_for_pot(players.size(), false);
            size_t money = 0;
            size_t min_infront;
            std::for_each(players.begin(), players.end(), [&min_infront](auto &player) {
                if (player.first.infront < min_infront && player.first.infront > 0) {
                    min_infront = player.first.infront;
                }
            });

            for (size_t i = 0; i < players.size(); ++i) {
                if (players[i].first.infront > 0) {
                    money += min_infront;
                    players[i].first.to_pot(min_infront);
                    if (in_game[i]) {
                        players_for_pot[i] = true;
                    }
                }
            }
            pots.emplace_back(players_for_pot, money);
        }
        return false;
    }
};

#endif  // POKER_TABLE_H
