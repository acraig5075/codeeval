#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>

using std::vector;
using std::string;
using std::ifstream;
using std::ostream;
using std::cout;

struct item
{
	size_t id = 0;
	double weight = 0.0;
	int value = 0;
	//double ratio = 0.0;
};


auto split(const string &value, char delimiter) -> vector<string>;
auto parse(const string &line, double &weight) -> vector<item>;
auto trim(string &str, const string &chars) -> void;
auto greedy_knapsack(double &capacity, vector<item> &items) -> vector<size_t>;
auto output_csv(ostream &os, const vector<size_t> &ids) -> void;
auto integer_weight(double weight) -> int;


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
					double total = 0;
					vector<item> items = parse(line, total);
					vector<size_t> package = greedy_knapsack(total, items);
					output_csv(cout, package);
					}
				}
			}
		}

	return 0;
}

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

auto parse(const string &line, double &weight) -> vector<item>
{
	auto tokens = split(line, ':');
	assert(tokens.size() == 2);
	trim(tokens[0], " ");
	trim(tokens[1], " ");

	weight = std::atof(tokens[0].c_str());

	vector<item> items;
	tokens = split(tokens[1], ' ');

	for (const auto &p : tokens)
		{
		string params = p;
		assert(params[0] == '(' && params[params.size() - 1] == ')');
		trim(params, "()");

		auto fields = split(params, ',');
		assert(fields.size() == 3);
		string id = fields[0];
		string weight = fields[1];
		string cost = fields[2];
		trim(cost, "$");

		item i;
		i.id = std::atoi(id.c_str());
		i.weight = std::atof(weight.c_str());
		i.value = std::atoi(cost.c_str());
		items.push_back(i);
		}

	return items;
}

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

auto greedy_knapsack(double &capacity, vector<item> &items) -> vector<size_t>
{
	//std::for_each(begin(items), end(items), [](item & i)
	//	{
	//	i.ratio = i.value / i.weight;
	//	});

	std::sort(begin(items), end(items), [](const item & a, const item & b)
		{
		if (a.value == b.value)
			return a.weight < b.weight;
		else
			return a.value > b.value;
		});

	double value = 0.0;
	vector<size_t> knapsack;

	auto itr = begin(items);

	while (capacity > 0 && itr != end(items))
		{
		if (itr->weight <= capacity)
			{
			// found something that fits, so add to the knapsack
			value += itr->value;
			capacity -= itr->weight;
			knapsack.push_back(itr->id);
			}
		//else
		//	{
		//	// nothing fits, so add a fraction of the most valuable
		//	value += itr->ratio * capacity;
		//	capacity = 0;
		//	}

		++itr;
		}

	return knapsack;
}

auto output_csv(ostream &os, const vector<size_t> &ids) -> void
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

auto integer_weight(double weight) -> int
{
	return static_cast<int>(std::floor(weight * 100.0 + 0.1));
}
