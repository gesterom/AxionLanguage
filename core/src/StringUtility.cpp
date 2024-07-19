#include "StringUtility.h"

#include <algorithm> 
#include <cctype>
#include <locale>

void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

void trim(std::string& s) {
	rtrim(s);
	ltrim(s);
}

// trim from start (copying)
std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
std::string trim_copy(std::string s) {
	trim(s);
	return s;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}
inline bool validPreambuleChar(uint8_t ch) {
	return (
		ch == '.' or
		(ch >= '0' and ch <= '9') or
		(ch >= 'a' and ch <= 'z') or
		(ch >= 'A' and ch <= 'Z')
		);
}
bool validPreambuleName(const std::string& str) {
	bool res = true;
	for (const auto& i : str) {
		res = validPreambuleChar(i);
		if (res == false) return false;
	}
	return true;
}

bool isSpace(uint8_t ch) {
	return
		ch == 0x20/*space*/ or
		ch == 0x09/*TAB*/ or
		ch == 0x0A/*New Line*/ or
		ch == 0x0B/*vertical tab*/ or
		ch == 0x0D/*Carriage return*/ or
		ch == 0x1C/*File separator*/ or
		ch == 0x7F/*DEL*/
		;
}

bool isCharIdentifier(uint8_t ch) {
	return (
		(ch >= 'a' and ch <= 'z') // lowercase
		or
		(ch >= 'A' and ch <= 'Z') // upercase
		or
		(ch >= '0' and ch <= '9') // digits
		or
		(ch == '_') // underscore
		);
}