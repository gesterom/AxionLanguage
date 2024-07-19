#pragma once

#include <string>
#include <ostream>
#include <fstream>

#include "VirtualFile.h"

class CodeLocation {
	std::string file_name;
	struct CodeLocationPoint {
		uint64_t position; // counted from beginig of line and from 1 and in bytes
		uint64_t position_utf8; // counts utf-8 characters from begining of line starting on 1
		uint64_t line; // counted lines and started on 1
		uint64_t byte_number; // count bytes
		uint64_t character_number; // counted by utf-8 characters from begining of file;
		bool operator==(const CodeLocationPoint& other) const ;
	};
	CodeLocationPoint start_pos;// including
	CodeLocationPoint end_pos;//excluding

	uint8_t last_byte; // used mainly to check for \r\n on windows;
	std::string value; // string representation
	std::shared_ptr<VirtualFile> file;
public:
	bool in_quote;
	int64_t commentLevel;
	bool lineComment;
	bool comment() const noexcept ;
	explicit CodeLocation(std::string filename) ;
	std::string getFilename() const noexcept ;
	CodeLocation(const CodeLocation& other) noexcept ;
	CodeLocation operator=(const CodeLocation& other) noexcept ;
	CodeLocation(CodeLocation&& other) noexcept ;
	CodeLocation operator=(CodeLocation&& other) noexcept ;
	CodeLocation operator+=(char ch) noexcept ;
	CodeLocation moveStartToEnd() const noexcept ;
	CodeLocation move(uint64_t n) const noexcept ;
	CodeLocation substr(uint64_t n) const noexcept ;
	bool isDouble() const noexcept ;
	size_t size() const noexcept ;
	std::string val() const noexcept ;
	std::string start() const noexcept ;
	std::string end() const noexcept ;
	bool operator<(const CodeLocation& other) const noexcept ;
	uint8_t get();
	std::string get(int64_t);
	uint8_t peek();
	std::string peek(int64_t);
	bool is_good() const noexcept;
};

std::ostream& operator<<(std::ostream& out, const CodeLocation& loc);