#include "CodeLocation.h"


#include <algorithm>

#include "StringUtility.h"
#include "TODO.h"

//bool CodeLocation::comment() const noexcept { return commentLevel > 0 or lineComment; }

CodeLocation CodeLocation::null() noexcept
{
	return CodeLocation{};
}

CodeLocation::CodeLocation() {
	this->file_name = "";
	this->start_pos = 0;
	this->end_pos = 0;
	this->last_byte = '\0';
	this->file = nullptr;
	this->limit_start = 0;
	this->limit_end = 0;
#ifdef DEBUG
	this->visual = "";
#endif // DEBUG
}

CodeLocation::CodeLocation(std::string filename) {
	this->file_name = filename;
	this->start_pos = 0;
	this->end_pos = 0;
	this->last_byte = '\0';
	this->file = std::make_shared<VirtualFile>(filename);
	this->limit_start = 0;
	this->limit_end = file->size();
#ifdef DEBUG
	this->visual = this->to_string();
#endif // DEBUG
}
CodeLocation CodeLocation::asLimiter() const noexcept
{
	CodeLocation res = *this;
	res.limit_start = this->start_pos;
	res.limit_end = this->end_pos;
	res.end_pos = this->start_pos;
	res.start_pos = this->start_pos;
#ifdef DEBUG
	res.visual = res.to_string();
#endif // DEBUG
	return res;
}

std::string CodeLocation::getFilename() const noexcept { return this->file_name; }

CodeLocation::CodeLocation(const CodeLocation& other) noexcept {
	this->file_name = other.file_name;
	this->start_pos = other.start_pos;
	this->end_pos = other.end_pos;
	this->last_byte = other.last_byte;
	this->file = other.file;
	this->limit_start = other.limit_start;
	this->limit_end = other.limit_end;
#ifdef DEBUG
	this->visual = this->to_string();
#endif // DEBUG
}

CodeLocation CodeLocation::operator=(const CodeLocation& other) noexcept {
	this->file_name = other.file_name;
	this->start_pos = other.start_pos;
	this->end_pos = other.end_pos;
	this->last_byte = other.last_byte;
	this->file = other.file;
	this->limit_start = other.limit_start;
	this->limit_end = other.limit_end;
#ifdef DEBUG
	this->visual = this->to_string();
#endif // DEBUG
	return *this;
}

CodeLocation::CodeLocation(CodeLocation&& other) noexcept {
	this->file_name = other.file_name;
	this->start_pos = other.start_pos;
	this->end_pos = other.end_pos;
	this->last_byte = other.last_byte;
	this->file = other.file;
	this->limit_start = other.limit_start;
	this->limit_end = other.limit_end;
#ifdef DEBUG
	this->visual = this->to_string();
#endif // DEBUG
}

CodeLocation CodeLocation::operator=(CodeLocation&& other) noexcept {
	this->file_name = other.file_name;
	this->start_pos = other.start_pos;
	this->end_pos = other.end_pos;
	this->last_byte = other.last_byte;
	this->file = other.file;
	this->limit_start = other.limit_start;
	this->limit_end = other.limit_end;
#ifdef DEBUG
	this->visual = this->to_string();
#endif // DEBUG
	return *this;
}

CodeLocation CodeLocation::operator+=(char ch) noexcept {
	if (isSpace(ch) and start_pos == end_pos) {
		this->start_pos++;
	}
	this->end_pos++;
#ifdef DEBUG
	this->visual = this->to_string();
#endif // DEBUG

	return *this;
}

CodeLocation CodeLocation::consume(int64_t n) noexcept
{
	int i = 0;
	for (i = 0; isSpace(this->look(0)) and i < n; i++) {
		this->start_pos++;
		this->end_pos++;
	}
	if (i < n) {
		this->end_pos += (n - i);
	}
#ifdef DEBUG
	this->visual = this->to_string();
#endif // DEBUG
	return *this;
}

uint64_t CodeLocation::file_pointer_start() const
{
	return this->start_pos;
}

uint64_t CodeLocation::file_pointer_end() const
{
	return this->end_pos;
}

