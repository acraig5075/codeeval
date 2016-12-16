// 176-ray-of-light.cpp : Defines the entry point for the console application.
//

#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <cassert>
#include <vector>

// Define this for debug formatted output. Undefine for codeeval submission
#define delimited_output

// Define this to compare output with a second command-line filename. Undefine for codeeval submission
#define do_file_comparison

constexpr size_t room_width = 10;
constexpr size_t room_height = 10;
constexpr size_t max_light_distribution = 20;

constexpr size_t top_wall = 0;
constexpr size_t left_wall = 0;
constexpr size_t right_wall = room_width - 1;
constexpr size_t bottom_wall = room_height - 1;

enum class Heading { NorthEast, NorthWest, SouthWest, SouthEast, None };
enum class CellType { EmptySpace, Column, Prism, SpentPrism, Wall, WallCorner, WallOpening };

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

	void reinstate_prisms()
	{
		for (int x = 0; x < room_width; ++x)
		{
			for (int y = 0; y < room_height; ++y)
			{
				char &c = at(x, y);
				if (c == 's')
					c = '*';
			}
		}
	}
};

std::istream &operator >>(std::istream &in, Room &room)
{
	std::string line;
	std::getline(in, line);

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
	else
	{
		in.setstate(std::ios_base::failbit);
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

bool operator== (const Photon&lhs, const Photon &rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.heading == rhs.heading;
}

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
			RayOfLight ray;
			ray.push_back(start);
			rays.push_back(ray);

			while (num_completed < rays.size())
			{
				track(rays.at(num_completed));
			}

			room.reinstate_prisms();

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
		Photon next(ray.back());
		ray.pop_back();

		while (ray.size() < max_light_distribution)
		{
			if (process_photon(ray, next))
				break;

			next = next_photon(next);

		}

		if (ray.size() == max_light_distribution)
		{
			num_completed++;
			return; // ray finished
		}
	}

	bool process_photon(RayOfLight &ray, const Photon &p)
	{
		CellType type = get_cell_type(p);

		switch (type)
		{
		case CellType::WallOpening:
			if (!ray.empty())
			{
				ray.push_back(p);
				num_completed++;
				return true;
			}
			// drop through intentional
		case CellType::EmptySpace:
			if (std::find(begin(ray), end(ray), p) == end(ray))
			{
				ray.push_back(p);
				return false;
			}
			else
			{
				num_completed++;
				return true; // ray finished
			}
		case CellType::WallCorner:
		case CellType::Column:
		case CellType::SpentPrism:
			num_completed++;
			return true; // ray finished
		case CellType::Prism:
		{
			std::array<Photon, 3> split = split_photon(p);

			RayOfLight left(ray);
			RayOfLight right(ray);

			ray.push_back(split[0]);
			left.push_back(split[1]);
			right.push_back(split[2]);

			rays.push_back(left);
			rays.push_back(right);

			room.at(p.x, p.y) = 's'; // prism can become a spent prism now.

			return true;
		}
		case CellType::Wall:
		default:
			assert(false);
			return false;
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

		if (get_cell_type(r) == CellType::Wall)
			r = reflection(r);

		return r;
	}

	CellType get_cell_type(const Photon &p)
	{
		size_t x = p.x;
		size_t y = p.y;
		char c = room.at(x, y);

		switch (c)
		{
		case ' ':
			return CellType::EmptySpace;
		case 'o':
			return CellType::Column;
		case '*':
			return CellType::Prism;
		case 's':
			return CellType::SpentPrism;
		case '#':
			if ((x == left_wall && y == top_wall) ||
				(x == right_wall && y == top_wall) ||
				(x == left_wall && y == bottom_wall) ||
				(x == right_wall && y == bottom_wall))
				return CellType::WallCorner;
			else
				return CellType::Wall;
		case '/':
		case '\\':
			if (p.y == top_wall ||
				p.x == left_wall ||
				p.x == right_wall ||
				p.y == bottom_wall)
				return CellType::WallOpening;
			else
				return CellType::EmptySpace;
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

				assert(c == ' ' || c == '/' || c == '\\' || c == 'X');

				if (c == 'X')
				{
					continue;
				}
				else if (p.heading == Heading::NorthEast || p.heading == Heading::SouthWest)
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

#if defined do_file_comparison
		std::ifstream fin2;
		bool compare = false;
		int test_no = 0;
		if (argc > 2)
		{
			std::string filename2(argv[2]);
			fin2.open(filename2.c_str());
			compare = fin2.is_open();
		}
#endif

		if (fin.is_open())
		{
			while (fin.good())
			{
				Room room;
				fin >> room;

				if (fin)
				{
					Simulation sim(room);
					sim.run();

					std::cout << room;

#if defined do_file_comparison
					if (compare && fin2.good())
					{
						Room room2;
						if (fin2 >> room2)
						{
							std::stringstream ss1, ss2;
							ss1 << room;
							ss2 << room2;
							std::string s1 = ss1.str();
							std::string s2 = ss2.str();
							if (s1 != s2)
							{
								std::cerr << "Mismatch on test #"
									<< test_no + 1 << "\n"
									<< "Actual:\n" << s1 << "\n"
									<< "Expected:\n" << s2 << "\n";
							}
						}
						test_no++;
					}
#endif
				}
			}
		}
	}

	return 0;
}

