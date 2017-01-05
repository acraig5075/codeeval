// 69-distinct-subsequences.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <algorithm>

using IndexRank = std::pair < size_t, int>;
using IndexRankVector = std::vector<IndexRank>;

auto split(const std::string &value, char delimiter) -> std::vector<std::string>;
auto subsequences(const std::string &sequence, const std::string &subsequence) -> int;
auto get_rank(char c, const std::string &sequence) -> IndexRank;

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
					std::cout << answer;
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

int subsequences(const std::string &sequence, const std::string &subsequence)
{
	for (char c : subsequence)
	{
		IndexRank rank = get_rank(c, sequence);
	}
	return 0;
}

IndexRank get_rank(char find, const std::string &sequence)
{
	size_t a = std::count_if(begin(sequence), end(sequence), [find](const char &c)
	{
		return c == find;
	});

	return std::make_pair(a, 0);
}

