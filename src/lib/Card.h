#ifndef POKER_CARD_H
#define POKER_CARD_H

enum class Suit { diamonds, clubs, hearts, spades };

enum class Rank { two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace };

int rank_uint(Rank const &rank);

int suit_uint(Suit const &suit);

struct Card {
    Suit suit;
    Rank rank;
};

#endif  // POKER_CARD_H
