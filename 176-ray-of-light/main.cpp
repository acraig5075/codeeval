// 176-ray-of-light.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <array>
#include <cassert>
#include <vector>

// Define this for debug formatted output. Undefine for codeeval submission
#define delimited_output

constexpr size_t room_width = 10;
constexpr size_t room_height = 10;

constexpr size_t top_wall = 0;
constexpr size_t left_wall = 0;
constexpr size_t right_wall = room_width - 1;
constexpr size_t bottom_wall = room_height - 1;

struct Room
{
	std::array<std::array<char, room_width>, room_height> cells;

	const char& at(size_t x, size_t y) const
	{
		return cells.at(y).at(x);
	}

	char& at(size_t x, size_t y)
	{
		return cells.at(y).at(x);
	}
};

std::istream &operator >>(std::istream &in, Room &room)
{
	std::string line;
	std::getline(in, line);
	assert(line.length() == room_width * room_height);

	if (line.length() <= room_width * room_height)
	{
		const char *buffer = line.c_str();

		for (size_t c = 0; c < line.length(); ++c, ++buffer)
		{
			size_t y = c / room_height;
			size_t x = c % room_width;
			room.at(x, y) = *buffer;
		}
	}

	return in;
}

std::ostream &operator<< (std::ostream &out, const Room &room)
{
	for (size_t y = 0; y < room_height; ++y)
	{
		for (size_t x = 0; x < room_width; ++x)
		{
			out << room.at(x, y);
		}

#if defined delimited_output
		out << "\n";
#endif
	}
	out << "\n";
	return out;
}

enum class Heading { NorthEast, NorthWest, SouthWest, SouthEast, None };

struct Photon
{
	Photon() = default;
	Photon(size_t x, size_t y, Heading heading)
		: x(x), y(y), heading(heading)
	{}

	size_t x = room_width;
	size_t y = room_height;
	Heading heading = Heading::None;
};

using RayOfLight = std::vector<Photon>;

class Simulation
{
public:
	Simulation(Room &room)
		: room(room)
	{}

	void run()
	{
		Photon start = get_start_photon();

		if (start.x < room_width && start.y < room_height && start.heading != Heading::None)
		{
			RayOfLight current;
			current.push_back(start);
			rays.push_back(current);

			while (num_completed < rays.size())
			{
				track(rays.at(num_completed));
			}
		}
	}

	void track(RayOfLight &ray)
	{
		num_completed++;
	}


private:
	Photon get_start_photon()
	{
		for (size_t y = 0; y < room_height; ++y)
		{
			for (size_t x = 0; x < room_width; ++x)
			{
				if (x == left_wall || y == top_wall || x == right_wall || y == bottom_wall)
				{
					char c = room.at(x, y);
					switch (c)
					{
						// TODO: Check this logic
						case '\\':
							if (x == left_wall || y == top_wall)
								return{ x, y, Heading::SouthEast };
							else
								return{ x, y, Heading::NorthWest };
							break;
						case '/':
							if (x == left_wall || y == top_wall)
								return{ x, y, Heading::SouthWest };
							else
								return{ x, y, Heading::NorthEast };
							break;
						default:
							break;
					}
				}
			}
		}

		return{};
	}

private:
	Room &room;
	size_t num_completed = 0;
	std::vector<RayOfLight> rays;
};

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		std::string filename(argv[1]);
		std::ifstream fin(filename.c_str());

		if (fin.is_open())
		{
			while (fin.good())
			{
				Room room;
				fin >> room;

				Simulation sim(room);
				sim.run();

				std::cout << room;
			}
		}
	}

	return 0;
}

