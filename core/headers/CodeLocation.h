#pragma once

#include <string>
#include <ostream>
#include <fstream>

#include "VirtualFile.h"

class CodeLocation {
	std::string file_name;
	using CodeLocationPoint = uint64_t;
	CodeLocationPoint start_pos;// including
	CodeLocationPoint end_pos;//excluding

	uint8_t last_byte; // used mainly to check for \r\n on windows;
	//std::string value; // string representation
	std::shared_ptr<VirtualFile> file;
	uint64_t limit_start = 0;
	uint64_t limit_end = 0;

public:
	explicit CodeLocation(std::string filename) ;
	CodeLocation asLimiter() const noexcept ;
	std::string getFilename() const noexcept ;
	CodeLocation(const CodeLocation& other) noexcept ;
	CodeLocation operator=(const CodeLocation& other) noexcept ;
	CodeLocation(CodeLocation&& other) noexcept ;
	CodeLocation operator=(CodeLocation&& other) noexcept ;
	CodeLocation operator+=(char ch) noexcept ;
	CodeLocation moveStartToEnd() const noexcept ;
	CodeLocation move(uint64_t n) const noexcept ;
	CodeLocation substr(uint64_t n) const noexcept ;
	size_t size() const noexcept ;
	std::string val() const noexcept ;
	std::string start() const noexcept ;
	std::string end() const noexcept ;
	bool operator<(const CodeLocation& other) const noexcept ;
	uint8_t get();
	std::string get(uint64_t);
	uint8_t peek();
	std::string peek(uint64_t);
	uint8_t look(int64_t);
	bool is_good() const noexcept;
};

std::ostream& operator<<(std::ostream& out, const CodeLocation& loc);