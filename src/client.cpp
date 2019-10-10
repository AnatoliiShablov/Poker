#include <cmath>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "SFML/Graphics.hpp"

#include "lib/Card.h"
#include "lib/Player.h"
#include "lib/Signals.h"

enum class Anchor {
    top_left, top_right, bottom_left, bottom_right, center
};

class Button {
    sf::Texture button_texture_normal;
    sf::Sprite button_sprite_normal;

    sf::Texture button_texture_pressed;
    sf::Sprite button_sprite_pressed;

    sf::Vector2f pos;
    Anchor anchor;
    bool is_pressed;

public:
    Button(sf::String const &name, sf::Vector2f pos, Anchor anchor = Anchor::top_left)
            : pos{pos}, anchor{anchor}, is_pressed{false} {
        button_texture_normal.loadFromFile(name + "_normal.png");
        button_sprite_normal.setTexture(button_texture_normal, true);
        button_sprite_normal.setOrigin({button_sprite_normal.getGlobalBounds().width / 2.0f,
                                        button_sprite_normal.getGlobalBounds().height / 2.0f});

        button_texture_pressed.loadFromFile(name + "_pressed.png");
        button_sprite_pressed.setTexture(button_texture_pressed, true);
        button_sprite_pressed.setOrigin({button_sprite_pressed.getGlobalBounds().width / 2.0f,
                                         button_sprite_pressed.getGlobalBounds().height / 2.0f});
    }

    bool press(sf::Vector2f mouse_pos) {
        if (button_sprite_normal.getGlobalBounds().contains(mouse_pos)) {
            return is_pressed = true;
        }
        return false;
    }

    bool clicked(sf::Vector2f mouse_pos) {
        if (!is_pressed) {
            return false;
        }
        is_pressed = false;
        return button_sprite_pressed.getGlobalBounds().contains(mouse_pos);
    }

    void operator()(sf::RenderWindow &window) {
        switch (anchor) {
            case Anchor::top_left: {
                button_sprite_normal.setPosition(pos);
                button_sprite_pressed.setPosition(pos);
                break;
            }
            case Anchor::top_right: {
                button_sprite_normal.setPosition(window.getSize().x + pos.x, pos.y);
                button_sprite_pressed.setPosition(window.getSize().x + pos.x, pos.y);
                break;
            }
            case Anchor::bottom_left: {
                button_sprite_normal.setPosition(pos.x, window.getSize().y + pos.y);
                button_sprite_pressed.setPosition(pos.x, window.getSize().y + pos.y);
                break;
            }
            case Anchor::bottom_right: {
                button_sprite_normal.setPosition(window.getSize().x + pos.x, window.getSize().y + pos.y);
                button_sprite_pressed.setPosition(window.getSize().x + pos.x, window.getSize().y + pos.y);
                break;
            }
            case Anchor::center: {
                button_sprite_normal.setPosition(window.getSize().x / 2.0f + pos.x, window.getSize().y / 2.0f + pos.y);
                button_sprite_pressed.setPosition(window.getSize().x / 2.0f + pos.x, window.getSize().y / 2.0f + pos.y);
                break;
            }
        }
        window.draw(is_pressed ? button_sprite_pressed : button_sprite_normal);
    }
};

class BarNumerated {
    sf::Texture point_texture_normal;
    sf::Sprite point_sprite_normal;

    sf::Texture point_texture_pressed;
    sf::Sprite point_sprite_pressed;

    sf::Texture bar_texture;
    sf::Sprite bar_sprite;

    sf::Font font;
    sf::Text number;

    sf::Uint64 now_number;

    sf::Vector2f pos;
    Anchor anchor;
    bool is_pressed;

    sf::Uint64 min_number{};
    sf::Uint64 max_number{};

public:
    void set_mm(sf::Uint64 min, sf::Uint64 max) {
        now_number = min_number = min;
        max_number = max;
    }

