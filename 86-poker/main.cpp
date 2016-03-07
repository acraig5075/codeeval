// 86-poker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <iostream>
#include <algorithm>

using std::string;
using std::vector;
using std::array;

enum class Rank{ None, HighCard, OnePair, TwoPairs, Threes, Straight, Flush, FullHouse, Fours, StraightFlush, RoyalFlush };
enum class Suit{ None, Clubs, Diamonds, Hearts, Spades };

struct Card
{
	array<char, 2> code;

	int value() const
	{
		switch (code[0])
		{
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'T':
			return 10;
		case 'J':
			return 11;
		case 'Q':
			return 12;
		case 'K':
			return 13;
		case 'A':
			return 14;
		default:
			return 0;
		}
	}

	Suit suit() const
	{
		switch (code[1])
		{
		case 'C':
			return Suit::Clubs;
		case 'D':
			return Suit::Diamonds;
		case 'H':
			return Suit::Hearts;
		case 'S':
			return Suit::Spades;
		default:
			return Suit::None;
		}
	}
};

struct Hand
{
	array<Card, 5> cards;
	Rank rank = Rank::None;

	void orderByValue()
	{
		std::sort(cards.begin(), cards.end(), [=](const Card &a, const Card &b) { return a.value() < b.value(); });
	}

	int highest() const
	{
		return cards[4].value();
	}

	void evaluate()
	{
		bool flush = 
			cards[0].suit() == cards[1].suit() && 
			cards[1].suit() == cards[2].suit() && 
			cards[2].suit() == cards[3].suit() && 
			cards[3].suit() == cards[4].suit();
		bool straight = 
			cards[0].value() == cards[1].value() - 1 &&
			cards[1].value() == cards[2].value() - 1 &&
			cards[2].value() == cards[3].value() - 1 &&
			cards[3].value() == cards[4].value() - 1;
		bool royal = straight && 
			cards[0].value() == 10;

		if (royal && flush)
		{
			rank = Rank::RoyalFlush;
			return;
		}
		else if (straight && flush)
		{
			rank = Rank::StraightFlush;
		}

		// for every permutation keep track of highest rank:
		// - test first 4 for equality: fours
		// - test first 3 and last 2 for equality: full house
		// - test first 2 and next 2 for equality: two pair
		// - test first 2 for equality: one pair

	}

};

std::istream& operator>>(std::istream &in, Card &card)
{
	in >> card.code[0] >> card.code[1];
	return in;
}

std::istream& operator>>(std::istream &in, Hand &hand)
{
	if (in >> hand.cards[0] >> hand.cards[1] >> hand.cards[2] >> hand.cards[3] >> hand.cards[4])
		hand.orderByValue();
	return in;
}

std::ostream& operator<<(std::ostream &out, const Card &card)
{
	out << card.code[0] << card.code[1];
	return out;
}

std::ostream& operator<<(std::ostream &out, const Hand &hand)
{
	out << hand.cards[0] << " " << hand.cards[1] << " " << hand.cards[2] << " " << hand.cards[3] << " " << hand.cards[4];
	return out;
}
int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		string filename(argv[1]);
		std::ifstream fin(filename.c_str());

		Hand hand1, hand2;
		while (fin >> hand1 >> hand2)
		{
			hand1.evaluate();
			hand2.evaluate();
			std::cout << hand1 << "   " << hand2 << "\n";
		}
	}

	return 0;
}

