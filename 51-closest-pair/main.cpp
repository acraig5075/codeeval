// 51-closest-pair.cpp : Defines the entry point for the console application.
//
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>

using std::begin;
using std::end;

using CoordinateVector = std::vector <std::pair<int, int>> ;

float squared_distance(const std::pair<int, int> &a, const std::pair<int, int> &b)
{
	int dx = a.first - b.first;
	int dy = a.second - b.second;
	return static_cast<float>((dx * dx) + (dy * dy));
}

float process(const CoordinateVector &coordinates)
{
	float shortest_distance = std::numeric_limits<float>::max();

	CoordinateVector::const_iterator a = begin(coordinates);
	CoordinateVector::const_iterator b = a;
	b++;

	while (true)
		{
		while (b != end(coordinates))
			{
			float distance = squared_distance(*a, *b);
			if (distance < shortest_distance)
				shortest_distance = distance;

			b++;
			}

		a++;
		b = a;
		b++;
		if (b == end(coordinates))
			break;
		}

	return std::sqrt(shortest_distance);
}

std::pair<int, int> parse_coord(const std::string &text)
{
	std::pair<int, int> ret;
	size_t space = text.find(" ");

	assert(space != text.npos);
	if (space != text.npos)
		{
		ret.first = atoi(text.substr(0, space).c_str());
		ret.second = atoi(text.substr(space).c_str());
		}

	return ret;
}

bool is_number(const std::string &s)
{
	return !s.empty() && std::find_if(s.begin(), s.end(), [](char c)
		{
		return !std::isdigit(c);
		}) == s.end();
}

bool read_next_set(std::ifstream &fin, CoordinateVector &coords)
{
	if (fin.good())
		{
		std::string line;
		std::getline(fin, line);

		bool valid = is_number(line);
		assert(valid);

		if (valid)
			{
			int n = std::atoi(line.c_str());
			if (n == 0)
				return false;

			if (n > 0)
				{
				coords.clear();
				coords.reserve(n);

				while (fin.good())
					{
					assert(n > 0);

					std::getline(fin, line);

					auto c = parse_coord(line);
					coords.push_back(c);

					n--;
					if (n == 0)
						return true;
					}

				assert(false);
				return false;
				}
			else
				{
				assert(false);
				return false;
				}
			}
		else
			{
			assert(false);
			return false;
			}
		}
	else
		{
		assert(false);
		return false;
		}
}

int main(int argc, char *argv[])
{
	if (argc > 1)
		{
		std::string filename(argv[1]);
		std::ifstream fin(filename.c_str());

		if (fin.is_open())
			{
			CoordinateVector coordinates;

			while (read_next_set(fin, coordinates))
				{
				float distance = process(coordinates);

				if (distance < 10000.f)
					std::cout << std::fixed << std::setprecision(4) << distance << "\n";
				else
					std::cout << "INFINITY\n";
				}
			}
		}

	return 0;
}