    BarNumerated(sf::String const &name, sf::Vector2f pos, Anchor anchor = Anchor::top_left)
            : pos{pos}, anchor{anchor} {
        point_texture_normal.loadFromFile(name + "_normal.png");
        point_sprite_normal.setTexture(point_texture_normal, true);
        point_sprite_normal.setOrigin(
                {point_sprite_normal.getGlobalBounds().width / 2.0f,
                 point_sprite_normal.getGlobalBounds().height / 2.0f});

        point_texture_pressed.loadFromFile(name + "_pressed.png");
        point_sprite_pressed.setTexture(point_texture_pressed, true);
        point_sprite_pressed.setOrigin({point_sprite_pressed.getGlobalBounds().width / 2.0f,
                                        point_sprite_pressed.getGlobalBounds().height / 2.0f});

        bar_texture.loadFromFile(name + "_bar.png");
        bar_sprite.setTexture(bar_texture, true);
        bar_sprite.setOrigin({bar_sprite.getGlobalBounds().width / 2.0f, bar_sprite.getGlobalBounds().height / 2.0f});

        now_number = max_number + (max_number - min_number) / 2;
        font.loadFromFile(name + "_font.ttf");

        number.setString(std::to_string(now_number));
        number.setFont(font);
        number.setCharacterSize(25);
        number.setFillColor(sf::Color::Blue);
        number.setOutlineThickness(0.1f);
        number.setOutlineColor(sf::Color::Green);
        number.setOrigin({number.getGlobalBounds().width / 2.0f, number.getGlobalBounds().height / 2.0f});

        is_pressed = false;
    }

    bool press(sf::Vector2f mouse_pos) {
        if (bar_sprite.getGlobalBounds().contains(mouse_pos)) {
            is_pressed = true;
            return moved(mouse_pos);
        }
        return false;
    }

    sf::Uint64 unpress() {
        is_pressed = false;
        return now_number;
    }

    bool moved(sf::Vector2f mouse_pos) {
        if (!is_pressed) {
            return false;
        }

        float left = bar_sprite.getGlobalBounds().left + 30.0f;
        float right = left + bar_sprite.getGlobalBounds().width - 60.0f;
        float x = mouse_pos.x < left ? left : (mouse_pos.x > right ? right : mouse_pos.x);
        now_number = min_number + static_cast<sf::Uint64>(static_cast<float>(max_number - min_number) *
                                                          ((x - left) / (right - left)));
        return true;
    }

    void operator()(sf::RenderWindow &window) {
        switch (anchor) {
            case Anchor::top_left: {
                bar_sprite.setPosition(pos);
                number.setPosition(pos.x, pos.y - 50.0f);
                break;
            }
            case Anchor::top_right: {
                bar_sprite.setPosition(window.getSize().x + pos.x, pos.y);
                number.setPosition(window.getSize().x + pos.x, pos.y - 50.0f);
                break;
            }
            case Anchor::bottom_left: {
                bar_sprite.setPosition(pos.y, window.getSize().y + pos.y);
                number.setPosition(pos.x, window.getSize().y + pos.y - 50.0f);
                break;
            }
            case Anchor::bottom_right: {
                bar_sprite.setPosition(window.getSize().x + pos.x, window.getSize().y + pos.y);
                number.setPosition(window.getSize().x + pos.x, window.getSize().y + pos.y - 50.0f);
                break;
            }
            case Anchor::center: {
                bar_sprite.setPosition(window.getSize().x / 2.0f + pos.x, window.getSize().y / 2.0f + pos.y);
                number.setPosition(window.getSize().x / 2.0f + pos.x, window.getSize().y / 2.0f + pos.y - 50.0f);
                break;
            }
        }
        float left = bar_sprite.getGlobalBounds().left + 30.0f;
        float right = left + bar_sprite.getGlobalBounds().width - 60.0f;
        float x = left + (right - left) * (static_cast<float>(now_number - min_number) /
                                           static_cast<float>(max_number - min_number));
        number.setString(std::to_string(now_number));
        point_sprite_normal.setPosition(x, bar_sprite.getPosition().y);
        point_sprite_pressed.setPosition(x, bar_sprite.getPosition().y);
        window.draw(bar_sprite);
        window.draw(is_pressed ? point_sprite_pressed : point_sprite_normal);
        window.draw(number);
    }
};

