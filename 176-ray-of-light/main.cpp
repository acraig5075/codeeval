// 176-ray-of-light.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <array>
#include <cassert>

// Define this for debug formatted output. Undefine for codeeval submission
#define delimited_output

constexpr size_t room_width = 10;
constexpr size_t room_height = 10;

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

struct Photon
{};

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
				std::cout << room;
			}
		}
	}

	return 0;
}

