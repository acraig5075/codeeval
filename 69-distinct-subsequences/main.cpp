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

					// gcc < 5.0 doesn't support std::rbegin and std::rend. So rather work with a reversed copy.
					std::string revcopy{ tokens.at(1) };
					std::reverse(std::begin(revcopy), std::end(revcopy));

					int answer = subsequences(tokens.at(0), revcopy);
					std::cout << answer << "\n";
					}
				}
			}
		}

	return 0;
}

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

int subsequences(const std::string &sequence, const std::string &rev_subsequence)
{
	IndexRankVector current, previous;

	for (auto it = std::begin(rev_subsequence); it != std::end(rev_subsequence); ++it)
		{
		size_t off = 0;

		size_t index = sequence.find(*it, off);

		if (off == 0 && index == sequence.npos)
			return 0; // no point in continuing.

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

int calc_score(const IndexRankVector &v)
{
	return std::accumulate(std::begin(v), std::end(v), 0, [](int val, const IndexRank &item)
		{
		return val + item.second;
		});
}
