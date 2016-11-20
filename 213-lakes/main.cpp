#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <cassert>
#include <algorithm>

using std::string;
using std::vector;
using std::set;

vector<set<size_t>> lakes;

// Tokenise a string.
vector<string> split(const string &value, char delimiter)
{
	vector<string> ret;

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

// for debugging purposes
void print_lakes()
{
	std::cout << "{\n";
	for (auto lake : lakes)
	{
		std::cout << "    {    ";
		for (auto c : lake)
		{
			std::cout << c << " ";
		}
		std::cout << "   }\n";

	}
	std::cout << "}\n";
}

// Three forest strips at a time allow to inspect all around each cell
void process(const string &row0, const string &row1, const string &row2, size_t r0_id, size_t r1_id, size_t r2_id, size_t width)
{
	r0_id *= width;
	r1_id *= width;
	r2_id *= width;

	auto surrounding_all = [&](size_t c)
	{
		vector<size_t> adj;

		auto surrounding_one = [&](size_t c, const string &row, size_t row_start)
		{
			if (!row.empty())
			{
				if (c > 0 && row.at(c - 1) == 'o')
					adj.push_back(row_start + c - 1);
				if (row.at(c) == 'o')
					adj.push_back(row_start + c);
				if (c < width - 1 && row.at(c + 1) == 'o')
					adj.push_back(row_start + c + 1);
			}
		};

		surrounding_one(c, row0, r0_id);
		surrounding_one(c, row1, r1_id);
		surrounding_one(c, row2, r2_id);

		return adj;
	};

	for (size_t c = 0; c < width; ++c)
	{
		if (row1.at(c) == 'o')
		{
			vector<size_t> adjacent = surrounding_all(c);

			vector<int> overlaps;
			int index = 0;
			for (const auto &lake : lakes)
			{
				auto is_part_of_lake = [&lake](size_t val)
				{
					return std::find(begin(lake), end(lake), val) != end(lake);
				};

				if (std::any_of(begin(adjacent), end(adjacent), is_part_of_lake))
				{
					overlaps.push_back(index);
				}

				index++;
			}

			if (overlaps.empty())
			{
				// make new lake from adjacent
				set<size_t> lake;
				lake.insert(begin(adjacent), end(adjacent));
				lakes.push_back(lake);
			}
			else if (overlaps.size() == 1)
			{
				// append adjacent to existing lake
				auto &lake = lakes.at(overlaps.at(0));
				lake.insert(begin(adjacent), end(adjacent));
			}
			else
			{
				// merge lakes together, and append adjacent to the result
				assert(overlaps.size() == 2);
				auto &lake1 = lakes.at(overlaps.at(0));
				auto &lake2 = lakes.at(overlaps.at(1));
				lake1.insert(begin(lake2), end(lake2));
				lake1.insert(begin(adjacent), end(adjacent));
				auto itr = begin(lakes) + overlaps.at(1);
				lakes.erase(itr);
			}
		}
	}
	//print_lakes();
}

// Do the test for a given input line
int test_case(const string &input)
{
	lakes.clear();

	vector<string> rows = split(input, '|');

	auto minmax = std::minmax_element(begin(rows), end(rows), [](const string &lhs, const string &rhs)
	{
		return lhs.size() < rhs.size();
	});

	size_t min_width = minmax.first->size();
	size_t max_width = minmax.second->size();
	size_t width = min_width;
	if (min_width != max_width)
		return -1;

	string row0 = "";
	string row1 = rows.at(0);
	string row2;

	size_t r0_id = 0;
	size_t r1_id = 0;
	size_t r2_id = 1;

	for (size_t r = 1; r <= rows.size(); ++r)
	{
		if (r == rows.size())
			row2 = "";
		else
			row2 = rows.at(r);

		process(row0, row1, row2, r0_id, r1_id, r2_id, width);

		row0 = row1;
		row1 = row2;

		r0_id = r1_id;
		r1_id = r2_id;
		r2_id++;
	}

	return static_cast<int>(lakes.size());
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		string filename(argv[1]);
		std::ifstream fin(filename.c_str());

		// read the file
		if (fin.is_open())
		{
			string line;
			while (fin.good())
			{
				std::getline(fin, line);

				line.erase(std::remove(begin(line), end(line), ' '), end(line));

				if (!line.empty())
				{
					int num_lakes = test_case(line);

					if (-1 == num_lakes)
						{
						std::cout << line << "\n";
						assert(false);
						}

					std::cout << num_lakes << "\n";
				}
			}
		}
	}

	return 0;
}

