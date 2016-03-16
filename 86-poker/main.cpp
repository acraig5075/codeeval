#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <functional>
#include <cassert>

using std::string;
using std::vector;

enum class Rank { None, HighCard, OnePair, TwoPairs, Threes, Straight, Flush, FullHouse, Fours, StraightFlush, RoyalFlush };

enum class Suit { None, Clubs, Diamonds, Hearts, Spades };

std::vector<string> rankText = { "None", "HighCard", "OnePair", "TwoPairs", "Threes", "Straight", "Flush", "FullHouse", "Fours", "StraightFlush", "RoyalFlush" };

struct Card
{
	std::array<char, 2> code;

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
	std::array<Card, 5> cards;
	Rank rank = Rank::None;
	vector<int> scoringValues; // the indexes into `cards` that contribute to the rank in decreasing value
	vector<int> remainingValues; // the indexes in decreasing value that are remaining, i.e. the kickers. 

	void orderByValue()
	{
		std::sort(cards.begin(), cards.end(), std::greater<Card>());
	}

	// Does the hard work of determing the rank of a hand, the scoring cards of the rank, and the cards not contributing to the rank
	void evaluate()
	{
		bool flush =
			cards[0].suit() == cards[1].suit() &&
			cards[1].suit() == cards[2].suit() &&
			cards[2].suit() == cards[3].suit() &&
			cards[3].suit() == cards[4].suit();

		bool straight =
			cards[0].value() == cards[1].value() + 1 &&
			cards[1].value() == cards[2].value() + 1 &&
			cards[2].value() == cards[3].value() + 1 &&
			cards[3].value() == cards[4].value() + 1;

		bool royal = straight &&
								 cards[0].value() == 14;

		if (royal && flush)
			{
			rank = Rank::RoyalFlush;
			return;
			}
		else if (straight && flush)
			{
			rank = Rank::StraightFlush;
			return;
			}

		Rank best = flush ? Rank::Flush : (straight ? Rank::Straight : Rank::HighCard);
		scoringValues = { cards[0].value(), cards[1].value(), cards[2].value(), cards[3].value(), cards[4].value() };

		// for every permutation keep track of highest rank

		vector<size_t> perms(cards.size());
		std::iota(perms.begin(), perms.end(), 0);
		do
			{
			auto value0 = cards[perms[0]].value();
			auto value1 = cards[perms[1]].value();
			auto value2 = cards[perms[2]].value();
			auto value3 = cards[perms[3]].value();
			auto value4 = cards[perms[4]].value();

			bool threes =
				value0 == value1 &&
				value1 == value2;

			bool fours = 
				threes && 
				value2 == value3;

			bool fullhouse = 
				threes &&
				value3 == value4;

			bool onepair =
				value0 == value1;

			bool twopair = 
				onepair &&
				value2 == value3;

			if (fours && Rank::Fours > best)
				{
				best = Rank::Fours;
				scoringValues = { value0 };
				remainingValues = { value4 };
				}
			else if (fullhouse && Rank::FullHouse > best)
				{
				best = Rank::FullHouse;
				scoringValues = { value0, value3 };
				}
			else if (threes && Rank::Threes > best)
				{
				best = Rank::Threes;
				scoringValues = { value0 };
				remainingValues = { value3, value4 };
				}
			else if (twopair && Rank::TwoPairs > best)
				{
				best = Rank::TwoPairs;
				scoringValues = { value0, value2 };
				remainingValues = { value4 };
				}
			else if (onepair && Rank::OnePair > best)
				{
				best = Rank::OnePair;
				scoringValues = { value0 };
				remainingValues = { value2, value3, value4 };
				}

			}
		while (std::next_permutation(perms.begin(), perms.end()));

		// set the final rank of the hand to be the best we've encounterd thus far
		rank = best;

		// sort the rank values in decreasing value, excepting FullHouse which needs the Threes compared before the Pair
		if (rank != Rank::FullHouse)
			std::sort(scoringValues.begin(), scoringValues.end(), std::greater<int>());

		// sort the kickers in decreasing value
		std::sort(remainingValues.begin(), remainingValues.end(), std::greater<int>());
	}
};

// A generic function to compare the first differing items of one range with the item of the same index of another range.

template <typename Itr, typename P>
bool innerCompare(const Itr &first1, const Itr &last1, const Itr &first2, const Itr &last2, P predicate, bool &ok)
{
	assert(std::distance(first1, last1) == std::distance(first2, last2));

	if (std::distance(first1, last1) == std::distance(first2, last2))
	{
		Itr itr1, itr2;
		std::tie(itr1, itr2) = std::mismatch(first1, last1, first2);

		if (itr1 != last1 && itr2 != last2)
		{
			ok = true;
			return predicate(*itr1, *itr2);
		}
		else
		{
			ok = false;
			return false;
		}
	}

	ok = false;
	return false;
}

// Comparison operators

bool operator< (const Card &lhs, const Card &rhs)
{
	return lhs.value() < rhs.value();
}

bool operator> (const Card &lhs, const Card &rhs)
{
	return lhs.value() > rhs.value();
}

bool operator== (const Card &lhs, const Card &rhs)
{
	return lhs.value() == rhs.value();
}

bool operator> (const Hand &lhs, const Hand &rhs)
{
	if (lhs.rank > rhs.rank)
		{
		return true;
		}
	else if (lhs.rank == rhs.rank)
		{
		bool ok;
		bool gt = innerCompare(lhs.scoringValues.cbegin(), lhs.scoringValues.cend(), rhs.scoringValues.cbegin(), rhs.scoringValues.cend(), std::greater<int>(), ok);

		if (ok)
			return gt;
		else
			return innerCompare(lhs.cards.cbegin(), lhs.cards.cend(), rhs.cards.cbegin(), rhs.cards.cend(), std::greater<Card>(), ok);
		}
	return false;
}

// Input stream operators

std::istream &operator>>(std::istream &in, Card &card)
{
	in >> card.code[0] >> card.code[1];
	return in;
}

std::istream &operator>>(std::istream &in, Hand &hand)
{
	hand.rank = Rank::None;
	hand.scoringValues.clear();
	hand.remainingValues.clear();

	if (in >> hand.cards[0] >> hand.cards[1] >> hand.cards[2] >> hand.cards[3] >> hand.cards[4])
		{
		hand.orderByValue();
		hand.evaluate();
		}
	return in;
}

// Output stream operators

std::ostream &operator<<(std::ostream &out, const Card &card)
{
	out << card.code[0] << card.code[1];
	return out;
}

std::ostream &operator<<(std::ostream &out, const Hand &hand)
{
	out << hand.cards[0] << " "
		<< hand.cards[1] << " "
		<< hand.cards[2] << " "
		<< hand.cards[3] << " "
		<< hand.cards[4] << " "
		<< std::setw(15) << std::left << rankText.at(static_cast<size_t>(hand.rank));
	return out;
}

// All that `main` now needs to do is iteratively construct two hands from the file stream and compare them with operator>.

int main(int argc, char *argv[])
{
	if (argc > 1)
		{
		std::ifstream fin(argv[1]);

		Hand hand1, hand2;
		while (fin >> hand1 >> hand2)
			{
			//std::cout << hand1 << " | " << hand2 << "\n";

			if (hand1 > hand2)
				std::cout << "left\n";
			else if (hand2 > hand1)
				std::cout << "right\n";
			else
				std::cout << "none\n";
			}
		}

	return 0;
}
