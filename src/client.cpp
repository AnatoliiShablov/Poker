#include <cmath>
#include <thread>
#include <mutex>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "lib/Deck.h"
#include "lib/Signals.h"
#include "lib/Player.h"

struct Field {
    inline static const size_t width_ = 800;
    inline static const size_t height_ = 600;

    inline static const float ellipse_hw_ = 300;
    inline static const float ellipse_hh_ = 200;

    inline static const float attention_y_ = 100;
    inline static const size_t attention_s_ = 40;

    inline static const float win_x_ = 100;
    inline static const float lost_x_ = 100;

    inline static const float bar_x_ = 625;
    inline static const float bar_y_ = 550;
    inline static const float cirkle_x_min_ = 630;
    inline static const float cirkle_x_max_ = 790;
    inline static const float cirkle_y_ = 550;


    inline static const float button_xs_ = 50;
    inline static const float button_ys_ = 20;
    inline static const float button_y_ = 570;
    inline static const float button1_x_ = 630;
    inline static const float button2_x_ = 685;
    inline static const float button3_x_ = 740;

    sf::RenderWindow window;

    Signal last_signal;
    sf::Uint64 additional_info_signal;
    Action action_saver;
    std::pair<std::array<Card, 5>, sf::Uint8> table_cards;
    std::vector<std::pair<sf::String, Player>> players;
    sf::Uint8 main_player_pos;
    sf::Uint8 dealer;
    sf::Uint8 active_player;

    std::vector<sf::Uint64> pots;

    sf::Font main_font;
    sf::Font attention_font;

    std::mutex refresh_data;
public:
    Field() : window(sf::VideoMode(width_, height_), "Poker Client"), last_signal{}, additional_info_signal{},
              action_saver{}, table_cards{{}, 0}, players{}, main_player_pos{}, dealer{}, active_player{},
              pots{}, main_font{}, attention_font{} {
        main_font.loadFromFile("../data/fonts/main.ttf");
        main_font.loadFromFile("../data/fonts/attention.ttf");
    }


    void draw(bool display = true) {
        struct player_data {
            sf::Text name;
            sf::Text balance;
            sf::Text infront;
            sf::Sprite left_card;
            sf::Sprite right_card;
        };
        std::vector<player_data> players_coord(players.size());
        for (size_t i = 0; i < players.size(); ++i) {
            players_coord[i].name.setFont(main_font);
            players_coord[i].name.setString(players[i].first);
            players_coord[i].name.setFillColor(active_player == i ? sf::Color::Yellow : sf::Color::Red);
            players_coord[i].name.setCharacterSize(30);
            players_coord[i].balance.setFont(main_font);
            players_coord[i].balance.setString(std::to_string(players[i].second.balance));
            players_coord[i].balance.setFillColor(sf::Color::Green);
            players_coord[i].balance.setCharacterSize(30);
            players_coord[i].infront.setFont(main_font);
            players_coord[i].infront.setString(std::to_string(players[i].second.infront));
            players_coord[i].infront.setFillColor(sf::Color::Green);
            players_coord[i].infront.setCharacterSize(30);
            players_coord[i].left_card = players[i].second.show_cards().lhs.getSprite();
            players_coord[i].right_card = players[i].second.show_cards().rhs.getSprite();
        }

        auto diff = static_cast<float>(2.0 * M_PI / players_coord.size());
        float fi = M_PI_2 - diff * static_cast<float>(main_player_pos);

        for (auto &player_coord : players_coord) {
            fi += diff;
            float r = (ellipse_hh_ * ellipse_hw_) /
                      std::sqrt(ellipse_hh_ * ellipse_hh_ * std::cos(fi) * std::cos(fi) +
                                ellipse_hw_ * ellipse_hw_ * std::sin(fi) * std::sin(fi));

            player_coord.name.setPosition(static_cast<float>(width_ / 2.0 + r * std::cos(fi) - 45.0f),
                                          static_cast<float>(height_ / 2.0 + r * std::sin(fi) - 44.0f));
            player_coord.balance.setPosition(static_cast<float>(width_ / 2.0 + r * std::cos(fi) - 45.0f),
                                             static_cast<float>(height_ / 2.0 + r * std::sin(fi) - 36.0f));
            player_coord.infront.setPosition(static_cast<float>(width_ / 2.0 + r * std::cos(fi) - 45.0f),
                                             static_cast<float>(height_ / 2.0 + r * std::sin(fi) - 28.0f));
            player_coord.left_card.setPosition(static_cast<float>(width_ / 2.0 + r * std::cos(fi) - 45.0f),
                                               static_cast<float>(height_ / 2.0 + r * std::sin(fi) - 20.0f));
            player_coord.right_card.setPosition(static_cast<float>(width_ / 2.0 + r * std::cos(fi) + 1.0f),
                                               static_cast<float>(height_ / 2.0 + r * std::sin(fi) - 20.0f));
        }
        window.clear(sf::Color::Black);
        for (auto &player_coord : players_coord) {
            window.draw(player_coord.name);
            window.draw(player_coord.balance);
            window.draw(player_coord.infront);
            window.draw(player_coord.left_card);
            window.draw(player_coord.right_card);
        }
        if (display) {
            window.display();
        }
    }

