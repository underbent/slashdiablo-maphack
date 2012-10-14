#pragma once
#include <string>
#include <vector>
#include <sstream>

void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);
wchar_t* AnsiToUnicode(const char* str);
char* UnicodeToAnsi(const wchar_t* str);
template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

template< class type> std::string to_string( const type & value)
{ std::stringstream ss; ss << value; return ss.str(); }

bool StringToBool(std::string str);

std::string Trim(std::string source);

struct KeyCode {
	std::string name;
	unsigned int value;
	std::string literalName;
};

KeyCode GetKeyCode(unsigned int nKey);
KeyCode GetKeyCode(const char* name);