struct TableInfo {
    struct PlayerWithInfo {
        sf::String name{};
        Player data{};
        bool in_game{true};
        bool is_winner{false};
    };
    std::vector<PlayerWithInfo> players;
    sf::Uint8 main_player{};
    sf::Uint8 active_player{};
    sf::Uint8 dealer{};

    std::vector<Card> table_cards;
    std::vector<sf::Uint64> pots;

    Signal last_signal;
    bool reseted{};
    std::mutex refresh_data;

    void new_game(sf::Packet &package) {
        sf::Uint8 players_amount;
        package >> players_amount >> dealer;
        players.assign(players_amount, PlayerWithInfo{});
        for (sf::Uint8 i = 0; i < players_amount; ++i) {
            package >> players[i].name >> players[i].data.balance;
            if (players[i].name == "Me") {
                main_player = i;
            }
        }
        Player::Hand hand;
        package >> hand;
        players[main_player].data.retake(hand);
        table_cards.clear();
        pots.clear();
    }

    void refresh_table_cards(sf::Packet &package) {
        sf::Uint8 amount;
        package >> amount;
        table_cards.resize(amount);
        for (sf::Uint8 i = 0; i < amount; ++i) {
            package >> table_cards[i];
        }
    }

    void refresh_active_player(sf::Packet &package) { package >> active_player; }

    void refresh_balances_and_activity(sf::Packet &package) {
        for (auto &player : players) {
            package >> player.data.balance >> player.data.infront;
        }
        for (auto &player : players) {
            package >> player.in_game;
        }
    }

    void refresh_pots(sf::Packet &package) {
        sf::Uint8 amount;
        package >> amount;
        pots.resize(amount);
        for (auto &pot : pots) {
            package >> pot;
        }
    }

    void showing_cards(sf::Packet &package) {
        Player::Hand hand;
        for (auto &player : players) {
            package >> hand;
            player.data.retake(hand);
        }
    }

    void showing_winners(sf::Packet &package) {
        sf::Uint8 amount, num;
        package >> amount;

        for (auto &player : players) {
            player.is_winner = false;
        }
        for (sf::Uint8 i = 0; i < amount; ++i) {
            package >> num;
            players[num].is_winner = true;
        }
    }
};

struct RenderTable {
    TableInfo table;

    sf::RenderWindow window;

    sf::Font text_font;
    sf::Texture background_texture;
    sf::Sprite background;
    Button call_button;
    Button check_button;
    Button raise_button;
    Button fold_button;
    BarNumerated bar;

    RenderTable(sf::Uint32 width, sf::Uint32 height)
            : table{}, window{sf::VideoMode{width, height}, "Client"},
              call_button("../data/buttons/call_button", {-370.0f, -40.0f}, Anchor::bottom_right),
              check_button("../data/buttons/check_button", {-370.0f, -40.0f}, Anchor::bottom_right),
              raise_button("../data/buttons/raise_button", {-215.0f, -40.0f}, Anchor::bottom_right),
              fold_button("../data/buttons/fold_button", {-70.0f, -40.0f}, Anchor::bottom_right),
              bar("../data/buttons/bar", {-220.0f, -90.0f}, Anchor::bottom_right) {
        text_font.loadFromFile("../data/fonts/main.ttf");
        background_texture.loadFromFile("../data/backgrounds/asphalt.png");
        background.setTexture(background_texture, true);
    }

