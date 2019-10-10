#ifndef POKER_TABLE_H
#define POKER_TABLE_H

#include <algorithm>
#include <utility>
#include "Croupier.h"
#include "Player.h"
#include "SFML/Network.hpp"
#include "SFML/System.hpp"
#include "Signals.h"

class Table {
    typedef std::add_pointer_t<sf::TcpSocket> ID;

    struct Pot {
        std::vector<bool> variant;
        sf::Uint64 money;

        Pot(std::vector<bool> variant, sf::Uint64 money) : variant(std::move(variant)), money(money) {}
    };

    struct InGame {
        std::vector<bool> alive;
        sf::Uint8 amount;

        explicit InGame(sf::Uint8 amount) : alive(amount, true), amount{amount} {}
    };

    struct HCPower {
        sf::Uint32 power;

        explicit HCPower(std::array<Card, 7> const cards) noexcept : power{0} {
            std::array<bool, 13> a_diamonds{};
            std::array<bool, 13> a_clubs{};
            std::array<bool, 13> a_hearts{};
            std::array<bool, 13> a_spades{};
            std::array<uint8_t, 13> a_no_suit{};

            for (Card card : cards) {
                if (card.suit == Suit::diamonds) {
                    a_diamonds[card.rank.to_uint8()] = true;
                }
                if (card.suit == Suit::clubs) {
                    a_clubs[card.rank.to_uint8()] = true;
                }
                if (card.suit == Suit::hearts) {
                    a_hearts[card.rank.to_uint8()] = true;
                }
                if (card.suit == Suit::spades) {
                    a_spades[card.rank.to_uint8()] = true;
                }
                ++a_no_suit[card.rank.to_uint8()];
            }
            sf::Uint32 result{0};

            if (result = flush_straight(a_diamonds); result) {
                power = result;
                return;
            }
            if (result = flush_straight(a_clubs); result) {
                power = result;
                return;
            }
            if (result = flush_straight(a_hearts); result) {
                power = result;
                return;
            }
            if (result = flush_straight(a_spades); result) {
                power = result;
                return;
            }

            if (result = four_of_a_kind(a_no_suit); result) {
                power = result;
                return;
            }

            if (result = full_house(a_no_suit); result) {
                power = result;
                return;
            }

            if (result = flush(a_diamonds); result) {
                power = result;
                return;
            }
            if (result = flush(a_clubs); result) {
                power = result;
                return;
            }
            if (result = flush(a_hearts); result) {
                power = result;
                return;
            }
            if (result = flush(a_spades); result) {
                power = result;
                return;
            }

            if (result = straight(a_no_suit); result) {
                power = result;
                return;
            }

            if (result = three_of_a_kind(a_no_suit); result) {
                power = result;
                return;
            }

            if (result = two_pairs(a_no_suit); result) {
                power = result;
                return;
            }

            if (result = pair(a_no_suit); result) {
                power = result;
                return;
            }

            if (result = high_card(a_no_suit); result) {
                power = result;
                return;
            }
        }

        static sf::Uint32 flush_straight(std::array<bool, 13> const &mask) {
            sf::Uint32 row{0};
            for (size_t i = 13; i--;) {
                if (mask[i]) {
                    if (++row == 5) {
                        return 0xF0000000 | UINT32_C(1) << (18 + i);
                    }
                } else {
                    row = 0;
                }
            }
            if (row == 4 && mask[12]) {
                return 0xF0000000 | UINT32_C(1) << 17u;
            }
            return 0;
        }

        static sf::Uint32 four_of_a_kind(std::array<uint8_t, 13> const &mask) {
            sf::Uint32 four{15};
            sf::Uint32 additional{15};
            for (size_t i = 13; i--;) {
                if (mask[i] == 4 && four == 15) {
                    four = i;
                    continue;
                }
                if (mask[i] && additional == 15) {
                    additional = i;
                }
            }

            if (four != 15) {
                return 0xE0000000 | UINT32_C(1) << (14 + four) | UINT32_C(1) << additional;
            }
            return 0;
        }

        static sf::Uint32 full_house(std::array<uint8_t, 13> const &mask) {
            sf::Uint32 three = 15;
            sf::Uint32 pair = 15;
            for (size_t i = 13; i--;) {
                if (mask[i] == 3 && three == 15) {
                    three = i;
                    continue;
                }
                if (mask[i] >= 2 && pair == 15) {
                    pair = i;
                }
            }
            if (three != 15 && pair != 15) {
                return 0xD0000000 | UINT32_C(1) << (14 + three) | UINT32_C(1) << pair;
            }
            return 0;
        }

