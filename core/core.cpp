// core.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <cassert>
#include <functional>
#include <type_traits>

#include "Enumerate.h"
#include "StringUtility.h"

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

class CodeLocation {
	std::string file_name;
	struct CodeLocationPoint {
		uint64_t position; // counted from beginig of line and from 1 and in bytes
		uint64_t position_utf8;
		uint64_t line; // counted lines and started on 1
		uint64_t byte_number; // count bytes
		uint64_t character_number; // counted by utf-8 characters from begining of file;
		bool operator==(const CodeLocationPoint& other) const {
			return
				this->position == other.position and
				this->position_utf8 == other.position_utf8 and
				this->line == other.line and
				this->byte_number == other.byte_number and
				this->character_number == other.character_number;
		}
	};
	CodeLocationPoint start_pos;// including
	CodeLocationPoint end_pos;//excluding

	uint8_t last_byte; // used mainly to check for \r\n on windows;
	std::string value;

public:
	bool in_quote;
	int64_t commentLevel;
	bool lineComment;
	bool comment() const noexcept { return commentLevel > 0 or lineComment; }
	explicit CodeLocation(std::string filename) {
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
	}
	std::string getFilename() const noexcept { return this->file_name; }
	CodeLocation(const CodeLocation& other) noexcept{
		this->file_name = other.file_name;
		this->start_pos = other.start_pos;
		this->end_pos = other.end_pos;
		this->last_byte = other.last_byte;
		this->value = other.value;
		this->in_quote = other.in_quote;
		this->commentLevel = other.commentLevel;
		this->lineComment = other.lineComment;
	}
	CodeLocation operator=(const CodeLocation& other) noexcept{
		this->file_name = other.file_name;
		this->start_pos = other.start_pos;
		this->end_pos = other.end_pos;
		this->last_byte = other.last_byte;
		this->value = other.value;
		this->in_quote = other.in_quote;
		this->commentLevel = other.commentLevel;
		this->lineComment = other.lineComment;
		return *this;
	}
	CodeLocation(CodeLocation&& other) noexcept {
		this->file_name = other.file_name;
		this->start_pos = other.start_pos;
		this->end_pos = other.end_pos;
		this->last_byte = other.last_byte;
		this->value = other.value;
		this->in_quote = other.in_quote;
		this->commentLevel = other.commentLevel;
		this->lineComment = other.lineComment;
	}
	CodeLocation operator=(CodeLocation&& other) noexcept {
		this->file_name = other.file_name;
		this->start_pos = other.start_pos;
		this->end_pos = other.end_pos;
		this->last_byte = other.last_byte;
		this->value = other.value;
		this->in_quote = other.in_quote;
		this->commentLevel = other.commentLevel;
		this->lineComment = other.lineComment;
		return *this;
	}
	CodeLocation operator+=(char ch) noexcept {
		this->end_pos.byte_number++;
		
		if (last_byte == '\r' and ch == '\n') { // windows newline
			if (std::isspace(ch) and trim_copy(this->value) == "") {
				this->start_pos.line++;
				this->start_pos.position = 0;
				this->start_pos.position_utf8 = 0;
			}
			this->end_pos.line++;
			this->end_pos.position = 0;
			this->end_pos.position_utf8 = 0;
			this->lineComment = false;
		}
		else if (ch == '\n') {
			if (std::isspace(ch) and trim_copy(this->value) == "") {
				this->start_pos.line++;
				this->start_pos.position = 0;
				this->start_pos.position_utf8 = 0;
			}
			this->end_pos.line++;
			this->end_pos.position = 0;
			this->end_pos.position_utf8 = 0;
			this->lineComment = false;
		}
		else {
			if (std::isspace(ch) and trim_copy(this->value) == "") {
				this->start_pos.position++;
			}
			this->end_pos.position++;
		}

		if ((ch & 0xC0) != 0x80) // check if byte is not continuation of utf8 character
		{
			if (std::isspace(ch) and trim_copy(this->value) == "") {
				this->start_pos.character_number++;
				this->start_pos.position_utf8++;
			}
			this->end_pos.character_number++;
			this->end_pos.position_utf8++;
		}
		last_byte = ch;
		//if(this->in_quote or not std::isspace(ch) or (this->comment() and ch != '\n'))
		value += ch;
		return *this;
	}
	CodeLocation moveStartToEnd() const noexcept {
		CodeLocation res = *this;
		res.start_pos = res.end_pos;
		res.value = "";
		return res;
	}