    void draw_winners(std::vector<sf::Uint8> const &poses) {
        std::vector<sf::Text> winners(players.size());
        for (size_t i = 0; i < players.size(); ++i) {
            winners[i].setFont(main_font);
            winners[i].setString(std::any_of(poses.begin(), poses.end(),
                                             [i](auto t) { return t == i; }) ? players[i].first : "");
            winners[i].setFillColor(sf::Color::Yellow);
            winners[i].setCharacterSize(30);
        }

        auto diff = static_cast<float>(2.0 * M_PI / players.size());
        float fi = M_PI_2 - diff * static_cast<float>(main_player_pos);

        for (auto &winner : winners) {
            fi += diff;
            float r = (ellipse_hh_ * ellipse_hw_) /
                      std::sqrt(ellipse_hh_ * ellipse_hh_ * std::cos(fi) * std::cos(fi) +
                                ellipse_hw_ * ellipse_hw_ * std::sin(fi) * std::sin(fi));

            winner.setPosition(static_cast<float>(width_ / 2.0 + r * std::cos(fi) - 45.0f),
                               static_cast<float>(height_ / 2.0 + r * std::sin(fi)));
        }
        window.clear(sf::Color::Black);
        for (auto &winner : winners) {
            window.draw(winner);
        }
        window.display();
    }

    void draw_signal() {
        sf::Text text("", attention_font, attention_s_);
        text.setPosition(0.0f, attention_y_);
        text.setFillColor(sf::Color::Red);
        switch (last_signal()) {
            case Signal::Win:
                text.setString("Winner");
                text.move(win_x_, 0.0f);
                break;
            case Signal::Lost:
                text.setString("Looser");
                text.move(lost_x_, 0.0f);
                break;
            default:
                return;
        }
        window.clear(sf::Color::White);
        window.draw(text);
        window.display();
    }

    void draw_cr() {
        draw(false);

        sf::Texture check_texture;
        check_texture.loadFromFile("../data/buttons/check.png");
        sf::Texture raise_texture;
        raise_texture.loadFromFile("../data/buttons/raise.png");
        sf::Texture bar_texture;
        bar_texture.loadFromFile("../data/buttons/bar.png");
        sf::Texture cirkle_texture;
        cirkle_texture.loadFromFile("../data/buttons/cirkle.png");

        sf::Sprite check_button(check_texture);
        check_button.setPosition(button1_x_, button_y_);

        sf::Sprite raise_button(raise_texture);
        raise_button.setPosition(button2_x_, button_y_);

        sf::Sprite bar(bar_texture);
        bar.setPosition(bar_x_, bar_y_);

        sf::Sprite cirkle(cirkle_texture);
        cirkle.setPosition(cirkle_x_min_ + (cirkle_x_max_ - cirkle_x_min_) *
                                           (static_cast<float> (action_saver.additional_info) /
                                            static_cast<float>(players[main_player_pos].second.balance)), cirkle_y_);
        sf::Text money;
        money.setFont(attention_font);
        money.setCharacterSize(8);
        money.setFillColor(sf::Color::Transparent);
        money.setString(std::to_string(action_saver.additional_info));
        money.setPosition(cirkle_x_min_ + (cirkle_x_max_ - cirkle_x_min_) / 2.0f, cirkle_y_ - 10.0f);
        window.draw(check_button);
        window.draw(raise_button);
        window.draw(bar);
        window.draw(cirkle);
        window.draw(money);
        window.display();
    }