    void draw() {
        window.clear();
        window.draw(background);
        std::lock_guard<std::mutex> lock(table.refresh_data);
        switch (table.last_signal()) {
            case Signal::WaitForActionCheckRaise:
                draw_cr();
                break;
            case Signal::WaitForActionCallRaiseFold:
                draw_crf();
                break;
            case Signal::WaitForActionCallFold:
                draw_cf();
                break;
            case Signal::Win:
            case Signal::Lost:
                draw_finish_state();
                break;
            case Signal::ShowWinners:
                simple_redraw(true);
                break;
            case Signal::ShowCards:
            default:
                simple_redraw();
                break;
        }
        window.display();
    }

    void draw_finish_state() {
        sf::Text output;
        output.setString(table.last_signal == Signal::Win ? "Winner" : "Looser");
        output.setFont(text_font);
        output.setCharacterSize(40);
        output.setFillColor(sf::Color::Red);
        output.setOutlineThickness(0.1f);
        output.setOutlineColor(sf::Color::White);
        output.setOrigin({output.getGlobalBounds().width / 2.0f, output.getGlobalBounds().height / 2.0f});
        output.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
        window.draw(output);
    }

    void simple_redraw(bool winners_state = false) {
        sf::Vector2f win_max = static_cast<sf::Vector2f>(window.getSize());
        sf::Vector2f ellipse_max((win_max.x - 100.0f) / 2.0f, (win_max.y - 300.0f) / 2.0f);
        sf::Vector2f ellipse_center(win_max.x / 2.0f, win_max.y / 2.0f);

        sf::Text player_active("A", text_font, 50);
        player_active.setFillColor(sf::Color::Yellow);
        player_active.setOrigin(player_active.getGlobalBounds().width / 2.0f,
                                player_active.getGlobalBounds().height / 2.0f);
        sf::Text player_dealer("D", text_font, 50);
        player_dealer.setFillColor(sf::Color::Blue);
        player_dealer.setOrigin(player_dealer.getGlobalBounds().width / 2.0f,
                                player_dealer.getGlobalBounds().height / 2.0f);

        float diff = 2.0f * std::acos(-1.0f) / static_cast<float>(table.players.size());
        float fi = -std::asin(1.0f) + diff * static_cast<float>(table.main_player);
        for (size_t i = 0; i < table.players.size(); i++) {
            sf::Text player_name(table.players[i].name, text_font, 30);
            if (winners_state) {
                player_name.setFillColor(table.players[i].is_winner ? sf::Color::Yellow : sf::Color{100, 100, 100});
            } else {
                player_name.setFillColor(table.players[i].in_game ? sf::Color::Black : sf::Color{100, 100, 100});
            }
            player_name.setOutlineThickness(0.1f);
            player_name.setOutlineColor(sf::Color::White);
            player_name.setOrigin(player_name.getGlobalBounds().width / 2.0f,
                                  player_name.getGlobalBounds().height / 2.0f);

            sf::Text player_balance(std::to_string(table.players[i].data.balance), text_font, 30);
            player_balance.setFillColor(sf::Color::Green);
            player_balance.setOutlineThickness(0.1f);
            player_balance.setOutlineColor(sf::Color::White);
            player_balance.setOrigin(player_balance.getGlobalBounds().width / 2.0f,
                                     player_balance.getGlobalBounds().height / 2.0f);

            sf::Text player_infront(std::to_string(table.players[i].data.infront), text_font, 30);
            player_infront.setFillColor(sf::Color::Green);
            fi += diff;
            float r = (ellipse_max.x * ellipse_max.y) /
                      std::sqrt(ellipse_max.y * ellipse_max.y * std::cos(fi) * std::cos(fi) +
                                ellipse_max.x * ellipse_max.x * std::sin(fi) * std::sin(fi));
            player_infront.setOutlineThickness(0.1f);
            player_infront.setOutlineColor(sf::Color::White);
            player_infront.setOrigin(player_infront.getGlobalBounds().width / 2.0f,
                                     player_infront.getGlobalBounds().height / 2.0f);

            sf::Texture lc_texture;
            lc_texture.loadFromFile("../data/cards/" + table.players[i].data.show_cards().lhs.to_string() + ".png");
            sf::Texture rc_texture;
            rc_texture.loadFromFile("../data/cards/" + table.players[i].data.show_cards().rhs.to_string() + ".png");
            sf::Sprite player_lc;
            player_lc.setTexture(lc_texture, true);
            player_lc.setOrigin(player_lc.getGlobalBounds().width / 2.0f,
                                player_lc.getGlobalBounds().height / 2.0f);
            player_lc.setScale(0.12f, 0.12f);

            sf::Sprite player_rc;
            player_rc.setTexture(rc_texture, true);
            player_rc.setOrigin(player_rc.getGlobalBounds().width / 2.0f,
                                player_rc.getGlobalBounds().height / 2.0f);
            player_rc.setScale(0.12f, 0.12f);

            player_name.setPosition(ellipse_center.x + r * std::cos(fi), ellipse_center.y + r * std::sin(fi) - 70.0f);
            player_balance.setPosition(ellipse_center.x + r * std::cos(fi),
                                       ellipse_center.y + r * std::sin(fi) - 40.0f);
            player_infront.setPosition(ellipse_center.x + r * std::cos(fi),
                                       ellipse_center.y + r * std::sin(fi) - 10.0f);

            player_lc.setPosition(ellipse_center.x + r * std::cos(fi) - 20.0f,
                                  ellipse_center.y + r * std::sin(fi) + 75.0f);
            player_rc.setPosition(ellipse_center.x + r * std::cos(fi) + 30.0f,
                                  ellipse_center.y + r * std::sin(fi) + 75.0f);

            if (i == table.active_player) {
                player_active.setPosition(ellipse_center.x + r * std::cos(fi) - 20.0f,
                                          ellipse_center.y + r * std::sin(fi) - 110.0f);
            }
            if (i == table.dealer) {
                player_dealer.setPosition(ellipse_center.x + r * std::cos(fi) + 20.0f,
                                          ellipse_center.y + r * std::sin(fi) - 110.0f);
            }

            window.draw(player_name);
            window.draw(player_balance);
            window.draw(player_infront);
            window.draw(player_lc);
            window.draw(player_rc);
        }
        window.draw(player_active);
        window.draw(player_dealer);

        float plus = 100.0f;
        float start_pos_x = ellipse_center.x - 2.0f * plus;
        for (auto card : table.table_cards) {
            sf::Texture card_texture;
            card_texture.loadFromFile("../data/cards/" + card.to_string() + ".png");
            sf::Sprite card_sprite;
            card_sprite.setTexture(card_texture, true);
            card_sprite.setOrigin(card_sprite.getGlobalBounds().width / 2.0f,
                                  card_sprite.getGlobalBounds().height / 2.0f);

            card_sprite.setScale(0.12f, 0.12f);

            card_sprite.setPosition(start_pos_x, ellipse_center.y);
            window.draw(card_sprite);
            start_pos_x += plus;
        }
        plus = 60.0f;
        start_pos_x = ellipse_center.x - (static_cast<float>(table.pots.size() - 1) / 2.0f) * plus;
        for (auto pot : table.pots) {
            sf::Text pot_sprite(std::to_string(pot), text_font, 20);
            pot_sprite.setFillColor(sf::Color::Cyan);
            pot_sprite.setOrigin(pot_sprite.getGlobalBounds().width / 2.0f, pot_sprite.getGlobalBounds().height / 2.0f);
            pot_sprite.setPosition(start_pos_x, ellipse_center.y + 70.0f);
            window.draw(pot_sprite);
            start_pos_x += plus;
        }
    }

