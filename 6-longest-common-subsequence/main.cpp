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

auto split(const string &value, char delimiter) -> vector<string>;
auto lcs(string &str1, string &str2) -> string;
auto table(string &str1, string &str2) -> vector<vector<int>>;
auto reconstruct(const vector<vector<int>> &d, const string &a, const string &b) -> string;
auto print(vector<vector<int>> distances) -> void;


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
					cout << lcs(str1, str2) << '\n';
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

// longest common subsequence
auto lcs(string &str1, string &str2) -> string
{
	vector<vector<int>> distances = table(str1, str2);

	return reconstruct(distances, str1, str2);
}

// build the dynamic programming edit distance table
auto table(string &str1, string &str2) -> vector<vector<int>>
{
	size_t rows = str1.size() + 1;
	size_t cols = str2.size() + 1;

	vector<vector<int>> distances;

	// construct and initialize
	for (size_t r = 0; r < rows; ++r)
		distances.emplace_back(vector<int>(cols));
	for (size_t c = 0; c < cols; ++c)
		distances[0][c] = 0;
	for (size_t r = 0; r < rows; ++r)
		distances[r][0] = 0;

	// fill
	for (size_t i = 1; i < rows; ++i)
		{
		for (size_t j = 1; j < cols; ++j)
			{
			int a = distances[i - 1][j - 1] + 1;
			int b = distances[i - 1][j];
			int c = distances[i][j - 1];

			if (str1[i - 1] == str2[j - 1])
				distances[i][j] = a;
			else
				distances[i][j] = std::max(b, c);
			}
		}

	return distances;
}

// backtrack through the distance table for the lcs
auto reconstruct(const vector<vector<int>> &d, const string &a, const string &b) -> string
{
	string lcs;

	size_t i = d.size() - 1;
	size_t j = d[0].size() - 1;

	while (i > 0 && j > 0)
		{
		if (a[i - 1] == b[j - 1])
			{
			lcs.append(1, a[i - 1]);
			i--;
			j--;
			}
		else if (d[i - 1][j] > d[i][j - 1])
			{
			i--;
			}
		else
			{
			j--;
			}
		}

	std::reverse(lcs.begin(), lcs.end());
	return lcs;
}

// debug print the table
auto print(vector<vector<int>> distances) -> void
{
	size_t rows = distances.size();
	size_t cols = distances[0].size();

	for (size_t j = 0; j < rows; ++j)
		{
		for (size_t i = 0; i < cols; ++i)
			{
			cout << distances[j][i] << " ";
			}
		cout << '\n';
		}
	cout << '\n';
}
