// 175-the-cubes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cassert>

using std::string;
using std::vector;
using std::cout;
using std::ifstream;

auto split(const string &value, char delimiter)->vector<string>;

class floor_plan
{
public:
	floor_plan(size_t d)
		: dim(d)
	{
		rows.resize(d, vector<char>(d, ' '));
	}

	void make(const string &s)
	{
		assert(s.length() == dim * dim);

		size_t r = 0;
		size_t c = 0;

		for (auto v : s)
		{
			rows[r][c] = v;

			c++;
			if (c == dim)
			{
				r++;
				c = 0;
			}
		}
	}

private:
	size_t dim = 0;
	vector<vector<char>> rows;
};

class labyrinth
{
public:
	labyrinth(size_t d)
		: dim(d)
	{
		levels.resize(d, floor_plan(d));
	}

	void make(const string &s)
	{
		for (size_t i = 0; i < dim; ++i)
		{
			size_t start = i * dim;
			levels[i].make(s.substr(start, dim * dim));
		}
	}

private:
	size_t dim = 0;
	vector<floor_plan> levels;
};

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		string filename(argv[1]);
		ifstream fin(filename.c_str());

		if (fin.is_open())
		{
			string line;
			while (fin.good())
			{
				getline(fin, line);

				if (!line.empty())
				{
					auto tokens = split(line, ';');
					auto str1 = tokens[0];
					auto str2 = tokens[1];

					size_t dim = atoi(str1.c_str());
					assert(str2.length() >= dim * dim * dim);

					labyrinth labyrinth(dim);
					labyrinth.make(str2);
				}
			}
		}
	}

	return 0;
}

// string tokenise
auto split(const string &value, char delimiter) -> vector<string>
{
	vector<string> ret;

	size_t start = 0;
	while (true)
	{
		size_t pos = value.find(delimiter, start);
		if (pos == string::npos)
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