    void draw_cr() {
        simple_redraw();
        check_button(window);
        raise_button(window);

        if (!table.reseted) {
            bar.set_mm(1, table.players[table.main_player].data.balance);
            table.reseted = true;
        }
        bar(window);
    }

    void draw_crf() {
        simple_redraw();
        call_button(window);
        raise_button(window);
        fold_button(window);

        if (!table.reseted) {
            bar.set_mm(1, table.players[table.main_player].data.balance);
            table.reseted = true;
        }
        bar(window);
    }

    void draw_cf() {
        simple_redraw();
        call_button(window);
        fold_button(window);
    }
};

class MTQueue {
    std::queue<sf::Packet> packages;
    std::mutex lock;

public:
    bool end = false;

    std::mutex &operator()() { return lock; }

    void push(sf::Packet const &package) { packages.push(package); }

    int pop(sf::Packet &package) {
        if (packages.empty())
            return 0;
        package = packages.front();
        packages.pop();
        return 1;
    }
};

void package_reader(sf::TcpSocket *server_socket, TableInfo &table, MTQueue &queue) {
    Signal signal = Signal::NaI;

    while (signal != Signal::Lost && signal != Signal::Win) {
        sf::Packet package;
        server_socket->setBlocking(false);
        if (server_socket->receive(package) == sf::Socket::Status::Done) {
            package >> signal;
            switch (signal()) {
                case Signal::WaitForActionCheckRaise:
                case Signal::WaitForActionCallRaiseFold:
                case Signal::WaitForActionCallFold: {
                    std::lock_guard<std::mutex> lock(table.refresh_data);
                    table.last_signal = signal;
                    table.reseted = false;
                    break;
                }
                case Signal::Win:
                case Signal::Lost: {
                    std::lock_guard<std::mutex> lock(table.refresh_data);
                    table.last_signal = signal;
                    break;
                }
                case Signal::NewGameInfo: {
                    std::lock_guard<std::mutex> lock(table.refresh_data);
                    table.new_game(package);
                    break;
                }
                case Signal::ChangeTableCards: {
                    std::lock_guard<std::mutex> lock(table.refresh_data);
                    table.refresh_table_cards(package);
                    break;
                }
                case Signal::ChangeActivePlayer: {
                    std::lock_guard<std::mutex> lock(table.refresh_data);
                    table.refresh_active_player(package);
                    break;
                }
                case Signal::ChangeBalances: {
                    std::lock_guard<std::mutex> lock(table.refresh_data);
                    table.refresh_balances_and_activity(package);
                    break;
                }
                case Signal::ChangePot: {
                    std::lock_guard<std::mutex> lock(table.refresh_data);
                    table.refresh_pots(package);
                    break;
                }
                case Signal::ShowCards: {
                    std::lock_guard<std::mutex> lock(table.refresh_data);
                    table.showing_cards(package);
                    break;
                }
                case Signal::ShowWinners: {
                    std::lock_guard<std::mutex> lock(table.refresh_data);
                    table.showing_winners(package);
                    break;
                }
                case Signal::NaI:
                    break;
            }
        }
        package.clear();
        std::lock_guard<std::mutex> lock_q(queue.operator()());
        while (queue.pop(package)) {
            server_socket->send(package);
        }
        if (queue.end) {
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

    RenderTable render_table{1280, 720};
    MTQueue queue;
    std::thread reader(&package_reader, &server_socket, std::ref(render_table.table), std::ref(queue));

    while (render_table.window.isOpen()) {
        sf::Event event{};
        while (render_table.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                render_table.window.close();
            }
            if (event.type == sf::Event::Resized) {
                sf::Vector2u win_size = render_table.window.getSize();
                if (win_size.x < 1280 && win_size.y < 720) {
                    render_table.window.setSize({1280, 720});
                } else if (win_size.y < 720) {
                    render_table.window.setSize({win_size.x, 720});
                } else if (win_size.x < 1280) {
                    render_table.window.setSize({1280, win_size.y});
                }
                render_table.window.setView(sf::View(
                        sf::FloatRect(0.0f, 0.0f, render_table.window.getSize().x, render_table.window.getSize().y)));
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {
                sf::Vector2f mouse_pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(render_table.window));
                std::lock_guard<std::mutex> lock(render_table.table.refresh_data);
                if (render_table.table.last_signal == Signal::WaitForActionCallRaiseFold) {
                    render_table.call_button.press(mouse_pos);
                    render_table.raise_button.press(mouse_pos);
                    render_table.fold_button.press(mouse_pos);
                    render_table.bar.press(mouse_pos);
                }
                if (render_table.table.last_signal == Signal::WaitForActionCheckRaise) {
                    render_table.check_button.press(mouse_pos);
                    render_table.raise_button.press(mouse_pos);
                    render_table.bar.press(mouse_pos);
                }
                if (render_table.table.last_signal == Signal::WaitForActionCallFold) {
                    render_table.call_button.press(mouse_pos);
                    render_table.fold_button.press(mouse_pos);
                    render_table.bar.press(mouse_pos);
                }
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) {
                sf::Vector2f mouse_pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(render_table.window));
                std::lock_guard<std::mutex> lock(render_table.table.refresh_data);
                render_table.bar.unpress();
                if (render_table.table.last_signal == Signal::WaitForActionCallRaiseFold) {
                    if (render_table.call_button.clicked(mouse_pos)) {
                        sf::Packet package;
                        package << Action{Action::call};
                        std::lock_guard<std::mutex> lock_q(queue());
                        queue.push(package);
                        render_table.table.last_signal = Signal::NaI;
                    }
                    if (render_table.raise_button.clicked(mouse_pos)) {
                        sf::Packet package;
                        Action act{Action::raise};
                        act.additional_info = render_table.bar.unpress();
                        package << act;
                        std::lock_guard<std::mutex> lock_q(queue());
                        queue.push(package);
                        render_table.table.last_signal = Signal::NaI;
                    }
                    if (render_table.fold_button.clicked(mouse_pos)) {
                        sf::Packet package;
                        package << Action{Action::fold};
                        std::lock_guard<std::mutex> lock_q(queue());
                        queue.push(package);
                        render_table.table.last_signal = Signal::NaI;
                    }
                }
                if (render_table.table.last_signal == Signal::WaitForActionCheckRaise) {
                    if (render_table.check_button.clicked(mouse_pos)) {
                        sf::Packet package;
                        package << Action{Action::check};
                        std::lock_guard<std::mutex> lock_q(queue());
                        queue.push(package);
                        render_table.table.last_signal = Signal::NaI;
                    }
                    if (render_table.raise_button.clicked(mouse_pos)) {
                        sf::Packet package;
                        Action act{Action::raise};
                        act.additional_info = render_table.bar.unpress();
                        package << act;
                        std::lock_guard<std::mutex> lock_q(queue());
                        queue.push(package);
                        render_table.table.last_signal = Signal::NaI;
                    }
                }
                if (render_table.table.last_signal == Signal::WaitForActionCallFold) {
                    if (render_table.call_button.clicked(mouse_pos)) {
                        sf::Packet package;
                        package << Action{Action::call};
                        std::lock_guard<std::mutex> lock_q(queue());
                        queue.push(package);
                        render_table.table.last_signal = Signal::NaI;
                    }
                    if (render_table.fold_button.clicked(mouse_pos)) {
                        sf::Packet package;
                        package << Action{Action::fold};
                        std::lock_guard<std::mutex> lock_q(queue());
                        queue.push(package);
                        render_table.table.last_signal = Signal::NaI;
                    }
                }
            }
            if (event.type == sf::Event::MouseMoved &&
                (render_table.table.last_signal == Signal::WaitForActionCallRaiseFold ||
                 render_table.table.last_signal == Signal::WaitForActionCheckRaise)) {
                std::lock_guard<std::mutex> lock(render_table.table.refresh_data);
                render_table.bar.moved(static_cast<sf::Vector2f>(sf::Mouse::getPosition(render_table.window)));
            }
        }
        render_table.draw();
        sf::sleep(sf::milliseconds(20));
    }
    {
        std::lock_guard<std::mutex> lock_q(queue());
        queue.end = true;
    }
    reader.join();
    server_socket.disconnect();
    return 0;
}