	CodeLocation move(uint64_t n) const noexcept{
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
			}

			if ((ch & 0xC0) != 0x80) // Check if byte is not continuation of UTF-8 character
			{
				res.start_pos.character_number++;
				res.start_pos.position_utf8++;
			}
		}
		res.value = res.value.substr(n);
		return res;
	}

	CodeLocation substr(uint64_t n) const noexcept{
		CodeLocation res = *this;
		res.end_pos = res.start_pos;
		res.value = "";
		for (int i = 0; i < n and res.end_pos != this->end_pos; i++) {
			res += this->value[i];
		}
		return res;
	}
	bool isDouble() const noexcept {
		bool dot = false;
		for (const auto& i : this->value) {
			if (i == '.' and not dot) dot = true;
			else if (i == '.' and dot) return false;
			else if (not std::isdigit(i) and not isCharIdentifier(i)) return false;
		}
		return dot;
	}
	size_t size() const noexcept { return this->value.size(); }
	std::string val() const noexcept { return this->value; }
	std::string start() const noexcept { return std::to_string(start_pos.line) + ":" + std::to_string(start_pos.position_utf8); }
	std::string end() const noexcept { return std::to_string(end_pos.line) + ":" + std::to_string(end_pos.position_utf8); }
	bool operator<(const CodeLocation& other)const noexcept {
		return this->value < other.value;
	}
};

struct PreambuleDefinition {
	std::string representation;
};

std::vector<PreambuleDefinition> preambles = {
	PreambuleDefinition{"procedure"},
	PreambuleDefinition{"function"},
	PreambuleDefinition{"type"},
	PreambuleDefinition{"type.interface"}, // 
	PreambuleDefinition{"type.agent"}, // 
	PreambuleDefinition{"script"}, // compile time procedure
	PreambuleDefinition{"script.compile"}, // compile time procedure
	PreambuleDefinition{"compile"}, // runed when compiling
	PreambuleDefinition{"sql"}, // sql script
	PreambuleDefinition{"sql.query"}, // sql query
	PreambuleDefinition{"sql.migration"}
};

template <typename T>
concept HasRepresentation = requires(T t) {
	{ t.representation } -> std::convertible_to<std::string>;
};

template<typename T>
bool contains(const std::vector<T>& vec, const std::string& val) requires HasRepresentation<T> {
	for (const auto& item : vec) {
		if (item.representation == val) return true;
	}
	return false;
}
struct OperatorDefinition {
	std::vector<std::string> representation;
	uint8_t presedence;
	bool left_acocieted;
};

bool validPreambuleName(const std::string& str) {
	bool res = true;
	for (const auto& i : str) {
		res = 
			(
				i == '.' or 
				(i >= '0' and i <= '9') or 
				(i >= 'a' and i <= 'z') or 
				(i >= 'A' and i <= 'Z')
			);
		if(res == false) return false;
	}
	return true;
}

