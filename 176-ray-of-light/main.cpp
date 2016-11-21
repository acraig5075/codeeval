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
constexpr size_t max_light_distribution = 20;

constexpr size_t top_wall = 0;
constexpr size_t left_wall = 0;
constexpr size_t right_wall = room_width - 1;
constexpr size_t bottom_wall = room_height - 1;

enum class Heading { NorthEast, NorthWest, SouthWest, SouthEast, None };
enum class CellType { EmptySpace, Column, Prism, Wall, WallCorner };

struct Photon;

using RayOfLight = std::vector<Photon>;


struct Room
{
	std::array<std::array<char, room_width>, room_height> cells;

	const char& at(size_t x, size_t y) const
	{
		assert(x < room_width && y < room_height);
		return cells.at(y).at(x);
	}

	char& at(size_t x, size_t y)
	{
		assert(x < room_width && y < room_height);
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
{
	Photon() = default;

	Photon(size_t x, size_t y, Heading heading)
		: x(x), y(y), heading(heading)
	{}

	size_t x = room_width;
	size_t y = room_height;
	Heading heading = Heading::None;
};

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

			plot_rays();
		}
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
					case '\\':
						if (x == left_wall || y == top_wall)
							return{ x, y, Heading::SouthEast };
						else
							return{ x, y, Heading::NorthWest };
						break;
					case '/':
						if (x == right_wall || y == top_wall)
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

	void track(RayOfLight &ray)
	{
		while (ray.size() < max_light_distribution)
		{
			Photon next = next_photon(ray.back());
			CellType type = get_cell_type(next.x, next.y);

			switch (type)
			{
			case CellType::EmptySpace:
				ray.push_back(next);
				break;
			case CellType::WallCorner:
			case CellType::Column:
				num_completed++;
				return; // ray finished
			case CellType::Wall:
				next = reflection(next);
				ray.push_back(next);
				break;
			case CellType::Prism:
			{
				std::array<Photon, 3> split = split_photon(next);
				RayOfLight right(ray);
				RayOfLight left(ray);
				
				ray.push_back(split[0]);
				left.push_back(split[1]);
				right.push_back(split[2]);

				rays.push_back(left);
				rays.push_back(right);
				return; // push_back() on the vector-of-vectors has invalidated the reference.
			}
			default:
				assert(false);
				break;
			}
		}

		if (ray.size() == max_light_distribution)
		{
			num_completed++;
			return; // ray finished
		}
	}

	Photon next_photon(const Photon &p)
	{
		Photon r;
		switch (p.heading)
		{
		case Heading::NorthEast:
			assert(p.y > 0);
			r.x = p.x + 1;
			r.y = p.y - 1;
			break;
		case Heading::NorthWest:
			assert(p.x > 0);
			assert(p.y > 0);
			r.x = p.x - 1;
			r.y = p.y - 1;
			break;
		case Heading::SouthWest:
			assert(p.x > 0);
			r.x = p.x - 1;
			r.y = p.y + 1;
			break;
		case Heading::SouthEast:
			r.x = p.x + 1;
			r.y = p.y + 1;
			break;
		default:
			break;
		}

		assert(r.x <= right_wall);
		assert(r.y <= bottom_wall);
		r.heading = p.heading;
		return r;
	}

	CellType get_cell_type(size_t x, size_t y)
	{
		char c = room.at(x, y);

		switch (c)
		{
		case ' ':
			return CellType::EmptySpace;
		case 'o':
			return CellType::Column;
		case '*':
			return CellType::Prism;
		case '#':
			if ((x == left_wall && y == top_wall) ||
				(x == right_wall && y == top_wall) ||
				(x == left_wall && y == bottom_wall) ||
				(x == right_wall && y == bottom_wall))
				return CellType::WallCorner;
			else
				return CellType::Wall;
		default:
			assert(false);
			return CellType::WallCorner;
		}
	}

	Photon reflection(const Photon &p)
	{
		Photon r(p.x, p.y, Heading::None);

		if (p.x == left_wall)
		{
			r.x++;
			if (p.heading == Heading::NorthWest)
				r.heading = Heading::NorthEast;
			else if (p.heading == Heading::SouthWest)
				r.heading = Heading::SouthEast;
		}
		else if (p.x == right_wall)
		{
			r.x--;
			if (p.heading == Heading::NorthEast)
				r.heading = Heading::NorthWest;
			else if (p.heading == Heading::SouthEast)
				r.heading = Heading::SouthWest;
		}
		else if (p.y == top_wall)
		{
			r.y++;
			if (p.heading == Heading::NorthEast)
				r.heading = Heading::SouthEast;
			else if (p.heading == Heading::NorthWest)
				r.heading = Heading::SouthWest;
		}
		else if (p.y == bottom_wall)
		{
			r.y--;
			if (p.heading == Heading::SouthEast)
				r.heading = Heading::NorthEast;
			else if (p.heading == Heading::SouthWest)
				r.heading = Heading::NorthWest;
		}

		assert(r.heading != Heading::None);
		return r;
	}

	std::array<Photon, 3> split_photon(const Photon &p)
	{
		Photon ne{ p.x + 1, p.y - 1, Heading::NorthEast };
		Photon se{ p.x + 1, p.y + 1, Heading::SouthEast };
		Photon sw{ p.x - 1, p.y + 1, Heading::SouthWest };
		Photon nw{ p.x - 1, p.y - 1, Heading::NorthWest };

		switch (p.heading)
		{
		case Heading::NorthEast:
			return{ ne, nw, se };
		case Heading::NorthWest:
			return{ nw, sw, ne };
		case Heading::SouthWest:
			return{ sw, se, nw };
		case Heading::SouthEast:
			return{ se, ne, sw };
		default:
			assert(false);
			return{};
		}
	}

	void plot_rays()
	{
		for (const auto & ray : rays)
		{
			for (const auto & p : ray)
			{
				char &c = room.at(p.x, p.y);

				assert(c != '#' && c != 'o' && c != '*');

				if (p.heading == Heading::NorthEast || p.heading == Heading::SouthWest)
				{
					if (c == '\\')
						c = 'X';
					else
						c = '/';
				}
				else if (p.heading == Heading::NorthWest || p.heading == Heading::SouthEast)
				{
					if (c == '/')
						c = 'X';
					else
						c = '\\';
				}
			}
		}
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

