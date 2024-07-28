#include "Preambles/Type/Lexer.h"
#include "StringUtility.h"
#include "TODO.h"

enum class TypeTokenType {
	unknown,
	id,
	keyword,
	colon,
	semicolon,
	parenthesis,
};

Preamble::Type::Lexer::Lexer() {

}
void Preamble::Type::Lexer::reset() {

}
void Preamble::Type::Lexer::setPreambleIndex(int64_t x) {
	this->preambleIndex = x;
}
std::optional<Token> Preamble::Type::Lexer::lexHead(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	uint8_t next_ch = '\0';
	do {
		last_ch = ch;
		ch = loc.look(0);
		next_ch = loc.look(1);
		if ((ch == '(' or ch == ')') and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)TypeTokenType::parenthesis,res };
		}
		else if (ch == ':') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)TypeTokenType::colon,res };
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)TypeTokenType::id,res };
		}
		else {
			loc += ch;
		}
	} while (loc.is_good());
	return std::nullopt;
}
std::optional<Token> Preamble::Type::Lexer::lexBody(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	uint8_t next_ch = '\0';
	do {
		last_ch = ch;
		ch = loc.look(0);
		next_ch = loc.look(1);
		if ((ch == '(' or ch == ')' or ch == '[' or ch == ']') and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)TypeTokenType::parenthesis,res };
		}
		else if (ch == ';') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)TypeTokenType::semicolon,res };
		}
		else if (ch == ':') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)TypeTokenType::colon,res };
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			if (to_lowercase(res.val()) == "let") {
				return Token{ preambleIndex,(Token::Type)TypeTokenType::keyword,res };
			}
			return Token{ preambleIndex,(Token::Type)TypeTokenType::id,res };
		}
		else {
			loc += ch;
		}
	} while (loc.is_good());
	return std::nullopt;
}
std::string Preamble::Type::Lexer::to_string(Token::Type kind) const {
	switch ((TypeTokenType)kind) {
	case TypeTokenType::unknown: return "unknown";
	case TypeTokenType::id: return "id";
	case TypeTokenType::colon: return "colon";
	case TypeTokenType::semicolon: return "semicolon";
	case TypeTokenType::parenthesis: return "parenthesis";
	case TypeTokenType::keyword: return "keyword";
	default: return "<unknown>";
	}
}
Preamble::Type::Lexer::~Lexer() {

}