        static sf::Uint32 flush(std::array<bool, 13> const &mask) {
            sf::Uint32 amount{0};
            sf::Uint32 result{0xC0000000};
            for (size_t i = 13; i--;) {
                if (mask[i]) {
                    result |= UINT32_C(1) << (14 + i);
                    if (++amount == 5) {
                        return result;
                    }
                }
            }
            return 0;
        }

        static sf::Uint32 straight(std::array<uint8_t, 13> const &mask) {
            sf::Uint32 row{0};
            for (size_t i = 13; i--;) {
                if (mask[i]) {
                    if (++row == 5) {
                        return 0xB0000000 | UINT32_C(1) << (18 + i);
                    }
                } else {
                    row = 0;
                }
            }
            if (row == 4 && mask[12]) {
                return 0xB0000000 | UINT32_C(1) << 17u;
            }
            return 0;
        }

        static sf::Uint32 three_of_a_kind(std::array<uint8_t, 13> const &mask) {
            sf::Uint32 three{15};
            sf::Uint32 additional1{15}, additional2{15};
            for (size_t i = 13; i--;) {
                if (mask[i] == 3 && three == 15) {
                    three = i;
                    continue;
                }
                if (mask[i]) {
                    if (additional1 == 15) {
                        additional1 = i;
                        continue;
                    }
                    if (additional2 == 15) {
                        additional2 = i;
                    }
                }
            }
            if (three != 15) {
                return 0xA0000000 | UINT32_C(1) << (14 + three) | UINT32_C(1) << additional1 |
                       UINT32_C(1) << additional2;
            }
            return 0;
        }

        static sf::Uint32 two_pairs(std::array<uint8_t, 13> const &mask) {
            sf::Uint32 pair1{15}, pair2{15};
            sf::Uint32 additional{15};
            for (size_t i = 13; i--;) {
                if (mask[i] == 2) {
                    if (pair1 == 15) {
                        pair1 = i;
                        continue;
                    }
                    if (pair2 == 15) {
                        pair2 = i;
                        continue;
                    }
                }
                if (mask[i] && additional == 15) {
                    additional = i;
                }
            }
            if (pair1 != 15 && pair2 != 15) {
                return 0x90000000 | UINT32_C(1) << (14 + pair1) | UINT32_C(1) << (14 + pair2) |
                       UINT32_C(1) << additional;
            }
            return 0;
        }

        static sf::Uint32 pair(std::array<uint8_t, 13> const &mask) {
            sf::Uint32 pair{15};
            sf::Uint32 additional1{15}, additional2{15}, additional3{15};
            for (size_t i = 13; i--;) {
                if (mask[i] == 2 && pair == 15) {
                    pair = i;
                    continue;
                }
                if (mask[i]) {
                    if (additional1 == 15) {
                        additional1 = i;
                        continue;
                    }

                    if (additional2 == 15) {
                        additional2 = i;
                        continue;
                    }

                    if (additional3 == 15) {
                        additional3 = i;
                    }
                }
            }
            if (pair != 15) {
                return 0x80000000 | UINT32_C(1) << (14 + pair) | UINT32_C(1) << additional1 |
                       UINT32_C(1) << additional2 | UINT32_C(1) << additional3;
            }
            return 0;
        }

        static sf::Uint32 high_card(std::array<uint8_t, 13> const &mask) {
            sf::Uint32 amount{0};
            sf::Uint32 result{0x70000000};
            for (size_t i = 13; i--;) {
                if (mask[i]) {
                    result |= UINT32_C(1) << (14 + i);
                    if (++amount == 5) {
                        return result;
                    }
                }
            }
            return 0;
        }
    };

    Croupier croupier;
    std::vector<std::pair<Player, ID>> players;

public:
    sf::Uint8 dealer{};
    sf::Uint64 small_blind;
    std::array<Card, 5> table_cards{};

    template<typename InputIterator>
    Table(InputIterator beginID, InputIterator endID, sf::Uint64 start_balance, sf::Uint64 small_blind)
            : small_blind{small_blind} {
        for (auto it = beginID; it != endID; ++it) {
            players.emplace_back(Player(start_balance), *it);
        }
    }

    void tournament() {
        while (players.size() > 1) {
            game();
            for (size_t i = 0; i < players.size(); ++i) {
                if (players[i].first.balance == 0 || players[i].second->getRemoteAddress() == sf::IpAddress::None) {
                    send_info_to_looser(players[i].second);
                    players.erase(players.begin() + i);
                    if (i <= dealer) {
                        if (dealer) {
                            --dealer;
                        } else {
                            dealer = players.size() - 1;
                        }
                    }
                }
            }
            dealer = (dealer + 1) % players.size();
        }
        send_info_to_winner(players.front().second);
    }

