// Rabin-Karp implementation for massive strings. Complete overkill for this challenge. 
// Also handles multiple wildcards, not required. Again complete overkill. 

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <ctime> 
#include <cassert>


using std::string;
using std::vector;
using std::ifstream;
using std::cout;
using std::unordered_map;


static const int64_t prime = 179424691;


struct Data
{
	string pattern, text;
};


auto split(const string &value, char delimiter) -> vector<string>;
auto replace_asterisk(const string &original, const string &search, const string &replacement) -> string;
auto is_substring(const string &source, const string &query) -> bool;
auto mod(int64_t a, int64_t p) -> int64_t;
auto random(int64_t low, int64_t high) -> int64_t;
auto poly_hash(const string &S, size_t begin, size_t end, int64_t p, int64_t x) -> int64_t;
auto poly_hash(const string &S, int64_t p, int64_t x) -> int64_t;
auto are_equal(const string &pattern, const string &text, size_t off) -> bool;
auto precompute_hashes(const string &T, size_t L, int64_t p, int64_t x) -> unordered_map<size_t, int64_t>;
auto rabin_karp(const Data &input) -> vector<size_t>;

auto test_is_substring() -> void;
auto test_split() -> void;

int main(int argc, char *argv[])
{
	//test_split();
	//test_is_substring();

	if (argc > 1)
	{
		string filename(argv[1]);
		ifstream fin(filename.c_str());

		if (fin.is_open())
		{
			string line;
			while (fin.good())
			{
				getline(fin, line);

				if (!line.empty())
				{
					auto tokens = split(line, ',');
					cout << std::boolalpha << is_substring(tokens[0], tokens[1]) << "\n";
				}
			}
		}
	}

	return 0;
}


