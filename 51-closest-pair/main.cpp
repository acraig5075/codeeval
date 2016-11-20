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
//#include <cfenv>

using std::begin;
using std::end;

using Coordinate = std::pair<long long int, long long int>;

using CoordinateVector = std::vector <Coordinate> ;

double squared_distance(const Coordinate &a, const Coordinate &b)
{
	//std::feclearexcept(FE_ALL_EXCEPT);

	double dx = static_cast<double>(a.first - b.first);
	double dy = static_cast<double>(a.second - b.second);
	double dx2 = dx * dx;
	double dy2 = dy * dy;
	return dx2 + dy2;

	//if (std::fetestexcept(FE_OVERFLOW))
	//	return std::numeric_limits<double>::max();
	//else
	//	return dist2;
}

void subtract_smallest(CoordinateVector &coordinates)
{
	Coordinate smallest;
	smallest.first = std::numeric_limits<long long int>::max();
	smallest.second = std::numeric_limits<long long int>::max();

	for (const Coordinate &c : coordinates)
		{
		if (c.first < smallest.first)
			smallest.first = c.first;
		if (c.second < smallest.second)
			smallest.second = c.second;
		}

	for (Coordinate &c : coordinates)
		{
		c.first -= smallest.first;
		c.second -= smallest.second;
		}
}

double process(const CoordinateVector &coordinates)
{
	double shortest_distance = std::numeric_limits<double>::max();

	CoordinateVector::const_iterator a = begin(coordinates);
	CoordinateVector::const_iterator b = a;
	b++;

	while (true)
		{
		while (b != end(coordinates))
			{
			double distance = squared_distance(*a, *b);
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

Coordinate parse_coord(const std::string &text)
{
	auto num_spaces = std::count_if(begin(text), end(text), [](char c)
		{
		return std::isblank(c);
		});
	assert(num_spaces == 1);

	Coordinate ret;
	size_t space = text.find(" ");

	assert(space != text.npos);
	if (num_spaces == 1 && space != text.npos)
		{
		ret.first = atoll(text.substr(0, space).c_str());
		ret.second = atoll(text.substr(space).c_str());
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
				subtract_smallest(coordinates);

				double distance = process(coordinates);

				if (distance < 10000.f)
					std::cout << std::fixed << std::setprecision(4) << distance << "\n";
				else
					std::cout << "INFINITY\n";
				}
			}
		}

	return 0;
}

