#include "CodeLocation.h"

#include "StringUtility.h"

bool CodeLocation::CodeLocationPoint::operator==(const CodeLocationPoint& other) const {
	return
		this->position == other.position and
		this->position_utf8 == other.position_utf8 and
		this->line == other.line and
		this->byte_number == other.byte_number and
		this->character_number == other.character_number;
}

bool CodeLocation::comment() const noexcept { return commentLevel > 0 or lineComment; }

CodeLocation::CodeLocation(std::string filename) {
		this->file_name = filename;
		this->start_pos.position = 1;
		this->start_pos.position_utf8 = 1;
		this->start_pos.line = 1;
		this->start_pos.byte_number = 0;
		this->start_pos.character_number = 0;
		this->end_pos.position = 1;
		this->end_pos.position_utf8 = 1;
		this->end_pos.line = 1;
		this->end_pos.byte_number = 0;
		this->end_pos.character_number = 0;
		this->last_byte = '\0';
		this->in_quote = false;
		this->commentLevel = 0;
		this->lineComment = false;
		this->file = std::make_shared<VirtualFile>(filename);
	}
std::string CodeLocation::getFilename() const noexcept { return this->file_name; }
CodeLocation::CodeLocation(const CodeLocation& other) noexcept {
		this->file_name = other.file_name;
		this->start_pos = other.start_pos;
		this->end_pos = other.end_pos;
		this->last_byte = other.last_byte;
		this->value = other.value;
		this->in_quote = other.in_quote;
		this->commentLevel = other.commentLevel;
		this->lineComment = other.lineComment;
		this->file = other.file;
}

CodeLocation CodeLocation::operator=(const CodeLocation& other) noexcept {
		this->file_name = other.file_name;
		this->start_pos = other.start_pos;
		this->end_pos = other.end_pos;
		this->last_byte = other.last_byte;
		this->value = other.value;
		this->in_quote = other.in_quote;
		this->commentLevel = other.commentLevel;
		this->lineComment = other.lineComment;
		this->file = other.file;
		return *this;
}

CodeLocation::CodeLocation(CodeLocation&& other) noexcept {
	this->file_name = other.file_name;
	this->start_pos = other.start_pos;
	this->end_pos = other.end_pos;
	this->last_byte = other.last_byte;
	this->value = other.value;
	this->in_quote = other.in_quote;
	this->commentLevel = other.commentLevel;
	this->lineComment = other.lineComment;
	this->file = other.file;
}

CodeLocation CodeLocation::operator=(CodeLocation&& other) noexcept {
	this->file_name = other.file_name;
	this->start_pos = other.start_pos;
	this->end_pos = other.end_pos;
	this->last_byte = other.last_byte;
	this->value = other.value;
	this->in_quote = other.in_quote;
	this->commentLevel = other.commentLevel;
	this->lineComment = other.lineComment;
	this->file = other.file;
	return *this;
}

CodeLocation CodeLocation::operator+=(char ch) noexcept {
	this->end_pos.byte_number++;
	if (last_byte == '\r' and ch == '\n') { // windows newline
		if (isSpace(ch) and trim_copy(this->value) == "") {
			this->start_pos.line++;
			this->start_pos.position = 1;
			this->start_pos.position_utf8 = 1;
			this->start_pos.byte_number++;
		}
		this->end_pos.line++;
		this->end_pos.position = 1;
		this->end_pos.position_utf8 = 1;
		this->lineComment = false;
	}
	else if (ch == '\n') {
		if (isSpace(ch) and trim_copy(this->value) == "") {
			this->start_pos.line++;
			this->start_pos.position = 1;
			this->start_pos.position_utf8 = 1;
			this->start_pos.byte_number++;
		}
		this->end_pos.line++;
		this->end_pos.position = 1;
		this->end_pos.position_utf8 = 1;
		this->lineComment = false;
	}
	else {
		if (isSpace(ch) and trim_copy(this->value) == "") {
			this->start_pos.position++;
			this->start_pos.character_number++;
			this->start_pos.position_utf8++;
			this->start_pos.byte_number++;
		}
		this->end_pos.position++;
		if ((ch & 0xC0) != 0x80) // check if byte is not continuation of utf8 character
		{
			this->end_pos.character_number++;
			this->end_pos.position_utf8++;
		}
	}
	last_byte = ch;
	//if(this->in_quote or not isSpace(ch) or (this->comment() and ch != '\n'))
	value += ch;
	return *this;
}

CodeLocation CodeLocation::moveStartToEnd() const noexcept {
	CodeLocation res = *this;
	res.start_pos = res.end_pos;
	res.value = "";
	return res;
}

CodeLocation CodeLocation::move(uint64_t n) const noexcept {
	CodeLocation res = *this;
	// Increment the byte number for start_pos
	char ch = '\0';
	for (int i = 0; i < n and res.start_pos != res.end_pos; i++) {
		char last_byte = ch;
		ch = res.value[i];
		if (last_byte == '\r' && ch == '\n') { // Handle Windows newline
			res.start_pos.line++;
			res.start_pos.position = 1;
			res.start_pos.position_utf8 = 1;
			res.lineComment = false;
		}
		else if (ch == '\n') { // Handle Unix newline
			res.start_pos.line++;
			res.start_pos.position = 1;
			res.start_pos.position_utf8 = 1;
			res.lineComment = false;
		}
		else {
			res.start_pos.position++;
			if ((ch & 0xC0) != 0x80) // Check if byte is not continuation of UTF-8 character
			{
				res.start_pos.character_number++;
				res.start_pos.position_utf8++;
			}
		}
	}
	res.value = res.value.substr(n);
	return res;
}

CodeLocation CodeLocation::substr(uint64_t n) const noexcept {
	CodeLocation res = *this;
	res.end_pos = res.start_pos;
	res.value = "";
	for (int i = 0; i < n and res.end_pos != this->end_pos; i++) {
		res += this->value[i];
	}
	return res;
}

bool CodeLocation::isDouble() const noexcept {
	bool dot = false;
	for (const auto& i : this->value) {
		if (i == '.' and not dot) dot = true;
		else if (i == '.' and dot) return false;
		else if (not std::isdigit(i) and not isCharIdentifier(i)) return false;
	}
	return dot;
}

size_t CodeLocation::size() const noexcept { return end_pos.byte_number - start_pos.byte_number; }

std::string CodeLocation::val() const noexcept { return file->get(start_pos.byte_number,end_pos.byte_number); }

std::string CodeLocation::start() const noexcept { return std::to_string(start_pos.line) + ":" + std::to_string(start_pos.position_utf8); }

std::string CodeLocation::end() const noexcept { return std::to_string(end_pos.line) + ":" + std::to_string(end_pos.position_utf8); }

bool CodeLocation::operator<(const CodeLocation& other)const noexcept {
	return this->value < other.value;
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
	return this->file->get(end_pos.byte_number);
}

std::string CodeLocation::peek(int64_t n)
{
	return this->file->get(this->end_pos.byte_number,this->end_pos.byte_number+n);
}

bool CodeLocation::is_good() const noexcept
{
	return this->file->size() > this->end_pos.byte_number;
}

std::ostream& operator<<(std::ostream& out, const CodeLocation& loc) {
	out << loc.val();
	return out;
}