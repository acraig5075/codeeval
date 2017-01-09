// 69-distinct-subsequences.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <numeric>

using IndexRank = std::pair <size_t, int>;
using IndexRankVector = std::vector<IndexRank>;

auto split(const std::string &value, char delimiter) -> std::vector<std::string>;
auto subsequences(const std::string &sequence, const std::string &subsequence) -> int;
auto calc_rank(size_t index, const IndexRankVector &previous) -> int;
auto calc_score(const IndexRankVector &v) -> int;


int main(int argc, char *argv[])
{
	if (argc > 1)
		{
		std::string filename(argv[1]);
		std::ifstream fin(filename.c_str());

		if (fin.is_open())
			{
			std::string line;
			while (fin.good())
				{
				std::getline(fin, line);

				if (!line.empty())
					{
					std::vector<std::string> tokens = split(line, ',');
					assert(tokens.size() == 2);

					int answer = subsequences(tokens.at(0), tokens.at(1));
					std::cout << answer << "\n";
					}
				}
			}
		}

	return 0;
}

// String tokeniser
std::vector<std::string> split(const std::string &value, char delimiter)
{
	std::vector<std::string> ret;

	size_t start = 0;
	while (true)
		{
		size_t pos = value.find(delimiter, start);
		if (pos == -1)
			{
			ret.push_back(value.substr(start));
			break;
			}
		else
			{
			ret.push_back(value.substr(start, pos - start));
			start = pos + 1;
			}
		}

	return ret;
}

// Function to return the score of the test
int subsequences(const std::string &sequence, const std::string &subsequence)
{
	IndexRankVector current, previous;

	// gcc < 5.0 doesn't support std::rbegin and std::rend. So rather work with a reversed copy.
	std::string rev_subsequence{ subsequence };
	std::reverse(std::begin(rev_subsequence), std::end(rev_subsequence));

	for (auto it = std::begin(rev_subsequence); it != std::end(rev_subsequence); ++it)
		{
		size_t off = 0;

		size_t index = sequence.find(*it, off);

		// early exit if no subsequence exists at all
		if (off == 0 && index == sequence.npos)
			return 0;

		while (index != sequence.npos)
			{
			int rank = calc_rank(index, previous);
			current.push_back(std::make_pair(index, rank));

			off = index + 1;
			index = sequence.find(*it, off);
			}

		previous = current;
		current.clear();
		}

	return calc_score(previous);
}

// The rank is the no. of items in the vector having an index greater than `index`
int calc_rank(size_t index, const IndexRankVector &v)
{
	if (v.empty())
		{
		return 1;
		}
	else
		{
		return std::accumulate(std::begin(v), std::end(v), 0, [index](int val, const IndexRank &item)
			{
			if (item.first > index)
				return val + item.second;
			else
				return val;
			});
		}
}

// The score is the sum of all ranks of a vector
int calc_score(const IndexRankVector &v)
{
	return std::accumulate(std::begin(v), std::end(v), 0, [](int val, const IndexRank &item)
		{
		return val + item.second;
		});
}
