#include <iostream>
#include "SFML/Graphics.hpp"

#include "lib/Table.h"

int main(int argc, char *argv[]) {
    sf::TcpListener listener;
    sf::TcpSocket serer_socket;
    sf::Socket::Status status = serer_socket.connect(sf::IpAddress::getLocalAddress(), 56789);
    if (status != sf::Socket::Done) {
        std::cout << "Bad";
    }
    sf::Packet package;

    sf::RenderWindow window(sf::VideoMode(600, 800), "Client");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }
    serer_socket.disconnect();
    return 0;
}