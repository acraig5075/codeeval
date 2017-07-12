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
auto sequence_alignment(string &str1, string &str2) -> void;
auto reconstruct(size_t i, size_t j, const vector<vector<int>> &d, const string &a, const string &b, string &a_, string &b_) -> void;
auto output_matches(const string &str1, const string &str2) -> void;


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
					sequence_alignment(str1, str2);
					cout << str1 << '\n' << str2 << '\n';
					output_matches(str1, str2);
					cout << '\n';
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

auto sequence_alignment(string &str1, string &str2) -> void
{
	size_t rows = str1.size() + 1;
	size_t cols = str2.size() + 1;

	vector<vector<int>> distances;

	// construct and initialize
	for (size_t r = 0; r < rows; ++r)
		distances.emplace_back(vector<int>(cols));
	for (size_t c = 0; c < cols; ++c)
		distances[0][c] = (int)c;
	for (size_t r = 0; r < rows; ++r)
		distances[r][0] = (int)r;

	// fill
	for (size_t j = 1; j < cols; ++j)
	{
		for (size_t i = 1; i < rows; ++i)
		{
			int insertion = distances[i][j - 1] + 1;
			int deletion = distances[i - 1][j] + 1;
			int match = distances[i - 1][j - 1];
			int mismatch = distances[i - 1][j - 1] + 1;

			if (str1[i - 1] == str2[j - 1])
				distances[i][j] = std::min({ insertion, deletion, match });
			else
				distances[i][j] = std::min({ insertion, deletion, mismatch });
		}
	}

	// reconstruct optimal alignment
	string outstr1, outstr2;
	reconstruct(rows - 1, cols - 1, distances, str1, str2, outstr1, outstr2);

	str1 = outstr1;
	str2 = outstr2;
}

auto reconstruct(size_t i, size_t j, const vector<vector<int>> &d, const string &a, const string &b, string &a_, string &b_) -> void
{
	if (i == 0 && j == 0)
	{
		return;
	}
	if (i > 0 && d[i][j] == d[i - 1][j] + 1) // entered cell vertically (insertion)
	{
		reconstruct(i - 1, j, d, a, b, a_, b_);
		a_ += a[i - 1];
		b_ += "-";
	}
	else if (j > 0 && d[i][j] == d[i][j - 1] + 1) // entered cell horizontally (deletion)
	{
		reconstruct(i, j - 1, d, a, b, a_, b_);
		a_ += "-";
		b_ += b[j - 1];
	}
	else // match or mismatch
	{
		reconstruct(i - 1, j - 1, d, a, b, a_, b_);
		a_ += a[i - 1];
		b_ += b[j - 1];
	}
}

auto output_matches(const string &str1, const string &str2) -> void
{
	assert(str1.size() == str2.size());
	for (size_t i = 0; i < std::min(str1.size(), str2.size()); ++i)
	{
		if (str1[i] == str2[i])
			cout << str1[i];
	}
	cout << '\n';
}