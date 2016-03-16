#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

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
					m_tests.push_back({ spatial[0], spatial[1], spatial[2] });
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
	}
};

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
		}
	}

	return 0;
}

