// 175-the-cubes.cpp : Defines the entry point for the console application.
//

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
using std::pair;

using edges = vector<pair<size_t, size_t>>;

auto split(const string &value, char delimiter) -> vector<string>;
auto increment(size_t &value) -> void;
auto decrement(size_t &value) -> void;


class floor_plan
{
public:
	floor_plan(size_t d)
		: dim(d)
	{
		grid.resize(d, vector<char>(d, ' '));
	}

	void make(const string &s)
	{
		assert(s.length() == dim * dim);

		size_t r = 0;
		size_t c = 0;

		for (auto v : s)
			{
			grid[r][c] = v;

			c++;
			if (c == dim)
				{
				r++;
				c = 0;
				}
			}
	}

	size_t id(size_t level, size_t r, size_t c) const
	{
		return (level * dim * dim) + (r * dim) + c;
	}

	edges possible_steps(size_t level)
	{
		for (size_t r = 1; r < dim - 1; ++r)
			{
			for (size_t c = 1; c < dim - 1; ++c)
				{
				if (grid[r][c] != '*')
					{
					up(level, r, c);
					down(level, r, c);
					left(level, r, c);
					right(level, r, c);
					hole(level, r, c);
					}
				}
			}

		return steps;
	}

	void movement(size_t level, size_t r, size_t c, void(*rowOp)(size_t &), void(*colOp)(size_t &))
	{
		size_t from = id(level, r, c);

		if (rowOp)
			(*rowOp)(r);
		if (colOp)
			(*colOp)(c);

		if (grid[r][c] == '*')
			return;

		steps.push_back({ from, id(level, r, c) });

		// handle jump over hole
		if (grid[r][c] == 'o')
			{
			if (rowOp)
				(*rowOp)(r);
			if (colOp)
				(*colOp)(c);

			if (grid[r][c] != '*')
				steps.push_back({ from, id(level, r, c) });
			}
	}

	void up(size_t level, size_t r, size_t c)
	{
		movement(level, r, c, &decrement, nullptr);
	}

	void down(size_t level, size_t r, size_t c)
	{
		movement(level, r, c, &increment, nullptr);
	}

	void left(size_t level, size_t r, size_t c)
	{
		movement(level, r, c, nullptr, &decrement);
	}

	void right(size_t level, size_t r, size_t c)
	{
		movement(level, r, c, nullptr, &increment);
	}

	void hole(size_t level, size_t r, size_t c)
	{
		assert(level > 0);

		if (grid[r][c] == 'o')
			{
			size_t from = id(level, r, c);
			size_t to = id(level - 1, r, c);

			steps.push_back({ from, to });
			steps.push_back({ to, from });
			}
	}


private:
	size_t dim = 0;
	vector<vector<char>> grid;
	edges steps;
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

	edges all_steps()
	{
		edges all;

		for (size_t i = 0; i < dim; ++i)
			{
			edges steps = levels[i].possible_steps(i);
			all.insert(end(all), begin(steps), end(steps));
			}

		// todo add entrance and exit step

		return all;
	}

	size_t level_opening(size_t level)
	{
		return 0;
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

					edges movements = labyrinth.all_steps();
					size_t start = labyrinth.level_opening(0);
					size_t end = labyrinth.level_opening(dim - 1);

					//graph g(dim * dim * dim);
					//g.read(movements);
					//g.bfs(start);
					//g.get_distance(end);
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

auto increment(size_t &value) -> void
{
	++value;
}

auto decrement(size_t &value) -> void
{
	--value;
}
