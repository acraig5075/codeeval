#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cassert>
#include <queue>
#include <iterator>

using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::pair;
using std::queue;

using edges = vector<pair<size_t, size_t>>;

auto split(const string &value, char delimiter) -> vector<string>;
auto increment(size_t &value) -> void;
auto decrement(size_t &value) -> void;

#define infinity -1


// Adjacency list graph for shortest path
class graph
{
public:
	graph(size_t v)
		: n(v)
	{
		adjacency_list.resize(v, vector<int>());
		dist.resize(v, infinity);
	}

	void read(const edges &edges)
	{
		for (const auto e : edges)
		{
			adjacency_list[e.first].push_back(e.second);
		}
	}

	void bfs(int s)
	{
		dist.resize(n, infinity);
		dist[s] = 1;

		queue<int> q;
		q.push(s);

		while (!q.empty())
		{
			int u = q.front();
			q.pop();

			for (auto v : adjacency_list[u])
			{
				if (dist[v] == infinity)
				{
					q.push(v);
					dist[v] = dist[u] + 1;
				}
			}
		}
	}

	int get_distance(int v)
	{
		return dist[v];
	}


private:
	size_t n = 0;
	vector<vector<int>> adjacency_list;
	vector<int> dist;
};


// Encapsulate a single level floor plan
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

	void print(std::ostream &out) const
	{
		for (const auto &r : grid)
		{
			for (const auto &c : r)
			{
				out << c;
			}
			out << "\n";
		}
		out << "\n\n";
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

		assert(r < dim && c < dim);

		if (grid[r][c] == '*')
			return;

		// a valid step is into a space, into the start of a hole, or completely over a hole
		// an invalid step is jumping over a hole into a hole

		steps.push_back({ from, id(level, r, c) });

		// handle jump over hole
		while (grid[r][c] == 'o')
			{
			if (rowOp)
				(*rowOp)(r);
			if (colOp)
				(*colOp)(c);

			if (grid[r][c] != ' ')
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
		if (level > 0 && grid[r][c] == 'o')
			{
			size_t from = id(level, r, c);
			size_t to = id(level - 1, r, c);

			steps.push_back({ from, to });
			steps.push_back({ to, from });
			}
	}

	pair<size_t, size_t> entrance(size_t level)
	{
		const size_t side = dim - 1;
		for (size_t r = 0; r < dim; ++r)
		{
			for (size_t c = 0; c < dim; ++c)
			{
				if ((c == 0 || r == 0 || c == side || r == side) && grid[r][c] == ' ')
				{
					if (r == 0)
						return{ id(level, r, c), id(level, r + 1, c) };
					else if (r == side)
						return{ id(level, r, c), id(level, r - 1, c) };
					else if (c == 0)
						return{ id(level, r, c), id(level, r, c + 1) };
					else if (c == side)
						return{ id(level, r, c), id(level, r, c - 1) };
				}
			}
		}

		assert(false);
		return{ 0,0 };
	}

private:
	size_t dim = 0;
	vector<vector<char>> grid;
	edges steps;
};


// Encapsulate a labyrinth of multiple levels
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
			size_t start = i * dim * dim;
			levels[i].make(s.substr(start, dim * dim));
			}
	}

	void print(std::ostream &out)
	{
		for (const auto &l : levels)
			l.print(out);
	}

	edges all_steps()
	{
		edges all;

		for (size_t i = 0; i < dim; ++i)
			{
			edges steps = levels[i].possible_steps(i);
			all.insert(end(all), begin(steps), end(steps));
			}

		auto edge1 = levels[0].entrance(0);
		auto edge2 = levels[dim - 1].entrance(dim - 1);
		all.push_back(edge1);
		all.push_back(edge2);

		return all;
	}

	size_t level_opening(size_t level)
	{
		auto opening = levels[level].entrance(level);
		return opening.first;
	}

private:
	size_t dim = 0;
	vector<floor_plan> levels;
};


// The main program
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
					// tokenise the input
					auto tokens = split(line, ';');
					string count = tokens[0];
					string data = tokens[1];

					size_t dim = atoi(count.c_str());
					assert(data.length() >= dim * dim * dim);

					// construct the levels
					labyrinth labyrinth(dim);
					labyrinth.make(data);

					// determine all possible steps
					edges movements = labyrinth.all_steps();
					size_t start = labyrinth.level_opening(0);
					size_t end = labyrinth.level_opening(dim - 1);

					// graph the steps and determine short path
					graph g(dim * dim * dim);
					g.read(movements);
					g.bfs(start);
					int shortest = g.get_distance(end);

					// output
					if (shortest == infinity)
						cout << 0 << "\n";
					else
						cout << shortest << "\n";
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
	assert(value != 0);
	--value;
}