    void game() {
        croupier.shuffle();
        for (auto &player : players) {
            player.first.retake(Player::Hand{croupier.next_card(), croupier.next_card()});
        }
        send_new_game_info();
        std::vector<Pot> pots;
        InGame in_game(players.size());
        if (cirkle(pots, in_game, 0)) {
            return;
        }
        table_cards[0] = croupier.next_card();
        table_cards[1] = croupier.next_card();
        table_cards[2] = croupier.next_card();
        send_table_cards(3);
        if (cirkle(pots, in_game, 3)) {
            return;
        }
        table_cards[3] = croupier.next_card();
        send_table_cards(4);
        if (cirkle(pots, in_game, 4)) {
            return;
        }
        table_cards[4] = croupier.next_card();
        send_table_cards(5);
        if (cirkle(pots, in_game, 5)) {
            return;
        }
        show_cards(in_game);
        give_to_winner(pots);
    }

    bool cirkle(std::vector<Pot> &pots, InGame &in_game, size_t tc_amount) {
        sf::Uint8 player_now = dealer;
        sf::Uint64 max = 0;
        if (tc_amount == 0) {
            player_now = (player_now + 1) % players.size();
            if (players[player_now].first.balance <= small_blind) {
                players[player_now].first.infront = players[player_now].first.balance;
                players[player_now].first.balance = 0;
            } else {
                players[player_now].first.infront = small_blind;
                players[player_now].first.balance -= small_blind;
            }
            player_now = (player_now + 1) % players.size();
            if (players[player_now].first.balance <= small_blind * 2) {
                players[player_now].first.infront = players[player_now].first.balance;
                players[player_now].first.balance = 0;
            } else {
                players[player_now].first.infront = small_blind * 2;
                players[player_now].first.balance -= small_blind * 2;
            }
            max = small_blind * 2;
        }
        sf::Uint8 done = 0;
        player_now = (player_now + 1) % players.size();
        while (done != in_game.amount) {
            if (players[player_now].first.balance > 0 && in_game.alive[player_now]) {
                send_new_balances_and_activity(in_game);
                send_active_player(player_now);
                Action action = get_action(player_now, max);
                if (action == Action::call) {
                    if (players[player_now].first.balance + players[player_now].first.infront <= max) {
                        players[player_now].first.infront += players[player_now].first.balance;
                        players[player_now].first.balance = 0;
                    } else {
                        players[player_now].first.balance -= max - players[player_now].first.infront;
                        players[player_now].first.infront = max;
                    }
                }

                if (action == Action::fold) {
                    in_game.alive[player_now] = false;
                    --in_game.amount;
                    if (in_game.amount == 1) {
                        break;
                    }
                }

                if (action == Action::raise) {
                    max = action.additional_info;
                    players[player_now].first.balance -= max - players[player_now].first.infront;
                    players[player_now].first.infront = max;
                    done = 0;
                }
            }
            if (in_game.alive[player_now]) {
                ++done;
            }
            player_now = (player_now + 1) % players.size();
        }

        if (in_game.amount == 1) {
            sf::Uint8 winner = std::find(in_game.alive.begin(), in_game.alive.end(), true) - in_game.alive.begin();
            send_winners(std::vector<sf::Uint8>(1, winner));
            for (auto &player : players) {
                players[winner].first.balance += player.first.infront;
                player.first.infront = 0;
            }
            for (auto &pot : pots) {
                players[winner].first.balance += pot.money;
            }
            sf::sleep(sf::seconds(5));
            return true;
        }

        while (std::any_of(players.begin(), players.end(), [](auto &player) { return player.first.infront > 0; })) {
            std::vector<bool> players_for_pot(players.size(), false);
            sf::Uint64 money = 0;
            sf::Uint64 min_infront = 0;
            std::for_each(players.begin(), players.end(), [&min_infront](auto &player) {
                if (min_infront == 0 || (0 < player.first.infront && player.first.infront < min_infront)) {
                    min_infront = player.first.infront;
                }
            });

            for (size_t i = 0; i < players.size(); ++i) {
                if (players[i].first.infront > 0) {
                    money += min_infront;
                    players[i].first.infront -= min_infront;
                    if (in_game.alive[i]) {
                        players_for_pot[i] = true;
                    }
                }
            }
            if (!pots.empty() && pots.back().variant == players_for_pot) {
                pots.back().money += money;
            } else {
                pots.emplace_back(players_for_pot, money);
            }
        }
        send_pot_changes(pots);
        return false;
    }

