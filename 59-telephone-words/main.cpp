#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <cassert>

class Tree;
class Node;
using DataType = char;

//#define UNIT_TESTS

auto BreadthFirstInsertion(Tree &tree, const std::string &value) -> void;
auto DepthFirstPrint(const Tree &tree, std::ostream &os) -> void;
auto Split(const std::string &value, char delimiter) -> std::vector<std::string>;

static bool start = true;

const std::array<std::vector<char>, 10> telephone =
{
	{
		{ '0' },
		{ '1' },
		{ 'a', 'b', 'c' },
		{ 'd', 'e', 'f' },
		{ 'g', 'h', 'i' },
		{ 'j', 'k', 'l' },
		{ 'm', 'n', 'o' },
		{ 'p', 'q', 'r', 's' },
		{ 't', 'u', 'v' },
		{ 'w', 'x', 'y', 'z' }
	}
};

class Node
{
	Node *m_firstChild = nullptr;
	Node *m_nextSibling = nullptr;
	DataType m_data;

public:
	Node(const DataType &data = 0)
		: m_data(data)
	{}

	~Node()
	{
		if (m_nextSibling)
		{
			delete m_nextSibling;
		}
		if (m_firstChild)
		{
			delete m_firstChild;
		}
	}

	Node *AddChild(const DataType &data)
	{
		Node *node = new Node(data);

		if (m_firstChild)
		{
			Node *child = m_firstChild;
			while (child->m_nextSibling)
			{
				child = child->m_nextSibling;
			}
			child->m_nextSibling = node;
		}
		else
		{
			m_firstChild = node;
		}

		return node;
	}

	void Print(std::ostream &os, const std::string &prefix = "")
	{
		if (m_firstChild == nullptr)
		{
			if (start)
			{
				os << (prefix.c_str() + 1) << m_data;
				start = false;
			}
			else
			{
				os << prefix << m_data;
			}
		}

		Node *node = m_firstChild;
		while (node)
		{
			node->Print(os, prefix + m_data);
			node = node->m_nextSibling;
		}
	}

	void Assign(std::vector<std::string> &v, const std::string &prefix = "")
	{
		if (m_firstChild == nullptr)
			v.push_back(prefix + m_data);

		Node *node = m_firstChild;
		while (node)
		{
			node->Assign(v, prefix + m_data);
			node = node->m_nextSibling;
		}
	}

	Node *GetSibling()
	{
		return m_nextSibling;
	}
};

class Tree
{
	Node *m_root = nullptr;

public:
	Tree()
	{
		m_root = new Node(',');
		start = true;
	}

	~Tree()
	{
		delete m_root;
	}

	Node *GetRoot() const
	{
		return m_root;
	}
};

// the main function
int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		std::string filename(argv[1]);
		std::ifstream fin(filename.c_str());
		if (fin.is_open())
		{
			std::string line;
			while (fin.good())
			{
				std::getline(fin, line);
				if (line.empty())
					continue;

				Tree tree;
				BreadthFirstInsertion(tree, line);

				DepthFirstPrint(tree, std::cout);
			}
			fin.close();
		}
	}
	else
	{
		Tree tree;
		BreadthFirstInsertion(tree, "0000000"); //4155230

		DepthFirstPrint(tree, std::cout);
	}
	return 0;
}

void BreadthFirstInsertion(Tree &tree, const std::string &line)
{
	std::vector<Node *> level0;
	std::vector<Node *> level1;

	Node *root = tree.GetRoot();
	level0.push_back(root);

	for (char c : line)
	{
		int key = std::atoi(&c);

		const std::vector<char> &digits = telephone[key];

		for (Node *parent : level0)
		{
			for (auto i = std::begin(digits); i != std::end(digits); ++i)
			{
				Node *child = parent->AddChild(*i);

				level1.push_back(child);
			}
		}

		level0 = level1;
		level1.clear();
	}
}

void DepthFirstPrint(const Tree &tree, std::ostream &os)
{
#if defined UNIT_TESTS
	std::stringstream ss;
	tree.GetRoot()->Print(ss);

	const std::string str = ss.str();
	std::vector<std::string> words = Split(str, ',');

	assert(!str.empty());
	assert(str.at(0) != ',');
	assert(str.back() != ',');
	assert(std::all_of(std::begin(words), std::end(words), [](const std::string &s) {return s.length() == 7; }));
	assert(std::is_sorted(std::begin(words), std::end(words)));
#else
	tree.GetRoot()->Print(os);
	os << "\n";
#endif
}

std::vector<std::string> Split(const std::string &value, char delimiter)
{
	std::vector<std::string> ret;

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