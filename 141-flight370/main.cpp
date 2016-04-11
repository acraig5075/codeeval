#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

using std::string;
using std::vector;

struct Test
{
	double m_radius = 0.0;
	double m_longitude = 0.0;
	double m_latitude = 0.0;

	Test(double radius, double longitude, double latitude)
		: m_radius(radius), m_longitude(longitude), m_latitude(latitude)
	{}
};

struct Placemark
{
	string m_id;
	string m_name;
	string m_timeStamp;
	string m_coordinates;
	int m_confirmations = 0;
	double m_longitude = 0.0;
	double m_latitude = 0.0;
	double m_test = 0.0;

	bool isValid() const
	{
		return
			!m_id.empty() &&
			!m_name.empty() &&
			!m_timeStamp.empty() &&
			!m_coordinates.empty();
	}
};

namespace Utils
{
	// tokenise a string.
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

	// trim trailing characters
	void right_trim(string &str, const string &chars)
	{
		size_t endpos = str.find_last_not_of(chars);
		if (string::npos != endpos)
		{
			str = str.substr(0, endpos + 1);
		}
	}

	// trim leading characters
	void left_trim(string &str, const string &chars)
	{
		size_t startpos = str.find_first_not_of(chars);
		if (string::npos != startpos)
		{
			str = str.substr(startpos);
		}
	}

	// extract id attribute from Placemark xml element
	string placemarkID(const string &line)
	{
		auto start = line.find_first_of(R"(')", 0);
		auto end = line.find_first_of(R"(')", start + 1);
		return line.substr(start + 1, end - start - 1);
	}

	// extract the associated text if `line` contains `tag` element and assign to the pointed-at member variable
	bool elementText(const string &line, const string &tag, Placemark &placemark, string Placemark::*field)
	{
		string openTag = string("<") + tag + ">";
		string closeTag = string("</") + tag + ">";
		auto start = line.find(openTag, 0);
		auto end = line.find(closeTag, start);

		if (string::npos != start && string::npos != end)
		{
			placemark.*field = line.substr(start + openTag.length(), end - start - openTag.length());
			return true;
		}
		return false;
	}

	// convert degrees to radians
	double deg_rad(double degrees)
	{
		return degrees * M_PI / 180.0;
	}

	// haversine formula for great circle distance assuming spherical Earth
	double haversine(double lon1, double lat1, double lon2, double lat2)
	{
		double half_dlon = (lon2 - lon1) / 2.0;
		double half_dlat = (lat2 - lat1) / 2.0;
		return 2.0 * 6371.0 * asin(sqrt(pow(sin(half_dlat), 2.0) + cos(lat1) * cos(lat2) * pow(sin(half_dlon), 2.0)));
	}

}

struct TestReader
{
	vector<Test> &m_tests;

	TestReader(vector<Test> &tests)
		: m_tests(tests)
	{}

	void read(std::istream &in)
	{
		string line;
		while (in.good())
		{
			std::getline(in, line);
			if (!line.empty())
			{
				if (line.compare(R"(<?xml version='1.0' encoding='UTF-8'?>)") == 0)
				{
					return;
				}
				else
				{
					makeTest(line);
				}
			}
		}
	}

	void makeTest(const string &line)
	{
		auto tokens = Utils::split(line, ';');
		if (tokens.size() == 2)
		{
			auto coordinates = Utils::split(tokens.at(1), ',');
			if (coordinates.size() == 2)
			{
				string location1 = coordinates.at(0);
				string location2 = coordinates.at(1);
				Utils::left_trim(location1, " (");
				Utils::right_trim(location2, " )");

				double spatial[] =
				{
					atof(tokens.at(0).c_str()), // radius
					atof(location1.c_str()), // longitude
					atof(location2.c_str()) // latitude
				};

				if (std::all_of(std::begin(spatial), std::end(spatial), [](double val)
				{
					return val != 0;
				}))
				{
					m_tests.push_back({ spatial[0], Utils::deg_rad(spatial[1]), Utils::deg_rad(spatial[2]) });
				}
			}
		}
	}
};

struct KmlReader
{
	vector<Placemark> &m_placemarks;
	bool m_busyPlacemark = false;
	Placemark m_current;

	KmlReader(vector<Placemark> &tests)
		: m_placemarks(tests)
	{}

	void addCurrent()
	{
		if (m_current.isValid())
			m_placemarks.push_back(m_current);
		m_current = {};
	}

	void read(std::istream &in)
	{
		string line;
		while (in.good())
		{
			std::getline(in, line);
			if (!line.empty())
			{
				if (string::npos != line.find("/Placemark>"))
				{
					addCurrent();
					m_busyPlacemark = false;
				}
				else if (string::npos != line.find("<Placemark"))
				{
					m_busyPlacemark = true;
					m_current.m_id = Utils::placemarkID(line);
				}
				else if (m_busyPlacemark)
				{
					readPlacemark(line);
				}
			}
		}
	}
	
