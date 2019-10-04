#ifndef POKER_TABLE_H
#define POKER_TABLE_H

#include <algorithm>
#include "Croupier.h"
#include "Player.h"
#include "SFML/Network.hpp"
#include "SFML/System.hpp"

enum class ActionType { call, raise, fold, check };

struct Action {
    ActionType type;
    size_t additional_info;
};

class Table {
    typedef std::add_pointer_t<sf::TcpSocket> ID;

    struct Pot {
        std::vector<bool> players_for_pot;
        size_t money{0};

        Pot(std::vector<bool> const &players, size_t money) : players_for_pot(players), money(money) {}
    };

    struct InGame {
        std::vector<bool> players_in_game;
        size_t amount;

        InGame(size_t amount) : players_in_game(amount, true), amount(amount) {}
    };

    struct HCPower {
        uint32_t power;

        HCPower(Player::Hand const &hc, std::array<Card, 5> const &tc) {
            std::vector<Card> cards = {tc[0], tc[1], tc[2], tc[3], tc[4], hc.first, hc.second};
            size_t tmp1, tmp2;
            std::array<uint8_t, 13> checker;

            // check for straight flush
            for (Suit suit : std::array<Suit, 4>{Suit::clubs, Suit::diamonds, Suit::hearts, Suit::spades}) {
                checker.fill(0);
                tmp1 = 0;
                for (size_t i = 0; i < 7; ++i) {
                    if (cards[i].suit == suit) {
                        ++checker[rank_uint(cards[i].rank) - 2];
                    }
                }
                for (size_t i = 13; i--;) {
                    if (checker[i]) {
                        if (++tmp1 == 5) {
                            power = 0xF0000000 | UINT32_C(1) << (18 + i);
                            return;
                        }
                    } else {
                        tmp1 = 0;
                    }
                }
                if (tmp1 == 4 && checker[12]) {
                    power = 0xF0000000 | UINT32_C(1) << 17u;
                    return;
                }
            }

            // check for four of a kind
            checker.fill(0);
            for (size_t i = 0; i < 7; ++i) {
                ++checker[rank_uint(cards[i].rank) - 2];
            }
            tmp1 = 0;
            tmp2 = 0;
            for (size_t i = 13; i--;) {
                if (checker[i] == 4) {
                    power = 0xE0000000 | UINT32_C(1) << (14 + i);
                    if (tmp2 == 1) {
                        power |= UINT32_C(1) << tmp1;
                        return;
                    }
                    tmp2 = 2;
                }
                if (tmp2 == 0 && checker[i]) {
                    tmp1 = i;
                    tmp2 = 1;
                }
                if (tmp2 == 2 && checker[i]) {
                    power |= UINT32_C(1) << i;
                    return;
                }
            }

            // check for full house
            tmp1 = 15;
            tmp2 = 15;
            for (size_t i = 13; i--;) {
                if (tmp1 == 15 && checker[i] == 3) {
                    tmp1 = i;
                    continue;
                }
                if (tmp2 == 15 && checker[i] >= 2) {
                    tmp2 = i;
                }
            }
            if (tmp1 != 15 && tmp2 != 15) {
                power = 0xD0000000 | UINT32_C(1) << (14 + tmp1) | UINT32_C(1) << (14 + tmp2);
                return;
            }

            // check for flush
            for (Suit suit : std::array<Suit, 4>{Suit::clubs, Suit::diamonds, Suit::hearts, Suit::spades}) {
                tmp1 = 0;
                uint32_t mask = 0;
                for (size_t i = 0; i < 7; ++i) {
                    if (cards[i].suit == suit) {
                        mask |= UINT32_C(1) << (12u + rank_uint(cards[i].rank));
                        ++tmp1;
                    }
                }
                if (tmp1 >= 5) {
                    power = 0xC0000000 | mask;
                    return;
                }
            }

            // check for straight
            checker.fill(0);
            for (size_t i = 0; i < 7; ++i) {
                ++checker[rank_uint(cards[i].rank) - 2];
            }
            tmp1 = 0;
            for (size_t i = 13; i--;) {
                if (checker[i]) {
                    if (++tmp1 == 5) {
                        power = 0xB0000000 | UINT32_C(1) << (18 + i);
                        return;
                    }
                } else {
                    tmp1 = 0;
                }
            }
            if (tmp1 == 4 && checker[12]) {
                power = 0xB0000000 | UINT32_C(1) << 17u;
                return;
            }

            // check for three of a kind
            tmp1 = 15;
            tmp2 = 0;
            size_t c1 = 0;
            size_t c2 = 0;
            for (size_t i = 13; i--;) {
                if (tmp1 == 15 && checker[i] == 3) {
                    tmp1 = i;
                    if (tmp2 == 2) {
                        break;
                    }
                    continue;
                }
                if (tmp2 == 1 && checker[i]) {
                    c2 = i;
                    tmp2 = 2;
                }
                if (tmp2 == 0 && checker[i]) {
                    c1 = i;
                    tmp2 = 1;
                }
            }
            if (tmp1 != 15) {
                power = 0xA0000000 | UINT32_C(1) << (14 + tmp1) | UINT32_C(1) << c1 | UINT32_C(1) << c2;
                return;
            }

            // check for two pair
            tmp1 = 0;
            tmp2 = 0;
            c1 = 15;
            c2 = 15;
            for (size_t i = 13; i--;) {
                if (c1 == 15 && checker[i] == 2) {
                    c1 = i;
                    continue;
                }
                if (c2 == 15 && checker[i] == 2) {
                    c2 = i;
                    continue;
                }
                if (tmp1 == 0 && checker[i]) {
                    tmp1 = 1;
                    tmp2 = i;
                }
            }
            if (c1 != 15 && c2 != 15) {
                power = 0x90000000 | UINT32_C(1) << (14 + c1) | UINT32_C(1) << (14 + c2) | UINT32_C(1) << tmp2;
                return;
            }

            // check for one pair
            tmp1 = 15;
            tmp2 = 15;
            c1 = 15;
            c2 = 15;
            for (size_t i = 13; i--;) {
                if (tmp1 == 15 && checker[i] == 2) {
                    tmp1 = i;
                    continue;
                }
                if (c2 == 15 && checker[i]) {
                    c2 = i;
                    continue;
                }
                if (c1 == 15 && checker[i]) {
                    c1 = i;
                    continue;
                }
                if (tmp2 == 15 && checker[i]) {
                    tmp2 = i;
                    continue;
                }
            }
            if (tmp1 != 15) {
                power = 0x80000000 | UINT32_C(1) << (14 + tmp1) | UINT32_C(1) << (c1) | UINT32_C(1) << (c2) |
                        UINT32_C(1) << tmp2;
                return;
            }

            // check for high card
            tmp1 = 0;
            power = 0x70000000;
            for (size_t i = 13; i--;) {
                if (checker[i]) {
                    power |= UINT32_C(1) << (14 + i);
                    if (++tmp1 == 5) {
                        break;
                    }
                }
            }
        }
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
                    goodbye(players[i].second);
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
            sync_after_game();
        }
    }

    void give_to_winner(std::vector<Pot> const &pots) {
        std::vector<HCPower> powers;
        for (size_t i = 0; i < players.size(); ++i) {
            powers.emplace_back(players[i].first.show_cards(), table_cards);
        }
        for (Pot const &pot : pots) {
            std::vector<size_t> winners;
            winners.push_back(std::find(pot.players_for_pot.begin(), pot.players_for_pot.end(), true) -
                              pot.players_for_pot.begin());
            for (size_t i = winners.front() + 1; i < players.size(); ++i) {
                if (!pot.players_for_pot[i]) {
                    continue;
                }
                if (powers[i].power == powers[winners.front()].power) {
                    winners.push_back(i);
                } else if (powers[i].power > powers[winners.front()].power) {
                    winners.resize(1, i);
                }
            }
            std::vector<bool> send(players.size(), false);
            for (size_t i : winners) {
                send[i] = true;
            }
            for (size_t i = 0; i < players.size(); ++i) {
                send_money(players[i].second, (send[i] ? (pot.money / winners.size()) : 0));
            }
        }
    }

    void game() {
        croupier.shuffle();
        for (auto &player : players) {
            player.first.retake(croupier.next_card(), croupier.next_card());
        }
        std::vector<Pot> pots;
        InGame in_game(players.size());
        sync_in_game(pots, in_game, 0);
        if (cirkle(pots, in_game, 0)) {
            return;
        }
        table_cards[0] = croupier.next_card();
        table_cards[1] = croupier.next_card();
        table_cards[2] = croupier.next_card();
        sync_in_game(pots, in_game, 3);
        if (cirkle(pots, in_game, 3)) {
            return;
        }
        table_cards[3] = croupier.next_card();
        sync_in_game(pots, in_game, 4);
        if (cirkle(pots, in_game, 4)) {
            return;
        }
        table_cards[4] = croupier.next_card();
        sync_in_game(pots, in_game, 5);
        if (cirkle(pots, in_game, 5)) {
            return;
        }
        give_to_winner(pots);
    }

    bool cirkle(std::vector<Pot> &pots, InGame &in_game, size_t tc_amount) {
        size_t player_now = dealer;
        size_t max = 0;
        if (tc_amount == 0) {
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
        sync_in_game(pots, in_game, tc_amount);
        while (player_now != last_bet) {
            if (players[player_now].first.balance > 0 && in_game.players_in_game[player_now]) {
                Action action = get_action(players[player_now].second, max);
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
                        std::min(action.additional_info, players[player_now].first.balance));
                    last_bet = player_now;
                }
            }
            sync_in_game(pots, in_game, tc_amount);
            player_now = (player_now + 1) % players.size();
        }

        if (in_game.amount == 1) {
            size_t pos = std::find(in_game.players_in_game.begin(), in_game.players_in_game.end(), true) -
                         in_game.players_in_game.begin();
            for (size_t i = 0; i < players.size(); ++i) {
                if (i == pos) {
                    continue;
                }
                players[pos].first.from_pot(players[i].first.infront);
                players[i].first.to_pot(players[i].first.infront);
            }
            for (auto &pot : pots) {
                players[pos].first.from_pot(pot.money);
            }
            return true;
        }

        while (std::any_of(players.begin(), players.end(), [](auto &player) { return player.first.balance > 0; })) {
            std::vector<bool> players_for_pot(players.size(), false);
            size_t money = 0;
            size_t min_infront = SIZE_MAX;
            std::for_each(players.begin(), players.end(), [&min_infront](auto &player) {
                if (player.first.infront < min_infront && player.first.infront > 0) {
                    min_infront = player.first.infront;
                }
            });

            for (size_t i = 0; i < players.size(); ++i) {
                if (players[i].first.infront > 0) {
                    money += min_infront;
                    players[i].first.to_pot(min_infront);
                    if (in_game.players_in_game[i]) {
                        players_for_pot[i] = true;
                    }
                }
            }
            pots.emplace_back(players_for_pot, money);
        }
        return false;
    }

    void sync_after_game() {
        for (auto &player : players) {
            send_start_preinfo(player.second);
            for (auto &another_player : players) {
                send_players_info(player.second, another_player.second, another_player.first.balance);
            }
        }
    }

    void sync_in_game(std::vector<Pot> const &pots, InGame const &in_game, size_t tc_amount) {
        for (auto &player : players) {
            send_deck_info(player.second, tc_amount);
            for (auto &another_player : players) {
                send_players_info(player.second, another_player.second, another_player.first.balance,
                                  another_player.first.infront);
            }
        }
    }

    static Action get_action(ID player, size_t max) {
        sf::Packet package;
        package << sf::String("Action") << sf::Uint64(max);
        player->send(package);
        package.clear();
        player->receive(package);
        sf::String ans;
        sf::Uint64 add;
        package >> ans;
        if (ans == "Call") {
            return Action{ActionType::call, 0};
        }
        if (ans == "Check") {
            return Action{ActionType::check, 0};
        }
        if (ans == "Fold") {
            return Action{ActionType::fold, 0};
        }
        if (ans == "Raise") {
            package >> add;
            return Action{ActionType::raise, add};
        }
    }

    void send_start_preinfo(ID player) {
        sf::Packet package;
        package << sf::String("New Game") << sf::Uint64(players.size()) << sf::Uint64(dealer);
        player->send(package);
    }

    void send_deck_info(ID player, size_t tc_amount) {
        sf::Packet package;
        package << sf::String("Table Cards") << sf::Uint64(tc_amount);
        for (size_t i = 0; i < tc_amount; ++i) {
            package << sf::Uint64(suit_uint(table_cards[i].suit)) << sf::Uint64(rank_uint(table_cards[i].rank));
        }
        player->send(package);
    }

    static void send_players_info(ID player, ID another_player, size_t balance, size_t in_front = 0) {
        sf::Packet package;
        package << sf::String("Player Info") << sf::String(another_player->getRemoteAddress().toString())
                << another_player->getRemotePort() << sf::Uint64(balance) << sf::Uint64(in_front);
        player->send(package);
    }

    static void send_money(ID player, size_t money) {
        sf::Packet package;
        package << sf::String("Give Money") << sf::Uint64(money);
        player->send(package);
    }

    static void goodbye(ID player) {
        sf::Packet package;
        package << sf::String("Goodbye");
        player->send(package);
        player->disconnect();
    }
};

#endif  // POKER_TABLE_H
