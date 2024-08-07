#include "Preambles/Type/Lexer.h"
#include "StringUtility.h"
#include "TODO.h"


Preamble::Type::Lexer::Lexer() {

}
//void Preamble::Type::Lexer::reset() {}

void Preamble::Type::Lexer::setPreambleIndex(int32_t x) {
	this->preambleIndex = x;
}

std::optional<Token> Preamble::Type::Lexer::lexHead(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	std::optional<uint8_t> next_ch = '\0';
	while(loc.is_good()) {
		last_ch = ch;
		ch = loc.look(0).value();
		next_ch = loc.look(1);
		if ((ch == '(' or ch == ')') and loc.to_string() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken(Token::Type::parenthesis,res );
		}
		else if (ch == ':') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken((Token::Type)TypeTokenType::colon,res );
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken(Token::Type::atom,res );
		}
		else {
			loc += ch;
		}
	}
	return std::nullopt;
}
std::optional<Token> Preamble::Type::Lexer::lexBody(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	std::optional<uint8_t> next_ch = '\0';
	while(loc.is_good()){
		last_ch = ch;
		ch = loc.look(0).value();
		next_ch = loc.look(1);
		if ((ch == '(' or ch == ')' or ch == '[' or ch == ']') and loc.to_string() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken(Token::Type::parenthesis,res );
		}
		else if (ch == ';') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken( (Token::Type)TypeTokenType::semicolon,res );
		}
		else if (ch == ':') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return createToken((Token::Type)TypeTokenType::colon,res );
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			if (to_lowercase(res.to_string()) == "let") {
				return createToken((Token::Type)TypeTokenType::keyword,res );
			}
			return createToken(Token::Type::atom,res );
		}
		else {
			loc += ch;
		}
	}
	return std::nullopt;
}
std::string Preamble::Type::Lexer::to_string(Token::Type kind) const {
	switch ((TypeTokenType)kind) {
	case TypeTokenType::colon: return "colon";
	case TypeTokenType::semicolon: return "semicolon";
	case TypeTokenType::keyword: return "keyword";
	default: return "<unknown>";
	}
}
Preamble::Type::Lexer::~Lexer() {

}