	void readPlacemark(const string &line)
	{
		if (Utils::elementText(line, "name", m_current, &Placemark::m_name))
			return;
		if (Utils::elementText(line, "when", m_current, &Placemark::m_timeStamp))
			return;
		if (Utils::elementText(line, "coordinates", m_current, &Placemark::m_coordinates))
			return;

		auto confirm = line.find("Confirmation:", 0);
		if (string::npos != confirm)
		{
			string openTag = string("<b>");
			string closeTag = string("</b>");
			auto start = line.find(openTag, confirm);
			auto end = line.find(closeTag, start);

			if (string::npos != start && string::npos != end)
			{
				m_current.m_confirmations = atoi(line.substr(start + openTag.length(), end - start - openTag.length()).c_str());
			}
		}
	}
};

struct CoordinatesFunctor
{
	void operator()(Placemark &placemark)
	{
		auto values = Utils::split(placemark.m_coordinates, ',');
		placemark.m_longitude = Utils::deg_rad(atof(values.at(0).c_str()));
		placemark.m_latitude = Utils::deg_rad(atof(values.at(1).c_str()));
	}
};

struct HaversineFunctor
{
	const Test &m_test;

	HaversineFunctor(const Test &test)
		: m_test(test)
	{}

	void operator()(Placemark &placemark)
	{
		placemark.m_test = Utils::haversine(m_test.m_longitude, m_test.m_latitude, placemark.m_longitude, placemark.m_latitude);
	}
};

void print(const string &id, const vector<Placemark> &placemarks)
{
	auto it = std::find_if(placemarks.begin(), placemarks.end(), [&id](const Placemark &p)
	{
		return p.m_id == id;
	});

	if (it != placemarks.end())
		std::cout << it->m_id << " " << it->m_confirmations << " " << it->m_test << "\n";
}

void testCase(const Test &test, vector<Placemark> &placemarks)
{
	// Compute the test distance for each placemark
	HaversineFunctor hf(test);
	std::for_each(placemarks.begin(), placemarks.end(), hf);

	// Sort placemarks by increasing test distance
	std::sort(placemarks.begin(), placemarks.end(), [](const Placemark &lhs, const Placemark &rhs)
	{
		return lhs.m_test < rhs.m_test;
	});

	//print("3536615", placemarks);
	//print("6757595", placemarks);

	// Search for the first placemark that doesn't satisfy the radius test
	Placemark search;
	search.m_test = test.m_radius;
	auto itr = std::lower_bound(placemarks.begin(), placemarks.end(), search, [](const Placemark &lhs, const Placemark &rhs)
	{
		return lhs.m_test < rhs.m_test;
	});

	// Number of potential candidates to consider
	size_t numCandidates = std::distance(placemarks.begin(), itr);

	// Sort only these candidates by confirmations, then by timestamp, then by Id
	std::sort(placemarks.begin(), itr, [](const Placemark &lhs, const Placemark &rhs)
	{
		if (lhs.m_confirmations == rhs.m_confirmations)
		{
			if (rhs.m_timeStamp.compare(lhs.m_timeStamp) == 0)
				return lhs.m_id.compare(rhs.m_id) < 0;
			else
				return lhs.m_timeStamp.compare(rhs.m_timeStamp) > 0;
		}
		else
		{
			return lhs.m_confirmations > rhs.m_confirmations;
		}
	});

	if (numCandidates > 0)
	{
		auto endItr = placemarks.begin() + numCandidates;

		// Search for end of placemarks with same number of confirmations
		Placemark search2 = placemarks.at(0);
		auto itr2 = std::lower_bound(placemarks.begin(), endItr, search2, [](const Placemark &lhs, const Placemark &rhs)
		{
			return lhs.m_confirmations >= rhs.m_confirmations;
		});

		if (itr2 != endItr)
		{
			bool first = true;
			std::for_each(placemarks.begin(), itr2, [&](const Placemark &place)
			{
				if (first)
					std::cout << place.m_name;
				else
					std::cout << ", " << place.m_name;
				first = false;
			});
			std::cout << "\n";
		}
	}
	else
	{
		std::cout << "None\n";
	}
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		vector<Test> tests;
		TestReader testReader(tests);

		vector<Placemark> placemarks;
		KmlReader kmlReader(placemarks);

		// read the file in two stages: first the tests, then the kml data
		std::ifstream fin(argv[1]);
		if (fin.is_open())
		{
			testReader.read(fin);
			kmlReader.read(fin);
			fin.close();
		}

		// parse the coordinate string into radian lon and lat
		std::for_each(placemarks.begin(), placemarks.end(), CoordinatesFunctor());

		// query each test case
		for (auto &test : tests)
		{
			testCase(test, placemarks);
		}
	}

	return 0;
}


