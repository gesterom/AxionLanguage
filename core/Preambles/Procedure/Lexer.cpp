#include "Preambles/Procedure/Lexer.h"

#include "StringUtility.h"

enum class ProcedureTokenType {
	unknown,
	id,
	keyword,
	comma,
	colon,
	semicolon,
	parenthesis,
	string_literal,
	integer_literal,
	double_literal,
};

Preamble::Procedure::Lexer::Lexer() {

}
void Preamble::Procedure::Lexer::reset() {

}
void Preamble::Procedure::Lexer::setPreambleIndex(int64_t x) {
	this->preambleIndex = x;
}
std::optional<Token> Preamble::Procedure::Lexer::lexHead(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	uint8_t next_ch = '\0';
	do {
		last_ch = ch;
		ch = loc.look(0);
		next_ch = loc.look(1);
		if (ch == ':') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::colon,res };
		}
		else if (ch == ',') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::comma,res };
		}
		else if ((ch == '(' or ch == ')')) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::parenthesis,res };
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::id,res };
		}
		else {
			loc += ch;
		}
	} while (loc.is_good());
	return std::nullopt;
}
std::optional<Token> Preamble::Procedure::Lexer::lexBody(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	uint8_t next_ch = '\0';
	bool dot = false;
	CodeLocation number(loc);
	do {
		last_ch = ch;
		ch = loc.look(0);
		next_ch = loc.look(1);
		if ((ch == '(' or ch == ')' or ch == '[' or ch == ']') and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::parenthesis,res };
		}
		else if (ch == ';') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::semicolon,res };
		}
		else if (ch == ',') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::comma,res };
		}
		else if (ch == ':') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::colon,res };
		}
		else if (isSpace(ch))
		{
			loc += ch;
		}
		else if ((isdigit(ch) or ch == '.') and ((not isdigit(next_ch) and next_ch != '.') or isspace(next_ch))) {
			if (ch == '.') dot = true;
			if (dot) {
				auto res = loc += ch;;
				loc = loc.moveStartToEnd();
				return Token{ preambleIndex,(Token::Type)ProcedureTokenType::double_literal,res };
			}
			else {
				auto res = loc += ch;;
				loc = loc.moveStartToEnd();
				return Token{ preambleIndex,(Token::Type)ProcedureTokenType::integer_literal,res };
			}
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			if (to_lowercase(res.val()) == "let") {
				return Token{ preambleIndex,(Token::Type)ProcedureTokenType::keyword,res };
			}
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::id,res };
		}
		else {
			if (ch == '.') dot = true;
			loc += ch;
		}
	} while (loc.is_good());
	return std::nullopt;
}
std::string Preamble::Procedure::Lexer::to_string(Token::Type kind) const {
	switch ((ProcedureTokenType)kind) {
	case ProcedureTokenType::unknown: return "unknown";
	case ProcedureTokenType::id: return "id";
	case ProcedureTokenType::colon: return "colon";
	case ProcedureTokenType::comma: return "comma";
	case ProcedureTokenType::semicolon: return "semicolon";
	case ProcedureTokenType::parenthesis: return "parenthesis";
	case ProcedureTokenType::keyword: return "keyword";
	case ProcedureTokenType::double_literal: return "double_literal";
	case ProcedureTokenType::integer_literal: return "integer_literal";
	case ProcedureTokenType::string_literal: return "string_literal";
	default: return "<unknown>";
	}
}
Preamble::Procedure::Lexer::~Lexer() {

}