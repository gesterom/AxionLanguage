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

bool validPreambleChar(uint8_t ch) {
	return (
		ch == '.' or
		(ch >= '0' and ch <= '9') or
		(ch >= 'a' and ch <= 'z') or
		(ch >= 'A' and ch <= 'Z')
		);
}

bool validPreambleChar(std::optional<uint8_t> ch) {
	if (ch.has_value())
		return validPreambleChar(ch.value());
	return false;
}

bool validPreambleName(const std::string& str) {
	bool res = true;
	for (const auto& i : str) {
		res = validPreambleChar(i);
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

bool isSpace(std::optional<uint8_t> ch) {
	if(ch.has_value()) return isSpace(ch.value());
	return false;
}

bool isDigit(uint8_t ch)
{
	return (ch>='0' and ch<='9');
}

bool isHexDigit(uint8_t ch) {
	return isDigit(ch) or (ch>='a' and ch<='f') or (ch>='A' and ch <='F');
}

bool isHexDigit(std::optional<uint8_t> ch) {
	if (ch.has_value()) return isHexDigit(ch.value());
	return false;
}

bool isDigit(std::optional<uint8_t> ch)
{
	if (ch.has_value()) return isDigit(ch.value());
	return false;
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

bool isCharIdentifier(std::optional<uint8_t> ch) {
	if(ch.has_value())
		return isCharIdentifier(ch.value());
	return false;
}

std::string to_lowercase(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return str;
}