#include <iostream>
#include "SFML/Graphics.hpp"

#include "lib/Table.h"

int main() {
    sf::TcpListener listener;
    listener.listen(56789);
    std::vector<sf::TcpSocket *> players;
    while (players.size() != 2) {
        auto *client = new sf::TcpSocket;
        if (listener.accept(*client) == sf::Socket::Done) {
            players.push_back(client);
            players.back()->setBlocking(false);
        }
    }
    Table table(players.begin(), players.end(), 1000, 1);
    table.tournament();
    return 0;
}