    void give_to_winner(std::vector<Pot> const &pots) {
        std::vector<HCPower> powers;
        for (auto &player : players) {
            powers.emplace_back(std::array<Card, 7>{player.first.show_cards().lhs, player.first.show_cards().rhs,
                                                    table_cards[0], table_cards[1], table_cards[2], table_cards[3],
                                                    table_cards[4]});
        }
        for (Pot const &pot : pots) {
            std::vector<sf::Uint8> winners;
            winners.push_back(std::find(pot.variant.begin(), pot.variant.end(), true) - pot.variant.begin());
            for (size_t i = winners.front() + 1; i < players.size(); ++i) {
                if (!pot.variant[i]) {
                    continue;
                }
                if (powers[i].power == powers[winners.front()].power) {
                    winners.push_back(i);
                } else if (powers[i].power > powers[winners.front()].power) {
                    winners.resize(1, i);
                }
            }
            send_winners(winners);
            for (auto winner : winners) {
                players[winner].first.balance += pot.money / winners.size();
            }
            sf::sleep(sf::seconds(5));
        }
    }

    static void send_info_to_looser(ID player) {
        sf::Packet package;
        package << Signal(Signal::Lost);
        send_package(player, package);
    }

    static void send_info_to_winner(ID player) {
        sf::Packet package;
        package << Signal(Signal::Win);
        send_package(player, package);
    }

    void send_new_game_info() {
        for (size_t i = 0; i < players.size(); ++i) {
            sf::Packet package;
            package << Signal(Signal::NewGameInfo);
            package << sf::Uint8(players.size()) << dealer;
            for (size_t j = 0; j < players.size(); ++j) {
                if (i == j) {
                    package << sf::String("Me");
                } else {
                    package << sf::String(std::to_string(players[j].second->getLocalPort()));
                }
                package << players[j].first.balance;
            }
            package << players[i].first.show_cards();
            send_package(players[i].second, package);
        }
    }

    void send_table_cards(sf::Uint8 tc_amount) {
        sf::Packet package;
        package << Signal(Signal::ChangeTableCards);
        package << tc_amount;
        for (sf::Uint8 i = 0; i < tc_amount; ++i) {
            package << table_cards[i];
        }
        for (auto &player : players) {
            send_package(player.second, package);
        }
    }

    void show_cards(InGame const &in_game) {
        for (size_t i = 0; i < players.size(); ++i) {
            sf::Packet package;
            package << Signal(Signal::ShowCards);
            for (size_t j = 0; j < players.size(); ++j) {
                if (in_game.alive[j] || i == j) {
                    package << players[j].first.show_cards();
                } else {
                    package << Player::Hand{};
                }
            }
            send_package(players[i].second, package);
        }
    }

    void send_new_balances_and_activity(InGame const &in_game) {
        sf::Packet package;
        package << Signal(Signal::ChangeBalances);
        for (auto &player : players) {
            package << player.first.balance << player.first.infront;
        }
        for (bool b : in_game.alive) {
            package << b;
        }
        for (auto &player : players) {
            send_package(player.second, package);
        }
    }

    void send_active_player(sf::Uint8 active) {
        sf::Packet package;
        package << Signal(Signal::ChangeActivePlayer) << active;
        for (auto &player : players) {
            send_package(player.second, package);
        }
    }

    Action get_action(sf::Uint8 player, sf::Uint64 max) {
        sf::Packet package;
        if (max == players[player].first.infront) {
            package << Signal(Signal::WaitForActionCheckRaise);
        } else if (max >= players[player].first.balance + players[player].first.infront) {
            package << Signal(Signal::WaitForActionCallFold);
        } else {
            package << Signal(Signal::WaitForActionCallRaiseFold);
        }
        send_package(players[player].second, package);
        sf::Packet input_package;
        while (true) {
            sf::Socket::Status status = players[player].second->receive(input_package);
            if (status == sf::Socket::Done) {
                break;
            }
            if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
                input_package.clear();
                input_package << Action(Action::fold);
                break;
            }
        }
        Action res;
        input_package >> res;
        return res;
    }

    void send_winners(std::vector<sf::Uint8> const &winners) {
        sf::Packet package;
        package << Signal(Signal::ShowWinners) << sf::Uint8(winners.size());
        for (sf::Uint8 winner : winners) {
            package << winner;
        }
        for (auto &player : players) {
            player.second->send(package);
        }
    }

    void send_pot_changes(std::vector<Pot> const &pots) {
        sf::Packet package;
        package << Signal(Signal::ChangePot) << sf::Uint8(pots.size());
        for (auto &pot : pots) {
            package << pot.money;
        }
        for (auto &player : players) {
            player.second->send(package);
        }
    }

    static void send_package(ID player, sf::Packet package) {
        while (true) {
            sf::Socket::Status status = player->send(package);
            if (status == sf::Socket::Done || status == sf::Socket::Disconnected || sf::Socket::Error) {
                break;
            }
        }
    }
};

#endif  // POKER_TABLE_H