std::vector<OperatorDefinition> operators = {
	OperatorDefinition{{"||","or"}, 1, true}, // Logical OR
	OperatorDefinition{{"&&","and"}, 2, true}, // Logical AND
	OperatorDefinition{{"|","bor"}, 3, true}, // Bitwise OR
	OperatorDefinition{{"^","bxor"}, 4, true}, // Bitwise XOR
	OperatorDefinition{{"&","band"}, 5, true}, // Bitwise AND
	OperatorDefinition{{"=="}, 6, true}, // Equality
	OperatorDefinition{{"!="}, 6, true}, // Inequality
	OperatorDefinition{{"is"}, 6, true}, // Identity (Python)
	OperatorDefinition{{"is not"}, 6, true}, // Identity (Python)
	OperatorDefinition{{"<"}, 7, true}, // Less than
	OperatorDefinition{{">"}, 7, true}, // Greater than
	OperatorDefinition{{"<="}, 7, true}, // Less than or equal to
	OperatorDefinition{{">="}, 7, true}, // Greater than or equal to
	OperatorDefinition{{"in"}, 7, true}, // Membership (Python)
	OperatorDefinition{{"not in"}, 7, true}, // Membership (Python)
	OperatorDefinition{{"<=>"}, 7, true}, // Spaceship operator (PHP, Ruby)
	OperatorDefinition{{"<<","shl"}, 8, true}, // Left shift
	OperatorDefinition{{">>","shr"}, 8, true}, // Right shift
	OperatorDefinition{{">>>"}, 8, true}, // Zero-fill right shift (JavaScript)
	OperatorDefinition{{"+"}, 9, true}, // Addition
	OperatorDefinition{{"-"}, 9, true}, // Subtraction
	OperatorDefinition{{"*"}, 10, true}, // Multiplication
	OperatorDefinition{{"/"}, 10, true}, // Division
	OperatorDefinition{{"%"}, 10, true}, // Modulus
	OperatorDefinition{{"**"}, 11, false}, // Exponentiation (Python)
	OperatorDefinition{{"++"}, 12, true}, // Increment
	OperatorDefinition{{"--"}, 12, true}, // Decrement
	OperatorDefinition{{".."}, 13, true}, // Range (inclusive) (Ruby)
	OperatorDefinition{{"..."}, 13, true}, // Range (exclusive) (Ruby)
	OperatorDefinition{{"not","!"}, 14, false}, // Logical NOT (Python)
	OperatorDefinition{{"~","bnot"}, 15, false}, // Bitwise NOT
	OperatorDefinition{{"="}, 17, false}, // Assignment
	OperatorDefinition{{"+="}, 17, false}, // Addition assignment
	OperatorDefinition{{"-="}, 17, false}, // Subtraction assignment
	OperatorDefinition{{"*="}, 17, false}, // Multiplication assignment
	OperatorDefinition{{"/="}, 17, false}, // Division assignment
	OperatorDefinition{{"%="}, 17, false}, // Modulus assignment
	OperatorDefinition{{"<<="}, 17, false}, // Left shift assignment
	OperatorDefinition{{">>="}, 17, false}, // Right shift assignment
	OperatorDefinition{{"&="}, 17, false}, // Bitwise AND assignment
	OperatorDefinition{{"|="}, 17, false}, // Bitwise OR assignment
	OperatorDefinition{{"^="}, 17, false}, // Bitwise XOR assignment
};

template<typename T>
concept HasRepresentationV = requires(T t, const std::string & val) {
	{ t.representation } -> std::convertible_to<std::vector<std::string>>;
};

// Function to check if a vector of `OperatorDefinition` contains a `representation` that matches a given string
template<typename T>
bool contains(const std::vector<T>& vec, const std::string& val) requires HasRepresentationV<T> {
	for (const auto& item : vec) {
		if (std::find(item.representation.begin(), item.representation.end(), val) != item.representation.end()) {
			return true;
		}
	}
	return false;
}

struct CodeFragment {
	PreambuleDefinition preamble;
	std::map<std::string, std::string> atributes;
	std::vector<CodeLocation> body;
	std::string head;
};

struct Lexem {
	enum class Type
	{
		unknown = 0,
		id,
		preamble,
		keyword,
		parathetis,
		op,
		semicolon,
		colon,
		comma,
		string_literal,
		number_literal,
		double_literal,
		char_literal,
		comment,
	};
	Type type = Type::unknown;
	CodeLocation val;
	Lexem(std::string filename) : val(filename) {}
};