// string tokenise
auto split(const string &value, char delimiter) -> vector<string>
{
	vector<string> ret;

	size_t start = 0;
	while (true)
	{
		size_t pos = value.find(delimiter, start);
		if (pos == string::npos)
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

auto replace_asterisk(const string &original, const string &search, const string &replacement) -> string
{
	string text = original;
	size_t pos = 0;

	while (true)
	{
		pos = text.find(search, pos);
		if (pos == string::npos)
			break;

		text.replace(pos, search.length(), replacement);
		pos += replacement.length();
	}

	return text;
}

// test whether text2 is a substring of text1
auto is_substring(const string &source, const string &query) -> bool
{
	string text1 = replace_asterisk(source, "*", "!");
	string text2 = replace_asterisk(query, "\\*", "!");

	auto tokens = split(text2, '*');

	size_t start = 0;

	for (const auto &pattern : tokens)
	{
		if (pattern == "")
			continue;

		string text;
		text.assign(text1, start, string::npos);

		Data data;
		data.text = text;
		data.pattern = pattern;
		auto occurences = rabin_karp(data);

		if (occurences.empty())
			return false;

		start = start + occurences[0] + pattern.length();
	}

	return true;
}

int64_t mod(int64_t a, int64_t p)
{
	return ((a % p) + p) % p;
}

int64_t random(int64_t low, int64_t high)
{
	srand((unsigned)time(0));

	return low + (int64_t)(rand() * ((high - low) / RAND_MAX));
}

int64_t poly_hash(const string &S, size_t begin, size_t end, int64_t p, int64_t x)
{
	int64_t hash = 0;
	for (size_t i = end; i > begin; --i)
	{
		hash = mod(hash * x + S[i - 1], p);
	}
	return hash;
}

int64_t poly_hash(const string &S, int64_t p, int64_t x)
{
	return poly_hash(S, 0, S.length(), p, x);
}

bool are_equal(const string &pattern, const string &text, size_t off)
{
	const char *p = pattern.c_str();
	const char *t = text.c_str() + off;

	while (*p != '\0')
	{
		if (*p != *t)
			return false;
		++p;
		++t;
	}

	return true;
}

unordered_map<size_t, int64_t> precompute_hashes(const string &T, size_t L, int64_t p, int64_t x)
{
	size_t TL = T.length();

	unordered_map<size_t, int64_t> H;
	H.reserve(TL - L + 1);

	string S = T.substr(TL - L);
	H[TL - L] = poly_hash(S, p, x);

	int64_t y = 1;
	for (size_t i = 1; i <= L; ++i)
	{
		y = mod(y * x, p);
	}

	for (size_t j = TL - L; j > 0; --j)
	{
		size_t i = j - 1;
		H[i] = mod(x * H[i + 1] + T[i] - y * T[i + L], p);
	}

	return H;
}

vector<size_t> rabin_karp(const Data &input)
{
	vector<size_t> ans;

	const string &P = input.pattern;
	const string &T = input.text;
	const size_t L = P.length();
	const size_t TL = T.length();

	if (L <= TL)
	{
		int64_t p = prime;
		int64_t x = random(1, p - 1);

		int64_t pHash = poly_hash(P, p, x);

		unordered_map<size_t, int64_t> H = precompute_hashes(T, L, p, x);

		for (size_t i = 0; i <= TL - L; ++i)
		{
			int64_t tHash = H[i];

			if (pHash == tHash)
			{
				if (are_equal(P, T, i))
					ans.push_back(i);
			}
		}
	}

	return ans;
}

auto test_is_substring() -> void
{
	auto Test = [](bool expected, const string &a, const string &b)
	{
		assert(is_substring(a, b) == expected);
	};

	Test(true, "Hello", "ell");
	Test(true, "This is good", "is");
	Test(true, "CodeEval", "C*Eval");
	Test(false, "CodeEval", "C\\*Eval");
	Test(true, "Code*Eval", "Code\\*Eval");
	Test(false, "Old", "Young");
	Test(true, "the quick brown fox jumps over the lazy dog", "dog"); 
	Test(true, "the quick brown fox jumps over the lazy dog", "quick*fox");
	Test(false, "the quick brown fox jumps over the lazy dog", "quick\\*fox");
	Test(true, "the quick*fox jumps over the lazy dog", "quick\\*fox");
	Test(true, "123456789987654321", "7*9");
	Test(true, "123456789987654321", "1*1");
	Test(true, "123456789987654321", "1*");
	Test(true, "123456789987654321", "*1");
	Test(true, "anything whatsoever", "*");
	Test(false, "anything whatsoever", "\\*");
	Test(true, "*", "*");
	Test(true, "*", "\\*");
	Test(true, "abcdef*ghijkl", "f*g");
	Test(true, "abcdef*ghijkl", "f\\*g");
	Test(true, "abcdef*ghijkl", "d*i");
	Test(false, "the quick brown fox jumps over the lazy dog", "lazy*quick");
	Test(false, "the quiiiiick brown fox jumps over the laaaaazy dog", "quick*lazy");
	Test(false, "the quiiiiick brown fox jumps over the laaaaazy dog", "lazy*quick");
	Test(true, "the quickquickquick brown fox jumps over the lazylazylazy dog", "quick*lazy");
	Test(true, "quickquickquick brown fox jumps over the lazylazylazy", "quick*lazy");
	Test(true, "quickquickquicklazylazylazy", "quick*lazy");
	Test(false, "lazylazylazyquickquickquick", "quick*lazy");
	Test(false, "slowlazylazylazy", "quick*lazy");
	Test(false, "quickquickquick123456789", "quick*lazy");
	Test(true, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", "c*t");
	Test(true, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", "t*c");
	Test(true, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", "za");
	Test(true, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", "z*a");


	Test(false, "*So*eaE", "*JH*");
	Test(true, "* So*eaE", "**");
	Test(true, "blblah*", "blb*h*");
	Test(false, "blah*", "blb*h*");
	Test(false, "blah*", "blb*h");
	Test(true, "CodeEval", "C*Eval");
	Test(true, "CodeEval", "C*Ev*al");
	Test(false, "CodeE*val", "C*Ev*al");
	Test(false, "Old", "*Young*");
	Test(false, "Old", "*ung*");
	Test(true, "Old", "*l*");
	Test(true, "Old", "O*l*");
	Test(true, "Old", "*d");
	Test(true, "Old", "O*");
	Test(true, "Old", "O*d");

	Test(true, "rq6KRtZLQnYj29t8wrMs9kEAa", "Ms9kE");
	Test(false, "F3Lt8kAP48GV0KXg7w", "kL7APw8Vgt08KG3X");
	Test(true, "VO4nBSExzqUlGAU xoRpQ H VmEu", "xoRpQ H");
	Test(false, "iKdQ1CdNVT4", "4QT");
	Test(true, "rZqvA9MWQry0 7bEYn7fSiw DqWYW1Vge7", "W");
	Test(false, "YBsZaUeIwYdopfSr1e5FlTSJ", "sZaUeIwYdopfS\\*FlT");
	Test(true, "3K gATr4F0qqHkM8 nocDuMafyF8N4PU6YS", "gATr4F0qqHkM8 noc*fyF8N4PU");
	Test(true, "9 yhuBs", "9");
	Test(true, "2jyTx6rSt QQnq x  OochTAM", "jyT");
	Test(true, "zw1tK5djZhN7nmp54o77OTu", "K5djZhN*54");
	Test(false, "2kr08Rr tzRc7jLkkzz7 GWPxexHw2wiGi1 1 7", "Lkkzz7\\*xexHw2wiGi1");
	Test(true, "am jyCjcXch", "cX");
	Test(true, "bQ*F BvpPWdSRunNb QoSBNp", "Q\\*F BvpP");
	Test(true, "ecNTbxI7Z2DLGYJOZNkePl2Ue pPYP", "ZNk*l2Ue pPYP");
	Test(true, "hwI43PKf", "K");
	Test(true, "m41gaNa6BU1ziQlJJI93oyjJzSiMd", "m41gaNa6BU*Q");
	Test(true, "6x6 P1qjPhEbepB GMOArCZ", "pB GMO*Z");
	Test(false, "sI3awJJN2xpx33s4DkTXFtH6NX2nbD8f1e", "xJs842Ia1kXxpTDf3N2X3JFt");
	Test(false, "A0FQzyR6Ttso47w9f YsPP", "\\*");
	Test(true, "LqPbXiBy77st8j", "s*");
	Test(false, "mWqlPChz AWoiaa2FI32", "ah2C");
	Test(false, "GuOh", "\\*");
	Test(true, "1nGojur9D399 oLcM", "oLc");
	Test(true, "Hg JHzctQP09x PxCpgWlTR", "PxC");
	Test(false, "Wa UF7mMIyroNmMHXxSrIX3h1", "Nr Ixy73SaM");
	Test(true, "PQYARSqAynqv AZ 9Lh2 lOq v2kH4NwX", "*2kH4N");
	Test(false, "sohG ohTDDAXWj7wkytij m", "\\*i");
	Test(false, "zd hhRuf", "fzRh");
	Test(false, "Ktvr6pGYBbXMnTYAv1Iolf", "B1YfvYpobMrvnKlA6TtG");
	Test(false, "rXgXuv8FRfRxoCo7OoA kkitGB0OYIxtX", "XRftRxr0kBC");
	Test(true, "5MhTky8HS6XjpoIWtUkUI", "8HS");
	Test(true, "QigmDb5XuQv1SSu HX", "S");
	Test(false, "kX0blNRdIPe6", "b\\*dI");
	Test(false, "xpJuW 6yThHO7tRAX xR9UklJ34uDe", "xUxTJh3Hy 7J9uptX A4");
	Test(true, "g5IPRqUlQmkJ8 6B5qiZy8zaWFLqJwAYh", "QmkJ8 *qiZ");
	Test(false, "k1yK3deM8zwOV0xAcBr", "yMkOKB");
	Test(false, "aqn dKoyXlBfUD3zFSTDQozIn erWkSCGxC1oZ", "qSrByndzFSXoGao ZI");
	Test(true, "Zovk1cL8PNHsmFEOQb6", "vk1cL8*m");
	Test(true, "lWvuWANx20FuCvZdOLixXQ1nM4oRkp3h9tqI", "d*");
	Test(true, "p3tfkPhNh", "hN");

}

auto test_split() -> void
{
	auto Test = [](const string &text, const vector<string> &expected)
	{
		auto actual = split(text, '*');
		assert(actual == expected);
	};

	Test("C*Eval", { "C", "Eval" });
	Test("C*Ev*al", { "C", "Ev", "al" });
	Test("C*Eval*", { "C", "Eval", "" });
	Test("*C*Eval", { "", "C", "Eval" });
	Test("*C*Eval*", { "", "C", "Eval", "" });
	Test("C**Eval", { "C", "", "Eval" });
	Test("C***Eval", { "C", "", "", "Eval" });
}
