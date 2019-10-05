#include <iostream>
#include "SFML/Graphics.hpp"

#include "lib/Table.h"

enum class State {};

class Field {
    Player this_player;
    std::pair<std::array<Card, 5>, sf::Uint8> table_cards;
    std::map<sf::String, Player> rivals;
    std::vector<sf::Uint64> pots;

    void draw() {}
};

int main(int argc, char *argv[]) {
    sf::TcpListener listener;
    sf::TcpSocket serer_socket;
    sf::Socket::Status status = serer_socket.connect(sf::IpAddress::getLocalAddress(), 56789);
    if (status != sf::Socket::Done) {
        std::cout << "Bad";
    }
    sf::Packet package;
    sf::String type;
    sf::RenderWindow window(sf::VideoMode(600, 800), "Client");

    std::map<std::pair<sf::String, unsigned short>, std::pair<size_t, size_t>> players{};
    Player me(0);
    std::vector<size_t> pots;
    size_t tc_amount{0};
    std::array<Card, 5> table_cards{};

    sf::Font font;
    font.loadFromFile("../data/Am.ttf");
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::Red);

    while (window.isOpen()) {
        package.clear();
        serer_socket.receive(package);
        package >> type;

        if (type == "Table Cards") {
            sf::Uint64 tc_buf;
            package >> tc_buf;
            tc_amount = tc_buf;
            for (size_t i = 0; i < tc_amount; ++i) {
                sf::Uint64 suit, rank;
                package >> suit >> rank;
                table_cards[i].suit = underlying_cast<Suit>(suit);
                table_cards[i].rank = underlying_cast<Rank>(suit);
            }
            text_update(text, me, table_cards, tc_amount, players, pots);
            window.clear(sf::Color::White);
            window.draw(text);
            window.display();
        }
        if (type == "Players Info") {
            std::pair<sf::String, unsigned short> addr;
            std::pair<sf::Uint64, sf::Uint64> bal_fr;
            package >> addr.first >> addr.second >> bal_fr.first >> bal_fr.second;
            players[addr] = bal_fr;
            text_update(text, me, table_cards, tc_amount, players, pots);
            window.clear(sf::Color::White);
            window.draw(text);
            window.display();
        }
        if (type == "Player Info") {
            sf::Uint64 cfs, cfr, css, csr;
            sf::Uint64 bal, fr;
            sf::Uint64 ps;
            package >> cfs >> cfr >> css >> csr >> bal >> fr >> ps;
            me.retake(Card{underlying_cast<Suit>(cfs), underlying_cast<Rank>(cfr)},
                      Card{underlying_cast<Suit>(css), underlying_cast<Rank>(csr)});
            me.balance = bal;
            me.infront = fr;
            pots.resize(ps);
            for (auto &pot : pots) {
                package >> bal;
                pot = bal;
            }
            text_update(text, me, table_cards, tc_amount, players, pots);
            window.clear(sf::Color::White);
            window.draw(text);
            window.display();
        }
        if (type == "Goodbye") {
            window.clear(sf::Color::White);
            text.setString(type);
            window.draw(text);
            window.display();
            sf::sleep(sf::seconds(3));
            window.close();
            break;
        }
        if (type == "Action") {
            sf::Uint64 max;
            package >> max;
            text.getString();

            text.setString(text.getString() + "\n\n" + type + " " + std::to_string(max) +
                           (max == me.infront ? " Can check" : " Can't check"));
            window.clear(sf::Color::White);
            window.draw(text);
            window.display();
            std::string com;
            std::cin >> com;

            if (com == "check") {
                package.clear();
                package << sf::String("Check");
                serer_socket.send(package);
            }
            if (com == "call") {
                package.clear();
                package << sf::String("Call");
                serer_socket.send(package);
            }
            if (com == "fold") {
                package.clear();
                package << sf::String("Fold");
                serer_socket.send(package);
            }
            if (com == "raise") {
                size_t money;
                std::cin >> money;
                package << sf::String("Raise") << sf::Uint64(money);
                serer_socket.send(package);
            }
        }
    }
    serer_socket.disconnect();
    return 0;
}