    void draw_crf() {
        draw(false);
        sf::Texture call_texture;
        call_texture.loadFromFile("../data/buttons/call.png");
        sf::Texture raise_texture;
        raise_texture.loadFromFile("../data/buttons/raise.png");
        sf::Texture fold_texture;
        fold_texture.loadFromFile("../data/buttons/fold.png");
        sf::Texture bar_texture;
        bar_texture.loadFromFile("../data/buttons/bar.png");
        sf::Texture cirkle_texture;
        cirkle_texture.loadFromFile("../data/buttons/cirkle.png");

        sf::Sprite check_button(call_texture);
        check_button.setPosition(button1_x_, button_y_);

        sf::Sprite raise_button(raise_texture);
        raise_button.setPosition(button2_x_, button_y_);

        sf::Sprite fold_button(fold_texture);
        fold_button.setPosition(button3_x_, button_y_);

        sf::Sprite bar(bar_texture);
        bar.setPosition(bar_x_, bar_y_);

        sf::Sprite cirkle(cirkle_texture);
        cirkle.setPosition(cirkle_x_min_ + (cirkle_x_max_ - cirkle_x_min_) *
                                           static_cast<float> (action_saver.additional_info) /
                                           static_cast<float>(players[main_player_pos].second.balance), cirkle_y_);
        sf::Text money;
        money.setFont(attention_font);
        money.setCharacterSize(8);
        money.setFillColor(sf::Color::Transparent);
        money.setString(std::to_string(action_saver.additional_info));
        money.setPosition(cirkle_x_min_ + (cirkle_x_max_ - cirkle_x_min_) / 2.0f, cirkle_y_ - 10.0f);

        window.draw(check_button);
        window.draw(raise_button);
        window.draw(fold_button);
        window.draw(bar);
        window.draw(cirkle);
        window.draw(money);
        window.display();
    }

    void draw_cf() {
        draw(false);
        sf::Texture call_texture;
        call_texture.loadFromFile("../data/buttons/call.png");
        sf::Texture fold_texture;
        fold_texture.loadFromFile("../data/buttons/fold.png");

        sf::Sprite call_button(call_texture);
        call_button.setPosition(button1_x_, button_y_);

        sf::Sprite fold_button(fold_texture);
        fold_button.setPosition(button3_x_, button_y_);

        window.draw(call_button);
        window.draw(fold_button);
        window.display();
    }
};

