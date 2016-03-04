// minesweeper.cpp : Defines the entry point for the console application.
//

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using std::vector;
using std::string;
using std::cout;

struct Element
{
	bool mine = false;
	int value = 0;
};

class Grid
{
public:
	Grid(size_t m, size_t n) : m(m), n(n)
	{
		m_data.reserve(m);
		for (size_t i = 0; i < m; i++)
		{
			vector<Element> row;
			row.reserve(n);

			for (size_t j = 0; j < n; j++)
				row.push_back({});

			m_data.push_back(row);

		}
	}

	// Populate the grid with mines from textual description
	void read(const string &input)
	{
		vector<vector<Element>>::iterator row = m_data.begin();
		vector<Element>::iterator col = row->begin();

		for (char c : input)
		{
			if (c == '*')
				col->mine = true;

			col++;
			if (col == row->end())
			{
				row++;
				if (row == m_data.end())
					break;

				col = row->begin();
			}
		}
	}

	// Populate the grid with non-mine values
	void update()
	{
		size_t row = 0;
		for (const auto &r : m_data)
		{
			size_t col = 0;
			for (const auto &c : r)
			{
				if (c.mine)
				{
					around(row, col);
				}
				col++;
			}
			row++;
		}
	}

	// Increment the neighbouring values of a mine
	void around(size_t row, size_t col)
	{
		auto increment = [this](size_t r, size_t c)
		{
			if (r >= 0 && r < m)
			{
				if (c >= 0 && c < n)
				{
					Element &ele = m_data.at(r).at(c);
					if (!ele.mine)
						ele.value++;
				}
			}
		};

		increment(row - 1, col - 1);
		increment(row - 1, col);
		increment(row - 1, col + 1);
		increment(row, col - 1);
		increment(row, col + 1);
		increment(row + 1, col - 1);
		increment(row + 1, col);
		increment(row + 1, col + 1);
	}

	// Formatted console output
	void print() const
	{
		for (const auto &r : m_data)
		{
			for (const auto &c : r)
			{
				if (c.mine)
					cout << '*';
				else
					cout << c.value;
			}
		}
		cout << "\n";
	}

private:
	size_t m = 0; // num rows
	size_t n = 0; // num cols
	vector<vector<Element>> m_data; // data
};

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

// Do the test for a given input line
void process(const string &input)
{
	vector<string> parts = split(input, ';');
	vector<string> dims = split(parts.at(0), ',');
	string data = parts.at(1);

	size_t m = std::atoi(dims.at(0).c_str());
	size_t n = std::atoi(dims.at(1).c_str());

	Grid grid(m, n);
	grid.read(data);
	grid.update();
	grid.print();
}

/*
Example sample data:
3,5;**.........*...
4,4;*........*......
*/

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

				if (!line.empty())
				{
					process(line);
				}
			}
		}
	}

	return 0;
}

