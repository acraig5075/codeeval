#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <string>
#include <cassert>
#include <cctype>

using std::array;
using std::vector;
using std::string;

static const int Rows = 10;
static const int Cols = 10;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Screen class does nothing more than own a 2D array of chars and can render it to an output stream

class Screen
{
public:
	array<array<char, Cols>, Rows> buffer;

	Screen();
	void render(std::ostream &out) const;
};

////////////////////////////////////////////////////////////////
// Recording class encapsulates a recording of editor keystrokes

class Recording
{
public:
	vector<string> records;

	Recording() = default;
	void fromTest();
	void fromFile(const string &filename);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Editor class references the screen character array and can modify it, e.g. by playing back a previous Recording

class Editor
{
	array<array<char, Cols>, Rows> &buffer;
	int cursorC = 0;
	int cursorR = 0;
	enum class EditMode { Insert, Overwrite };
	EditMode editMode = EditMode::Overwrite;

public:
	explicit Editor(Screen &screen)
		: buffer(screen.buffer)
	{}

	void playback(const Recording &recording);

private:
	void handleControlCode(char code);
	void handleNormalCode(char code);
	void moveTo(int row, int col);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The main function: Sets up a screen and an associated editor. A recording is then played-back, and the screen is rendered to the console

int main(int argc, char* argv[])
{
	Recording recording;
	if (argc > 1)
		recording.fromFile(argv[1]);
	else
		recording.fromTest();

	Screen screen;
	Editor editor(screen);
	editor.playback(recording);
	screen.render(std::cout);

	return 0;
}

////////////////////////
// Screen Implementation

Screen::Screen()
{
	for (auto &row : buffer)
		row.fill(' ');
}

void Screen::render(std::ostream &out) const
{
	for (const auto &row : buffer)
	{
		for (const auto &col : row)
			out << col;

		out << "\n";
	}
}

///////////////////////////
// Recording Implementation

void Recording::fromFile(const string &filename)
{
	records.clear();

	std::ifstream fin(filename.c_str());
	if (fin.is_open())
	{
		string line;
		while (fin.good())
		{
			std::getline(fin, line);
			records.push_back(line);
		}
		fin.close();
	}
}

void Recording::fromTest()
{
	// Uses raw string literals to obviate escaping the backslashes.
	records.clear();
	records.push_back(R"(^h^c)");
	records.push_back(R"(^04^^)");
	records.push_back(R"(^13/ \^d^b  /   \)");
	records.push_back(R"(^u^d^d^l^l^l^l^l^l^l^l^l)");
	records.push_back(R"(^r^r^l^l^d<CodeEval >^l^l^d/^b \)");
	records.push_back(R"(^d^r^r^66/^b  \)");
	records.push_back(R"(^b^d   \ /)");
	records.push_back(R"(^d^l^lv^d^b===========^i^94O123456)");
	records.push_back(R"(789^94A=======^u^u^u^u^u^u^l^l\^o^b^r/)");
}

////////////////////////
// Editor implementation

void Editor::playback(const Recording &recording)
{
	bool control = false; // currently reading a control code
	bool move = false; // currently reading a ^DD control code
	int row = 0; // the row digit of a ^DD control code
	int col = 0; // the column digit of a ^DD control code

	for (const auto &keystrokes : recording.records)
	{
		for (char c : keystrokes)
		{
			if (control)
			{
				control = false;
				if (std::isdigit(c))
				{
					move = true;
					row = c - '0';
				}
				else
				{
					handleControlCode(c);
				}
			}
			else
			{
				if (c == '^')
				{
					control = true;
				}
				else if (move)
				{
					move = false;
					assert(std::isdigit(c)); // we expect a second digit after reading ^D
					if (std::isdigit(c))
					{
						col = c - '0';
						moveTo(row, col);
					}
					else // shouldn't happen
					{
						handleNormalCode(c);
					}
				}
				else
				{
					handleNormalCode(c);
				}
			}
		}
	}
}

void Editor::handleControlCode(char code)
{
	switch (code)
	{ 
	case 'c': // clear the entire screen; the cursor row and column do not change
		for (auto &row : buffer)
			row.fill(' ');
		break;
	case 'h': // move the cursor to row 0, column 0; the image on the screen is not changed
		cursorC = 0;
		cursorR = 0;
		break;
	case 'b': // move the cursor to the beginning of the current line; the cursor row does not change
		cursorC = 0;
		break;
	case 'd': // move the cursor down one row if possible; the cursor column does not change
		cursorR = std::min(cursorR + 1, Rows - 1);
		break;
	case 'u': // move the cursor up one row, if possible; the cursor column does not change
		cursorR = std::max(cursorR - 1, 0);
		break;
	case 'l': // move the cursor left one column, if possible; the cursor row does not change
		cursorC = std::max(cursorC - 1, 0);
		break;
	case 'r': // move the cursor right one column, if possible; the cursor row does not change
		cursorC = std::min(cursorC + 1, Cols - 1);
		break;
	case 'e': // erase characters to the right of, and including, the cursor column on the cursor's row; the cursor row and column do not change 
	{
		auto &arow = buffer.at(static_cast<size_t>(cursorR));
		auto i = arow.begin() + cursorC;
		std::fill(i, arow.end(), ' ');
		break;
	}
	case 'i': // enter insert mode
		editMode = EditMode::Insert;
		break;
	case 'o': // enter overwrite mode
		editMode = EditMode::Overwrite;
		break;
	case '^': // write a circumflex(^) at the current cursor location, exactly as if it was not a special character;
		handleNormalCode('^');
		break;
	}
}

void Editor::handleNormalCode(char code)
{
	switch (editMode)
	{
	case EditMode::Insert:
	{
		// model Insert behaviour by temporarily using a vector before copying back to the array
		auto &arow = buffer.at(static_cast<size_t>(cursorR));
		vector<char> vrow(arow.begin(), arow.end());
		auto i = vrow.begin() + cursorC;
		vrow.insert(i, code);
		std::copy_n(vrow.begin(), arow.size(), arow.begin());
		break;
	}
	case EditMode::Overwrite:
		auto &arow = buffer.at(static_cast<size_t>(cursorR));
		arow.at(cursorC) = code;
		break;
	}

	cursorC = std::min(cursorC + 1, Cols - 1);
}

void Editor::moveTo(int row, int col)
{
	cursorR = std::max(row, 0);
	cursorR = std::min(row, Rows);
	cursorC = std::max(col, 0);
	cursorC = std::min(col, Cols);
}
