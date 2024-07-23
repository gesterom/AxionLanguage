#include "CodeLocation.h"

#include "StringUtility.h"

//bool CodeLocation::comment() const noexcept { return commentLevel > 0 or lineComment; }

CodeLocation::CodeLocation(std::string filename) {
		this->file_name = filename;
		//this->start_pos.position = 1;
		//this->start_pos.position_utf8 = 1;
		//this->start_pos.line = 1;
		this->start_pos = 0;
		//this->start_pos.character_number = 0;
		//this->end_pos.position = 1;
		//this->end_pos.position_utf8 = 1;
		//this->end_pos.line = 1;
		this->end_pos = 0;
		//this->end_pos.character_number = 0;
		this->last_byte = '\0';
		this->file = std::make_shared<VirtualFile>(filename);
	}
std::string CodeLocation::getFilename() const noexcept { return this->file_name; }
CodeLocation::CodeLocation(const CodeLocation& other) noexcept {
		this->file_name = other.file_name;
		this->start_pos = other.start_pos;
		this->end_pos = other.end_pos;
		this->last_byte = other.last_byte;
		//this->value = other.value;
		this->file = other.file;
}

CodeLocation CodeLocation::operator=(const CodeLocation& other) noexcept {
		this->file_name = other.file_name;
		this->start_pos = other.start_pos;
		this->end_pos = other.end_pos;
		this->last_byte = other.last_byte;
		//this->value = other.value;
		this->file = other.file;
		return *this;
}

CodeLocation::CodeLocation(CodeLocation&& other) noexcept {
	this->file_name = other.file_name;
	this->start_pos = other.start_pos;
	this->end_pos = other.end_pos;
	this->last_byte = other.last_byte;
	//this->value = other.value;
	this->file = other.file;
}

CodeLocation CodeLocation::operator=(CodeLocation&& other) noexcept {
	this->file_name = other.file_name;
	this->start_pos = other.start_pos;
	this->end_pos = other.end_pos;
	this->last_byte = other.last_byte;
	//this->value = other.value;
	this->file = other.file;
	return *this;
}

CodeLocation CodeLocation::operator+=(char ch) noexcept {
	if (isSpace(ch) and start_pos == end_pos) {
		this->start_pos++;
	}
	this->end_pos++;
	return *this;
}

CodeLocation CodeLocation::moveStartToEnd() const noexcept {
	CodeLocation res = *this;
	res.start_pos = res.end_pos;
	//res.value = "";
	return res;
}

CodeLocation CodeLocation::move(uint64_t n) const noexcept {
	CodeLocation res = *this;
	res.start_pos = std::min(start_pos+n,end_pos);
	return res;
}

CodeLocation CodeLocation::substr(uint64_t n) const noexcept {
	CodeLocation res = *this;
	res.end_pos = res.start_pos+n;
	return res;
}

bool CodeLocation::isDouble() const noexcept {
	bool dot = false;
	for (const auto& i : this->val()) {
		if (i == '.' and not dot) dot = true;
		else if (i == '.' and dot) return false;
		else if (not std::isdigit(i) and not isCharIdentifier(i)) return false;
	}
	return dot;
}

size_t CodeLocation::size() const noexcept { return end_pos - start_pos; }

std::string CodeLocation::val() const noexcept { return file->get(start_pos,end_pos); }

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
	return std::to_string(line) + ":" + std::to_string(position_utf8);
}

std::string CodeLocation::end() const noexcept { 
	// Increment the byte number for start_pos
	uint64_t line=1;
	uint64_t position = 1;
	uint64_t position_utf8 = 1;
	char ch = '\0';
	for (int i = 0; i < this->end_pos ; i++) {
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
	return this->val() < other.val();
}

uint8_t CodeLocation::get()
{
	auto res = this->peek();
	this->operator+=(res);
	return res;
}

std::string CodeLocation::get(int64_t n)
{
	std::string res;
	res.reserve(n);
	for (int i = 0; i < n; i++) {
		res.push_back(this->get());
	}
	return res;
}

uint8_t CodeLocation::peek()
{
	return this->file->get(end_pos);
}

std::string CodeLocation::peek(int64_t n)
{
	return this->file->get(this->end_pos,this->end_pos+n);
}

uint8_t CodeLocation::look(int64_t n)
{
	return this->file->get(end_pos+n);
}

bool CodeLocation::is_good() const noexcept
{
	return this->file->size() > this->end_pos;
}

std::ostream& operator<<(std::ostream& out, const CodeLocation& loc) {
	out << loc.val();
	return out;
}