#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <iomanip>
#include <numeric>

using std::vector;
using std::string;
using std::ifstream;
using std::ostream;
using std::cout;

struct item
{
	size_t id = 0;
	int weight = 0;
	int value = 0;
};


auto split(const string &value, char delimiter) -> vector<string>;
auto parse(const string &line, double &total) -> vector<item>;
auto trim(string &str, const string &chars) -> void;
auto output_csv(ostream &os, const vector<int> &ids) -> void;
auto integer_weight(double weight) -> int;
auto dynamic_knapsack(size_t W, vector<item> &items) -> vector<int>;
auto reconstruct_solution(const vector<vector<int>> &value, size_t W, const vector<item> &items) -> vector<int>;
auto debug_output(int total, vector<item> &items, const vector<int> &solution) -> void;


int main(int argc, char *argv[])
{
	if (argc > 1)
		{
		string filename(argv[1]);
		ifstream fin(filename.c_str());

		if (fin.is_open())
			{
			vector<string> tests;
			string line;
			while (fin.good())
				{
				getline(fin, line);
				tests.push_back(line);

				if (!line.empty())
					{
					double total = 0;
					vector<item> items = parse(line, total);
					vector<int> package = dynamic_knapsack((size_t)integer_weight(total), items);
					//debug_output(integer_weight(total), items, package);
					output_csv(cout, package);
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

// build item list from a test case string
auto parse(const string &line, double &total) -> vector<item>
{
	auto tokens = split(line, ':');
	assert(tokens.size() == 2);
	trim(tokens[0], " ");
	trim(tokens[1], " ");

	total = std::atof(tokens[0].c_str());

	vector<item> items;
	tokens = split(tokens[1], ' ');

	for (const auto &p : tokens)
		{
		string params = p;
		assert(params[0] == '(' && params[params.size() - 1] == ')');
		trim(params, "()");

		auto fields = split(params, ',');
		assert(fields.size() == 3);
		string id     = fields[0];
		double weight = std::atof(fields[1].c_str());
		string cost   = fields[2];
		trim(cost, "$");

		if (weight <= total)
			{
			item i;
			i.id = std::atoi(id.c_str());
			i.weight = integer_weight(weight);
			i.value = std::atoi(cost.c_str());
			items.push_back(i);
			}
		}

	return items;
}

// trim left and right character(s)
auto trim(string &str, const string &chars) -> void
{
	auto right_trim = [chars](string & str)
		{
		size_t endpos = str.find_last_not_of(chars);
		if (string::npos != endpos)
			{
			str = str.substr(0, endpos + 1);
			}
		};

	auto left_trim = [chars](string & str)
		{
		size_t startpos = str.find_first_not_of(chars);
		if (string::npos != startpos)
			{
			str = str.substr(startpos);
			}
		};

	right_trim(str);
	left_trim(str);
}

// output a comma separated values list
auto output_csv(ostream &os, const vector<int> &ids) -> void
{
	if (ids.empty())
		{
		os << '-';
		}
	else
		{
		bool first = true;
		for (auto itr = begin(ids); itr != end(ids); ++itr, first = false)
			{
			if (first)
				os << *itr;
			else
				os << ',' << *itr;
			}
		}

	os << '\n';
}

// convert a 2 dec. place floating point weight to integer by scaling by 100
auto integer_weight(double weight) -> int
{
	return static_cast<int>(std::floor(weight * 100.0 + 0.1));
}

// the dynamic-programming knapsack algorithm
// [ The sample data passes, but the online submission fails.]
auto dynamic_knapsack(size_t W, vector<item> &items) -> vector<int>
{
	// sorting isn't supposed to be necessary, but doing so satisfies the sample data
	std::sort(begin(items), end(items), [](const item & a, const item & b)
		{
		if (a.value == b.value)
			return a.weight < b.weight;
		else
			return a.value > b.value;
		});

	size_t n = items.size();
	size_t rows = items.size() + 1;
	size_t cols = W + 1;

	vector<vector<int>> value;

	// construct and initialize
	for (size_t i = 0; i < rows; ++i)
		value.emplace_back(vector<int>(cols));
	for (size_t c = 0; c < cols; ++c)
		value[0][c] = 0;
	for (size_t r = 0; r < rows; ++r)
		value[r][0] = 0;

	// knapsack algorithm
	for (size_t i = 1; i <= n; ++i)
		{
		for (size_t j = 1; j <= W; ++j)
			{
			int item_weight = items[i - 1].weight;
			int item_value = items[i - 1].value;

			value[i][j] = value[i - 1][j];
			if ((size_t)(item_weight) <= j)
				{
				int val = value[i - 1][j - item_weight] + item_value;
				if (value[i][j] < val)
					value[i][j] = val;
				}
			}
		}

	return reconstruct_solution(value, W, items);
}

// build the solution from the dynamic-programming 2d array
auto reconstruct_solution(const vector<vector<int>> &value, size_t W, const vector<item> &items) -> vector<int>
{
	size_t r = value.size() - 1;
	size_t c = value[0].size() - 1;

	std::vector<bool> solution(r); // true if item was packed in the knapsack, false if not.

	while (r > 0)
		{
		bool item_used = false;

		int item_weight = items[r - 1].weight;
		int item_value = items[r - 1].value;

		if (value[r][c] >= item_value && c >= (size_t)item_weight)
			{
			int a = value[r - 1][c - item_weight];
			int b = value[r - 1][c];
			item_used = (a + item_value > b);
			}

		if (item_used)
			{
			solution[r - 1] = true;
			c = c - item_weight;
			r--;
			}
		else
			{
			solution[r - 1] = false;
			c = c;
			r--;
			}
		}

	// go through solution and sum the knapsack contents
	int knapsack_weight = 0;
	int knapsack_value = 0;
	vector<int> ids;

	for (size_t i = 0; i < solution.size(); ++i)
		{
		if (solution[i])
			{
			ids.push_back(items[i].id);
			knapsack_weight += items[i].weight;
			knapsack_value += items[i].value;
			}
		}

	// global check
	r = value.size() - 1;
	c = value[0].size() - 1;
	assert((size_t)knapsack_weight <= W);
	assert(knapsack_value == value[r][c]);

	sort(begin(ids), end(ids));
	return ids;
}

auto debug_output(int total, vector<item> &items, const vector<int> &solution) -> void
{
	cout << '\n';

	bool equal_cost = false;

	sort(begin(items), end(items), [&equal_cost](const item & a, const item & b)
		{
		if (a.value == b.value)
			{
			equal_cost = true;
			return a.weight < b.weight;
			}
		else
			{
			return a.value > b.value;
			}
		});

	if (equal_cost)
		cout << "* ";
	else
		cout << "  ";

	for (const auto &i : items)
		{
		cout << i.id << ": " << i.weight << " $" << i.value << "  |  ";
		}
	cout << '\n';

	auto pivot = std::partition(begin(items), end(items), [&solution](const item &i)
		{
		return std::find_if(begin(solution), end(solution), [id = i.id](int s)
			{
			return id == s;
			}) != end(solution);
		});

	int used_weight = std::accumulate(begin(items), pivot, 0, [](int value, const item &i) 
		{
		return value + i.weight;
		});

	int unused_weight = std::accumulate(pivot, end(items), 0, [](int value, const item &i)
		{
		return value + i.weight;
		});

	int remaining = total - used_weight;

	assert(used_weight <= total);

	cout << "  " << used_weight << " < " << total << " (" << remaining << ")\n";
}

