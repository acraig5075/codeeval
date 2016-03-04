// Nice learning opportunity for the following:
// std::next_permutation
// std::set_intersection
// std::iota
// pointers-to-struct-members
// using lambdas with std::sort and std::count_if

//Kuhn - Munkres algorithm
//Combinatorial optimization problems known as assignment problem

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>
#include <tuple>
#include <cctype>
#include <cassert>


using std::string;
using std::vector;
using std::map;
using std::tuple;


struct NameInfo
{
	string name;
	tuple<int, int, int> info; // letters, vowels, consonants

	NameInfo(const string &name) : name(name) {}
	friend bool operator<(const NameInfo& l, const NameInfo& r)
	{
		return l.name.compare(r.name) < 0;
	}

	int letters() const { return std::get<0>(info); }
	int vowels() const { return std::get<1>(info); }
	int consonants() const { return std::get<2>(info); }
};


struct Pairing
{
	size_t customer = 0;
	size_t product = 0;
	double score = 0.0;
};


struct Scenario
{
	double total = 0.0;
	vector<Pairing> candidates;
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

bool isvowel(char c)
{
	return (c == 65 || c == 69 || c == 73 || c == 79 || c == 85 || c == 89 || c == 97 || c == 101 || c == 105 || c == 111 || c == 117 || c == 121);
}

vector<NameInfo> name_info(const vector<string> &names)
{
	vector<NameInfo> ret;

	for (const auto &name : names)
	{
		int letters = 0;
		int vowels = 0;
		int consonants = 0;

		std::for_each(name.begin(), name.end(), [&](char c)
		{
			if (std::isalpha(c))
			{
				letters++;

				if (isvowel(c))
					vowels++;
				else
					consonants++;
			}
		});

		NameInfo info(name);
		info.info = std::make_tuple(letters, vowels, consonants);
		ret.push_back(info);
	}
	return ret;
}


// Factors of a number, exclusive of `1`.
vector<int> factors(int value)
{
	vector<int> ret;

	for (int test = 2; test <= value; ++test)
		{
		if (value % test == 0)
			ret.push_back(test);
		}

	return ret;
}


size_t factorial(size_t value)
{
	size_t ret = 1;
	while (value > 1)
	{
		ret *= value;
		value--;
	}
	return ret;
}

// Do two sets of values have any common elements?
template<typename T>
bool intersect(const vector<T> &set1, const vector<T> &set2)
{
	auto maximum = set1.size() + set2.size();
	vector<T> v(maximum);

	auto it = std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), v.begin());
	v.resize(it - v.begin());

	return !v.empty();
}


// The score given to a customer/product pairing.
double suitability_score(const NameInfo &customer, const NameInfo &product)
{
	double ss = 0.0;

	if (product.letters() % 2 == 0)
		ss = 1.5 * customer.vowels();
	else
		ss = 1.0 * customer.consonants();

	auto factors1 = factors(customer.letters());
	auto factors2 = factors(product.letters());

	if (intersect(factors1, factors2))
		ss *= 1.5;

	return ss;
}

// Get all possible combinations of elements of `smaller` with elements of `larger`.
vector<Scenario> combinations(const vector<size_t> &smaller, const vector<size_t> &larger, size_t Pairing::*field1, size_t Pairing::*field2)
{
	size_t perms = factorial(larger.size());
	vector<Scenario> scenarios;
	scenarios.reserve(perms);

	vector<size_t> permutable = larger;
	do
		{
		Scenario sc;
		for (const auto s : smaller)
			{
			// at this point we don't know which is a customer index and which is a product index, but "pointers-to-struct-members" assist beautifully. C++ is great! :)
			Pairing p;
			p.*field1 = s;
			p.*field2 = permutable[s];
			sc.candidates.push_back(p);
			}

		scenarios.push_back(sc);
		}
	while (std::next_permutation(permutable.begin(), permutable.end()));

	return scenarios;
}


// This brings it all together. For one test `input` get the maximised total suitability score.
double compute(const string &input)
{
	// tokenise the test data
	vector<string> discounts = split(input, ';');
	vector<string> customerNames = split(discounts.at(0), ',');
	vector<string> productNames = split(discounts.at(1), ',');

	// get the letters, vowels and consonants of all names
	vector<NameInfo> nameInfo = name_info(customerNames);
	vector<NameInfo> productInfo = name_info(productNames);
	nameInfo.insert(nameInfo.end(), productInfo.begin(), productInfo.end());
	std::sort(nameInfo.begin(), nameInfo.end());

	size_t numCustomers = customerNames.size();
	size_t numProducts = productNames.size();

	// get arrays of zero-based indexes into `customerNames` and `productNames`
	vector<size_t> customers(numCustomers);
	vector<size_t> products(numProducts);

	std::iota(customers.begin(), customers.end(), 0);
	std::iota(products.begin(), products.end(), 0);

	// a scenario is one possible way of giving customers a discount product.
	// so now get all combinations of scenarios
	vector<Scenario> scenarios;
	if (numProducts >= numCustomers)
		scenarios = combinations(customers, products, &Pairing::customer, &Pairing::product);
	else
		scenarios = combinations(products, customers, &Pairing::product, &Pairing::customer);

	int x = 1;
	// calculate the suitability of each pair, and the total score for each scenario
	for (Scenario &sc : scenarios)
		{
		sc.total = 0.0;

		for (Pairing &p : sc.candidates)
			{
			const auto &itr1 = std::lower_bound(nameInfo.begin(), nameInfo.end(), NameInfo(customerNames.at(p.customer)));
			const auto &itr2 = std::lower_bound(nameInfo.begin(), nameInfo.end(), NameInfo(productNames.at(p.product)));
			assert(itr1 != nameInfo.end());
			assert(itr2 != nameInfo.end());

			p.score = suitability_score(*itr1, *itr2);
			sc.total += p.score;
			}
		}

	// return the total score for the scenario having the greatest total score
	std::sort(scenarios.begin(), scenarios.end(), [](const Scenario & a, const Scenario & b)
		{
		return b.total < a.total;
		});
	return scenarios.begin()->total;
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

				if (!line.empty())
					{
					// do the calculation and print the result
					double score = compute(line);
					std::cout.precision(2);
					std::cout << std::fixed << score << "\n";
					}
				}
			}
		}

	return 0;
}