void package_reader(sf::TcpSocket *server_socket, Field &field) {
    Signal signal = Signal::NaI;
    while (signal != Signal::Lost && signal != Signal::Win) {
        sf::Packet package;
        server_socket->receive(package);
        package >> signal;
        switch (signal()) {
            case Signal::WaitForActionCheckRaise: {
                field.draw_cr();
                std::lock_guard<std::mutex> lock(field.refresh_data);
                field.last_signal = signal;
                package >> field.additional_info_signal;
                break;
            }
            case Signal::WaitForActionCallRaiseFold: {
                field.draw_crf();
                std::lock_guard<std::mutex> lock(field.refresh_data);
                field.last_signal = signal;
                package >> field.additional_info_signal;
                break;
            }
            case Signal::WaitForActionCallFold: {
                field.draw_cf();
                std::lock_guard<std::mutex> lock(field.refresh_data);
                field.last_signal = signal;
                package >> field.additional_info_signal;
                break;
            }
            case Signal::NewGameInfo: {
                sf::Uint8 amount;
                package >> amount;
                std::lock_guard<std::mutex> lock(field.refresh_data);
                package >> field.dealer;
                field.pots.clear();
                field.table_cards.second = 0;
                field.players.clear();
                for (sf::Uint8 i = 0; i < amount; ++i) {
                    sf::String name;
                    sf::Uint64 balance;
                    package >> name >> balance;
                    if (name == "Me") {
                        field.main_player_pos = i;
                    }
                    field.players.emplace_back(name, Player(balance));
                }
                Player::Hand hand;
                package >> hand;
                field.players[field.main_player_pos].second.retake(hand);
                field.last_signal = signal;
                break;
            }
            case Signal::Win:
            case Signal::Lost: {
                std::lock_guard<std::mutex> lock(field.refresh_data);
                field.last_signal = signal;
                field.draw_signal();
                break;
            }
            case Signal::ChangeTableCards: {
                std::lock_guard<std::mutex> lock(field.refresh_data);
                package >> field.table_cards.second;
                for (sf::Uint8 i = 0; i < field.table_cards.second; ++i) {
                    package >> field.table_cards.first[i];
                }
                field.last_signal = signal;
                break;
            }
            case Signal::ChangeActivePlayer: {
                std::lock_guard<std::mutex> lock(field.refresh_data);
                package >> field.active_player;
                field.last_signal = signal;
                break;
            }
            case Signal::ChangeBalances: {
                std::lock_guard<std::mutex> lock(field.refresh_data);
                for (auto &player : field.players) {
                    package >> player.second.balance >> player.second.infront;
                }
                field.last_signal = signal;
                break;
            }
            case Signal::ChangePot: {
                sf::Uint8 amount;
                package >> amount;
                std::lock_guard<std::mutex> lock(field.refresh_data);
                field.pots.resize(amount);
                for (sf::Uint8 i = 0; i < amount; ++i) {

                    package >> field.pots[i];
                }
                field.last_signal = signal;
                break;
            }
            case Signal::ShowCards: {
                std::lock_guard<std::mutex> lock(field.refresh_data);
                Player::Hand buf;
                for (auto &player : field.players) {
                    package >> buf;
                    player.second.retake(buf);
                }
                field.last_signal = signal;
                break;
            }
            case Signal::ShowWinners: {
                sf::Uint8 amount;
                package >> amount;
                std::vector<sf::Uint8> poses(amount);
                for (sf::Uint8 i = 0; i < amount; ++i) {
                    package >> poses[0];
                }
                std::lock_guard<std::mutex> lock(field.refresh_data);
                field.draw_winners(poses);
                field.last_signal = signal;
                break;
            }
            case Signal::NaI:
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    sf::TcpSocket server_socket;
    if (server_socket.connect(sf::IpAddress::getLocalAddress(), 56789) != sf::Socket::Done) {
        std::cerr << "Bad";
        return 1;
    }

    Field field;
    std::thread reader(&package_reader, &server_socket, std::ref(field));
    while (field.window.isOpen()) {
        sf::Event event{};
        std::lock_guard<std::mutex> lock(field.refresh_data);
        switch (field.last_signal()) {
            case Signal::NewGameInfo:
            case Signal::ChangeTableCards:
            case Signal::ChangeActivePlayer:
            case Signal::ChangeBalances:
            case Signal::ChangePot:
            case Signal::ShowCards:
                field.draw();
                field.last_signal = Signal::NaI;
                break;
            default:
                break;
        }
        while (field.window.pollEvent(event)) {
            switch (field.last_signal()) {
                case Signal::WaitForActionCheckRaise: {
                    if (event.type == sf::Event::MouseButtonPressed) {
                        sf::Vector2i vec = sf::Mouse::getPosition(field.window);
                        sf::Vector2f mouse_pos(static_cast<float>(vec.x), static_cast<float >( vec.y));
                        std::cout << Field::button1_x_ << "<" << mouse_pos.x << "<"
                                  << Field::button1_x_ + Field::button_xs_ << std::endl;
                        std::cout << Field::button_y_ << "<" << mouse_pos.y << "<"
                                  << Field::button_y_ + Field::button_ys_ << std::endl;
                        if (mouse_pos.x >= Field::button1_x_ && mouse_pos.x <= Field::button1_x_ + Field::button_xs_ &&
                            mouse_pos.y >= Field::button_y_ && mouse_pos.y <= Field::button_y_ + Field::button_ys_) {
                            field.action_saver.type = Action::check;
                            sf::Packet package;
                            package << field.action_saver;
                            server_socket.send(package);
                            field.draw();
                            field.last_signal = Signal::NaI;
                            field.action_saver.type = Action::NaA;
                            field.action_saver.additional_info = 0;
                        }
                        if (mouse_pos.x >= Field::button2_x_ && mouse_pos.x <= Field::button2_x_ + Field::button_xs_ &&
                            mouse_pos.y >= Field::button_y_ && mouse_pos.y <= Field::button_y_ + Field::button_ys_) {
                            field.action_saver.type = Action::raise;
                            sf::Packet package;
                            package << field.action_saver;
                            server_socket.send(package);
                            field.draw();
                            field.last_signal = Signal::NaI;
                            field.action_saver.type = Action::NaA;
                            field.action_saver.additional_info = 0;
                        }
                        if (mouse_pos.x >= Field::cirkle_x_min_ && mouse_pos.x <= Field::cirkle_x_max_ &&
                            mouse_pos.y >= Field::cirkle_y_ - 5.0f && mouse_pos.y <= Field::button_y_ + 5.0f) {
                            field.action_saver.additional_info = static_cast<sf::Uint64>(
                                    field.players[field.main_player_pos].second.balance * (
                                            (mouse_pos.x - Field::cirkle_x_min_) /
                                            (Field::cirkle_x_max_ - Field::cirkle_x_min_)));
                            field.draw_cr();
                        }
                    }
                    break;
                }
                case Signal::WaitForActionCallRaiseFold: {
                    if (event.type == sf::Event::MouseButtonPressed) {
                        sf::Vector2i vec = sf::Mouse::getPosition(field.window);
                        sf::Vector2f mouse_pos(static_cast<float>(vec.x), static_cast<float >( vec.y));
                        std::cout << Field::button1_x_ << "<" << mouse_pos.x << "<"
                                  << Field::button1_x_ + Field::button_xs_ << std::endl;
                        std::cout << Field::button_y_ << "<" << mouse_pos.y << "<"
                                  << Field::button_y_ + Field::button_ys_ << std::endl;
                        if (mouse_pos.x >= Field::button1_x_ && mouse_pos.x <= Field::button1_x_ + Field::button_xs_ &&
                            mouse_pos.y >= Field::button_y_ && mouse_pos.y <= Field::button_y_ + Field::button_ys_) {
                            field.action_saver.type = Action::call;
                            sf::Packet package;
                            package << field.action_saver;
                            server_socket.send(package);
                            field.draw();
                            field.last_signal = Signal::NaI;
                            field.action_saver.type = Action::NaA;
                            field.action_saver.additional_info = 0;
                        }
                        if (mouse_pos.x >= Field::button2_x_ && mouse_pos.x <= Field::button2_x_ + Field::button_xs_ &&
                            mouse_pos.y >= Field::button_y_ && mouse_pos.y <= Field::button_y_ + Field::button_ys_) {
                            field.action_saver.type = Action::raise;
                            sf::Packet package;
                            package << field.action_saver;
                            server_socket.send(package);
                            field.draw();
                            field.last_signal = Signal::NaI;
                            field.action_saver.type = Action::NaA;
                            field.action_saver.additional_info = 0;
                        }
                        if (mouse_pos.x >= Field::button3_x_ && mouse_pos.x <= Field::button3_x_ + Field::button_xs_ &&
                            mouse_pos.y >= Field::button_y_ && mouse_pos.y <= Field::button_y_ + Field::button_ys_) {
                            field.action_saver.type = Action::fold;
                            sf::Packet package;
                            package << field.action_saver;
                            server_socket.send(package);
                            field.draw();
                            field.last_signal = Signal::NaI;
                            field.action_saver.type = Action::NaA;
                            field.action_saver.additional_info = 0;
                        }
                        if (mouse_pos.x >= Field::cirkle_x_min_ && mouse_pos.x <= Field::cirkle_x_max_ &&
                            mouse_pos.y >= Field::cirkle_y_ - 5.0f && mouse_pos.y <= Field::button_y_ + 5.0f) {
                            field.action_saver.additional_info = static_cast<sf::Uint64>(
                                    field.players[field.main_player_pos].second.balance * (
                                            (mouse_pos.x - Field::cirkle_x_min_) /
                                            (Field::cirkle_x_max_ - Field::cirkle_x_min_)));
                            field.draw_crf();
                        }
                    }
                    break;
                }
                case Signal::WaitForActionCallFold: {
                    if (event.type == sf::Event::MouseButtonPressed) {
                        sf::Vector2i vec = sf::Mouse::getPosition(field.window);
                        sf::Vector2f mouse_pos(static_cast<float>(vec.x), static_cast<float >( vec.y));
                        std::cout << Field::button1_x_ << "<" << mouse_pos.x << "<"
                                  << Field::button1_x_ + Field::button_xs_ << std::endl;
                        std::cout << Field::button_y_ << "<" << mouse_pos.y << "<"
                                  << Field::button_y_ + Field::button_ys_ << std::endl;
                        if (mouse_pos.x >= Field::button1_x_ && mouse_pos.x <= Field::button1_x_ + Field::button_xs_ &&
                            mouse_pos.y >= Field::button_y_ && mouse_pos.y <= Field::button_y_ + Field::button_ys_) {
                            field.action_saver.type = Action::call;
                            sf::Packet package;
                            package << field.action_saver;
                            server_socket.send(package);
                            field.draw();
                            field.last_signal = Signal::NaI;
                            field.action_saver.type = Action::NaA;
                            field.action_saver.additional_info = 0;
                        }
                        if (mouse_pos.x >= Field::button3_x_ && mouse_pos.x <= Field::button3_x_ + Field::button_xs_ &&
                            mouse_pos.y >= Field::button_y_ && mouse_pos.y <= Field::button_y_ + Field::button_ys_) {
                            field.action_saver.type = Action::fold;
                            sf::Packet package;
                            package << field.action_saver;
                            server_socket.send(package);
                            field.draw();
                            field.last_signal = Signal::NaI;
                            field.action_saver.type = Action::NaA;
                            field.action_saver.additional_info = 0;
                        }
                    }
                    break;
                }
                case Signal::Win:
                case Signal::Lost:
                    if (event.type == sf::Event::Closed) {
                        field.window.close();
                    }
                    break;
                default:
                    break;
            }
        }
    }
    server_socket.disconnect();
    return 0;
}