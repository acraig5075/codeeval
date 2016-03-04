// Alasdair Craig
// 22/02/2016
// https://www.codeeval.com/open_challenges/14/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using std::string;
using std::vector;


// Perform a generic operation on each line in a file
void forEachLineInFile(const string &filename, void(*processFunc)(const string &line));

// Our generic function
void processLine(const string &value);

// Get a list of permutations for the given string value
vector<string> getPermutations(string &value);

// Sort a list of strings
void sortStrings(vector<string> &list, bool(*sortFunc)(const string &, const string &));

// Sorting predicate: digits < upper case letters < lower case letters
bool sortByDigitUpperLower(const string &, const string &);

// Output a list of strings, comma delimited
void csvOutput(const vector<string> &list, std::ostream &out);


// the main function
int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		string filename(argv[1]);
		forEachLineInFile(filename, &processLine);
	}
	return 0;
}

void forEachLineInFile(const string &filename, void(*processFunc)(const string &line))
{
	if (processFunc)
	{
		std::ifstream fin(filename.c_str());
		if (fin.is_open())
		{
			string line;
			while (fin.good())
			{
				std::getline(fin, line);
				(*processFunc)(line);
			}
			fin.close();
		}
	}
}

void processLine(const string &line)
{
	string value = line.substr(0, line.find_last_not_of(' ') + 1); // remove any trailing spaces just in case
	if (!value.empty())
	{
		vector<string> perms = getPermutations(value);
		sortStrings(perms, &sortByDigitUpperLower);
		csvOutput(perms, std::cout);
	}
}

vector<string> getPermutations(string &value)
{
	std::sort(value.begin(), value.end());

	vector<string> ret = { value };
	while (std::next_permutation(value.begin(), value.end())) // std library algorithm does the hard work for us!
		ret.push_back(value);

	return ret;
}

void sortStrings(vector<string> &list, bool(*sortFunc)(const string &, const string &))
{
	if (sortFunc)
		std::sort(list.begin(), list.end(), sortFunc);
}

bool sortByDigitUpperLower(const string &s1, const string &s2)
{
	return s1.compare(s2) < 0; // std::string::compare() just happens to sort in the order we want!
}

void csvOutput(const vector<string> &list, std::ostream &out)
{
	if (!list.empty())
	{
		string formatted;
		for (const auto &s : list)
		{
			formatted += s;
			formatted += ',';
		}
		out << formatted.substr(0, formatted.size() - 1) << "\n"; // output with trailing comma removed
	}
}