static std::string to_string(Lexem::Type type) {
	switch (type) {
		case Lexem::Type::unknown: return "unknown";
		case Lexem::Type::id: return "id";
		case Lexem::Type::preamble: return "preamble";
		case Lexem::Type::keyword: return "keyword";
		case Lexem::Type::parathetis: return "parathetis";
		case Lexem::Type::op: return "op";
		case Lexem::Type::colon: return "colon";
		case Lexem::Type::semicolon: return "semicolon";
		case Lexem::Type::comma: return "comma";
		case Lexem::Type::string_literal: return "string_literal";
		case Lexem::Type::number_literal: return "number_literal";
		case Lexem::Type::double_literal: return "double_literal";
		case Lexem::Type::char_literal: return "char_literal";
		case Lexem::Type::comment: return "comment";
		default: return "unknown";
	}
}

bool eachAnd(std::string str,std::function<bool(uint8_t a)> func) {
	for (const auto& a : str) {
		if (not func(a)) return false;
	}
	return true;
}

bool isDoubleliteral(const std::string& a) {
	bool dot = false;
	for (const auto& i : a) {
		if (i == '.' and dot) return false;
		else if (i == '.' and not dot) { dot = true; }
		else if (not (std::isdigit(i) or (i >= 'a' and i<='z' or i >= 'A' and i <= 'Z') or i == '_')) return false;
	}
	return true;
}

class StreamReader {

	Lexem::Type deduceTokenType(std::string a) {
		if (a == "(" or a == ")" or a == "{" or a == "}" or a == "[" or a == "]") return Lexem::Type::parathetis;
		if (a == ";") return Lexem::Type::semicolon;
		if (a == ":") return Lexem::Type::colon;
		if (a == ",") return Lexem::Type::comma;
		if (std::isdigit(a[0]) and eachAnd(a, isCharIdentifier)) return Lexem::Type::number_literal;
		if ((std::isdigit(a[0]) or a[0] == '.') and isDoubleliteral(a)) return Lexem::Type::double_literal;
		if (contains(operators,a)) return Lexem::Type::op;
		if (contains(preambles,a)) return Lexem::Type::preamble;
		if (eachAnd(a, isCharIdentifier)) return Lexem::Type::id;
	}

