#include "Card.h"

int rank_uint(Rank const &rank) {
    switch (rank) {
    case Rank::two:
        return 2;
    case Rank::three:
        return 3;
    case Rank::four:
        return 4;
    case Rank::five:
        return 5;
    case Rank::six:
        return 6;
    case Rank::seven:
        return 7;
    case Rank::eight:
        return 8;
    case Rank::nine:
        return 9;
    case Rank::ten:
        return 10;
    case Rank::jack:
        return 11;
    case Rank::queen:
        return 12;
    case Rank::king:
        return 13;
    case Rank::ace:
        return 14;
    }
    return 0;
}

int suit_uint(Suit const &suit) {
    switch (suit) {
    case Suit::diamonds:
        return 1;
    case Suit::hearts:
        return 2;
    case Suit::clubs:
        return 3;
    case Suit::spades:
        return 4;
    }
    return 0;
}