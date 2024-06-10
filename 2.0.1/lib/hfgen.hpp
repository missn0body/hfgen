#ifndef hfgen__hfgen__hpp
#define hfgen__hfgen__hpp

#ifndef __cplusplus
#error This header file is only to be used with C++ code
#endif /* __cplusplus */

/*
	Main functions needed for creation of header files
	Made by anson in 2024, see LICENSE for related details
*/

#include <algorithm>
#include <string>
#include <type_traits>

using std::literals::string_literals::operator""s;

// Append a bunch of strings into one using the comma and return it
// I really didn't want to do a bunch of '+=' or '.append()' statements
// so I really like this
template<typename... T>
std::string AsString(T&&... t)
{
	std::string ret;
	static_assert((std::is_convertible_v<T&&, std::string> && ...));
	return ((ret += std::forward<T>(t)), ...);
}

// Repeat a string N amount of times
// Again, saves me from having to both write and read repeated code
std::string Mult(const std::string &what, std::size_t count)
{
	std::string ret;
	while(count--) ret += what;
	return ret;
}

inline std::string ToLower(std::string &&what)
{
	std::transform(what.begin(), what.end(), what.begin(), [](unsigned char c){ return std::tolower(c); });
	return what;
}

inline std::string MakePreProc(const std::string &what, const std::string &arg = ""s) { return AsString("#", what, " ", arg); }
inline std::string MakeComment(const std::string &what, bool block = false)
{
	if	(block == false) return AsString("/* ", what, " */");
	else if (block == true)  return AsString("/*\n\t", what, "\n*/"); // Newline after 'what', watch out
	return {};
}

inline std::string MakeGuard(const std::string &p, const std::string &f, const std::string &e)
{
	return AsString(p, "__", f, "__", e);
}

// This one-liner is a bit more cramped but most of the above functions go
// making this one-liner possible, maybe I can find a better design for it
inline std::string MakeIfBlock(const std::string &guard, const std::string &mes = ""s, std::size_t spaces = 1, bool notdefined = false)
{
	return AsString((notdefined ? "#ifndef " : "#ifdef "), guard, "\n"s, mes, Mult("\n"s, spaces), "#endif ", MakeComment(guard), "\n"s);
}

#endif /* hfgen__hfgen__hpp */