	public:
		std::vector<Lexem> lexer(std::string file_name) {
			//basic syntax 
			// # atribute
			// ## file_scope_atribute
			// ### module_scope atribute
			// # option = value
			// ## file_scope_option = value
			// ### module_scope_option = value
			// preambule head { body }

			std::vector<Lexem> res;
			std::ifstream file(file_name);
			Lexem partial(file_name);
			CodeLocation loc(file_name);
			uint8_t next_ch = file.peek();
			uint8_t ch = '\0';
			uint8_t last_ch;

			auto produceLexem = [&](Lexem::Type type = Lexem::Type::unknown) {
				if (trim_copy(loc.val()) != "")
					partial.val = loc;
				else return;
				if (type == Lexem::Type::unknown) {
					assert(partial.type != Lexem::Type::unknown);
				}
				if (partial.type == Lexem::Type::unknown) {
					partial.type = type;
				}
				res.emplace_back(partial);
				//std::cout << partial.val.val() << std::endl;
				loc = loc.moveStartToEnd();
				partial.type = Lexem::Type::unknown;
			};

			do {
				last_ch = ch;
				ch = file.get();
				next_ch = file.peek();
				if (loc.comment()) {
					if (last_ch == '*' and ch == '/') {
						loc += ch;
						loc.commentLevel--;
						produceLexem(Lexem::Type::comment);
					}
					else if (ch == '\n') {
						produceLexem(Lexem::Type::comment);
						loc += ch;
					}
					else if (ch == '/' and next_ch == '/') {
						produceLexem(deduceTokenType(loc.val()));
						loc.lineComment = true;
						loc += ch;
					}
					else if (ch == '/' and next_ch == '*') {
						produceLexem(deduceTokenType(loc.val()));
						loc.commentLevel++;
						loc += ch;
					}
					else {
						loc += ch;
					}
				}
				else if (loc.in_quote) {
					if (ch == '"' and last_ch != '\\') {
						produceLexem(Lexem::Type::string_literal);
						loc.in_quote = false;
					}
					else {
						loc += ch;
					}
				}
				else if (ch == ',') {
					produceLexem(deduceTokenType(loc.val()));
					loc += ch;
					produceLexem(Lexem::Type::comma);
				}
				else if (ch == ';') {
					produceLexem(deduceTokenType(loc.val()));
					loc += ch;
					produceLexem(Lexem::Type::semicolon);
				}
				else if (ch == ':') {
					produceLexem(deduceTokenType(loc.val()));
					loc += ch;
					produceLexem(Lexem::Type::colon);
				}
				else if (ch == '(' or ch == ')' or ch == '{' or ch == '}' or ch == '[' or ch == ']') {
					produceLexem(deduceTokenType(loc.val()));
					loc += ch;
					produceLexem(Lexem::Type::parathetis);
				}
				else if ( // double
					(ch == '.' or (isCharIdentifier(ch))) and (
						((ch == '.' or std::isdigit(ch)) and loc.size() == 0)
						or 
						(loc.size() > 0 and (std::isdigit(loc.val()[0]) or loc.val()[0] == '.') and isDoubleliteral(loc.val()))) 
					)
				{
					if (loc.isDouble())
						partial.type = Lexem::Type::double_literal;
					else
						partial.type = Lexem::Type::number_literal;
					loc += ch;
				}
				else {
					if (ch == '/' and next_ch == '/') {
						produceLexem(deduceTokenType(loc.val()));
						loc.lineComment = true;
						loc += ch;
					}
					else if (ch == '/' and next_ch == '*') {
						produceLexem(deduceTokenType(loc.val()));
						loc.commentLevel++;
						loc += ch;
					}
					else if (not isCharIdentifier(ch)) {
						if (isCharIdentifier(last_ch))
							produceLexem(deduceTokenType(loc.val()));
						if (not loc.in_quote and ch == '"' and last_ch != '\\')
							loc.in_quote = true;
						else
							loc += ch;
					}
					else if (isCharIdentifier(ch)) {
						if (not isCharIdentifier(last_ch)) {
							while (loc.size() > 0) {
								auto s = loc.size();
								for (int i = loc.size(); i > 0; i--) {
									if (contains(operators, loc.val().substr(0, i))) {
										if (trim_copy(loc.val()) != "")
											partial.val = loc;
										else assert(false);
										partial.type = Lexem::Type::op;
										partial.val = loc.substr(i);
										res.emplace_back(partial);
										partial.type = Lexem::Type::unknown;
										loc = loc.move(i);
									}
								}
								if (s == loc.size()) {
									std::cout << "Unrecognized : " << loc.val() << std::endl;
									break;
								}
							}
						}
						loc += ch;
					}
					else {
						loc += ch;
					}
				}
			} while (file.good());
			return res;
		}
};

struct Preambule {
	std::map<CodeLocation,CodeLocation> atributes;
	CodeLocation preambule;
	CodeLocation head;
	CodeLocation body;
	std::vector<CodeLocation> lines;
	Preambule(std::string file_name) : head(file_name),body(file_name), preambule(file_name) {}
};

