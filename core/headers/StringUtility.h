#pragma once

#include <string>

//trim from start
void ltrim(std::string& s) ;

// trim from end (in place)
void rtrim(std::string& s) ;

// trim from both ends
void trim(std::string& s) ;

// trim from start (copying)
std::string ltrim_copy(std::string s) ;

// trim from end (copying)
std::string rtrim_copy(std::string s) ;

// trim from both ends (copying)
std::string trim_copy(std::string s) ;

// replace all acurence of from string using to string
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) ;

//check if string constains only [a-zA-Z0-9] and dot
bool validPreambuleName(const std::string& str) ;
bool validPreambuleChar(uint8_t);

// check if it is a ascii white space
bool isSpace(uint8_t ch) ;

//check ch is [a-zA-Z0-9_]
bool isCharIdentifier(uint8_t ch) ;