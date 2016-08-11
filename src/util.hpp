#pragma once

#include <string>

#define TOSTRING2(x) #x
#define TOSTRING(x) TOSTRING2(x)

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(*a))

#ifdef _WIN32
#	define DIR_DELIM "\\"
#	define fileno(x) _fileno(x)
inline bool isatty(int fd)
{
	DWORD mode;
	return GetConsoleMode((HANDLE)_get_osfhandle(fd), &mode);
}
#else
#	define DIR_DELIM "/"
#endif


inline std::string rtrim(const std::string & str)
{
	size_t back = str.size();
	while (std::isspace(str[back])) --back;
	return str.substr(0, back);
}


inline std::string trim(const std::string & str)
{
	size_t front = 0;
	while (std::isspace(str[front])) ++front;

	size_t back = str.size();
	while (back > front && std::isspace(str[back])) --back;

	return str.substr(front, back - front);
}

class Tokenizer {
	std::string str;
	size_t pos;
public:
	Tokenizer(const std::string &s) : str(s), pos(0) {}
	bool next(std::string *s, char sep)
	{
		if (pos >= str.size())
			return false;

		size_t n;
		if ((n = str.find(sep, pos)) == std::string::npos) {
			n = str.size();
		}
		*s = str.substr(pos, n - pos);
		pos = n + 1;
		return true;
	}
};