std::vector<Preambule> metaLexer(std::string filename) {
	std::vector<Preambule> res;
	Preambule partial(filename);
	std::map<CodeLocation, CodeLocation> file_scope_atributes;
	std::ifstream file(filename);
	CodeLocation loc(filename);
	CodeLocation atributeNamePartial(filename);
	CodeLocation line(filename);
	int64_t parenthis = 0;
	char last_ch = '\0';
	char ch = '\0';
	char next_ch = '\0';
	enum class Mode
	{
		idle = 0,atribute_name,atribute_value,preambule,head,body,
	};
	Mode mode = Mode::idle;
	do{
		last_ch = ch;
		ch = file.get();
		next_ch = file.peek();
		switch (mode)
		{
		case Mode::idle:
			if(next_ch == '#') {mode = Mode::atribute_name;loc+=ch;}
			else if(not std::isspace(ch)) { mode = Mode::preambule; loc = loc.moveStartToEnd(); loc+=ch; }
			else if(not std::isspace(next_ch)) {mode = Mode::preambule;loc+=ch;loc = loc.moveStartToEnd();}
			else if(std::isspace(ch)) loc += ch;
			else assert(false);
			break;
		case Mode::atribute_name:
			if (ch == '\n') {
				if (loc.val()[0] == '#') {
					file_scope_atributes.emplace(loc,loc.moveStartToEnd());
				}
				partial.atributes.emplace(loc, loc.moveStartToEnd());
				loc+=ch;
				loc = loc.moveStartToEnd();
				mode = Mode::idle;
			}
			else if (ch == '=') {
				atributeNamePartial = loc;
				loc += ch;
				loc = loc.moveStartToEnd();
			}
			else{
				loc+=ch;
			}
			break;
		case Mode::atribute_value:
			if (ch == '\n') {
				if (atributeNamePartial.val()[0] == '#') {
					file_scope_atributes.emplace(atributeNamePartial, loc);
				}
				partial.atributes.emplace(atributeNamePartial, loc);
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = Mode::idle;
			}
			else {
				loc += ch;
			}
			break;
		case Mode::preambule:
			if (not validPreambuleName(loc.val()+ch)) {
				partial.preambule = loc;
				loc = loc.moveStartToEnd();
				loc+=ch;
				mode = Mode::head;
			}
			else loc += ch;
			break;
		case Mode::head:
			if (ch == '{') {
				mode = Mode::body;
				parenthis = 0;
				partial.head = loc;
				loc+=ch;
				loc = loc.moveStartToEnd();
				line = loc;
			}
			else {
				loc+=ch;
			}
			break;
		case Mode::body:
			if(ch == '{') parenthis++;
			if(ch == '}') parenthis--;
			if(parenthis == -1) {
				mode = Mode::idle;
				partial.body = loc;
				res.push_back(partial); 
				loc+=ch;
				loc = loc.moveStartToEnd();
				partial.lines.clear();
				line = loc;
			}
			else if (ch == '\n') {
				loc += ch;
				partial.lines.emplace_back(line);
				line += ch;
				line = line.moveStartToEnd();
			}
			else {
				loc += ch;
				line +=ch;
			}
			break;
		default:
			break;
		}
	}
	while(file.good());
	return res;
}

int main(int argc,char** args)
{
	if (argc < 2) return - 1;
	//std::ifstream file(args[1]);
	auto a = metaLexer(args[1]);
	for (const auto& [i, elem] : enumerate(a)) {
		std::cout<<"--- === "<<std::setw(3)<<i<<" === ---"<<std::endl;
		std::cout<<"Preambule : " << std::setw(5) <<elem.preambule.start()<<" - "<< std::setw(5) << elem.preambule.end()<<" = " << elem.preambule.val() << std::endl;
		std::cout << "Head : " << std::setw(5) << elem.head.start() << " - " << std::setw(5) << elem.head.end() << " = " << elem.head.val() << std::endl;
		std::cout << "Body : " << std::setw(5) << elem.body.start() << " - " << std::setw(5) << elem.body.end() << " = " << elem.body.val() << std::endl;
		for (const auto& [j, line] : enumerate(elem.lines)) {
			std::cout << std::setw((int)std::ceil(log10(elem.lines.size()))) << std::setw(5) << line.start() << " - " << std::setw(5) << line.end() << ":" << std::setw(30) << "(TODO) - " << line.val() << std::endl;
		}
	}
	//StreamReader sr;
	//auto a = sr.lexer(args[1]);
	//for (const auto& [i, elem] : enumerate(a)) {
	//	std::cout << std::setw((int)std::ceil(log10(a.size())))<<std::setw(5) << elem.val.start()<<" - " << std::setw(5) <<elem.val.end() << ":" << std::setw(30) << "(" + to_string(elem.type) + ") - " << elem.val.val() << std::endl;
	//}

}