CodeLocation CodeLocation::moveStartToEnd() const noexcept {
	CodeLocation res = *this;
	res.start_pos = res.end_pos;
	//res.value = "";
#ifdef DEBUG
	res.visual = res.to_string();
#endif // DEBUG
	return res;
}

CodeLocation CodeLocation::move(uint64_t n) const noexcept {
	CodeLocation res = *this;
	res.start_pos = std::min(start_pos + n, end_pos);
#ifdef DEBUG
	this->visual = this->to_string();
#endif // DEBUG
	return res;
}

CodeLocation CodeLocation::substr(uint64_t n) const noexcept {
	CodeLocation res = *this;
	res.end_pos = res.start_pos + n;
#ifdef DEBUG
	res.visual = res.to_string();
#endif // DEBUG
	return res;
}

size_t CodeLocation::size() const noexcept { return end_pos - start_pos; }

std::string CodeLocation::to_string() const noexcept { return file->get(start_pos, end_pos); }

bool CodeLocation::empty() const noexcept {
	return this->start_pos == this->end_pos;
}

std::string CodeLocation::start() const noexcept { 	// Increment the byte number for start_pos
	uint64_t line = 1;
	uint64_t position = 1;
	uint64_t position_utf8 = 1;
	char ch = '\0';
	for (int i = 0; i < this->start_pos; i++) {
		char last_byte = ch;
		ch = this->file->get(i);
		if (last_byte == '\r' && ch == '\n') { // Handle Windows newline
			line++;
			position = 1;
			position_utf8 = 1;
		}
		else if (ch == '\n') { // Handle Unix newline
			line++;
			position = 1;
			position_utf8 = 1;
		}
		else {
			position++;
			if ((ch & 0xC0) != 0x80) // Check if byte is not continuation of UTF-8 character
			{
				position_utf8++;
			}
		}
	}
	return std::to_string(this->start_pos) + "-" + std::to_string(line) + ":" + std::to_string(position_utf8);
}

std::string CodeLocation::end() const noexcept {
	// Increment the byte number for start_pos
	uint64_t line = 1;
	uint64_t position = 1;
	uint64_t position_utf8 = 1;
	char ch = '\0';
	for (int i = 0; i < this->end_pos; i++) {
		char last_byte = ch;
		ch = this->file->get(i);
		if (last_byte == '\r' && ch == '\n') { // Handle Windows newline
			line++;
			position = 1;
			position_utf8 = 1;
		}
		else if (ch == '\n') { // Handle Unix newline
			line++;
			position = 1;
			position_utf8 = 1;
		}
		else {
			position++;
			if ((ch & 0xC0) != 0x80) // Check if byte is not continuation of UTF-8 character
			{
				position_utf8++;
			}
		}
	}
	return std::to_string(line) + ":" + std::to_string(position_utf8);
}

bool CodeLocation::operator<(const CodeLocation& other)const noexcept {
	return this->to_string() < other.to_string();
}

std::optional<uint8_t> CodeLocation::peek() const
{
	if (end_pos >= this->limit_end) return std::nullopt;
	if (end_pos < this->limit_start) return std::nullopt;
	return this->file->get(end_pos);
}

std::string CodeLocation::peek(uint64_t n) const
{
	ASSERT(this->end_pos + n > this->end_pos, "dont look back");
	return this->file->get(this->end_pos, std::min((uint64_t)(this->end_pos + n), this->limit_end));
}

std::optional<uint8_t> CodeLocation::look(int64_t n) const
{
	if (end_pos + n >= limit_end) return std::nullopt;
	if (end_pos + n < limit_start) return std::nullopt;
	return this->file->get(end_pos + n);
}

bool CodeLocation::is_good() const noexcept
{
	if (this->end_pos >= limit_end) return false;
	return this->file->size() > this->end_pos + 1;
}

std::ostream& operator<<(std::ostream& out, const CodeLocation& loc) {
	out << loc.to_string();
	return out;
}

bool operator==(const CodeLocation& loc, std::string str)
{
	return loc.to_string() == str;
}
