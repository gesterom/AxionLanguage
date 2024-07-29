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
	std::optional<uint8_t> next_ch = '\0';
	while(loc.is_good()) {
		last_ch = ch;
		ch = loc.look(0).value();
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
	}
	return std::nullopt;
}
std::optional<Token> Preamble::Procedure::Lexer::lexBody(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	std::optional<uint8_t> next_ch = '\0';
	bool dot = false;
	bool numberStarted = false;
	CodeLocation number(loc);

	auto returnLeftOvers = [&](){
		auto res = loc;
		loc = loc.moveStartToEnd();
		return Token{ preambleIndex,(Token::Type)ProcedureTokenType::id,res };
	};

	while(loc.is_good()) {
		last_ch = ch;
		ch = loc.look(0).value();
		next_ch = loc.look(1);
		if ((isDigit(ch) or (ch == '.' and not dot))) {
			if (ch == '.') {
				dot = true;
			}
			numberStarted = true;
			loc += ch;
		}
		else if (numberStarted and (isSpace(ch) or not isDigit(ch))) {
			auto res = loc;
			loc = loc.moveStartToEnd();
			if (dot) {
				return Token{ preambleIndex, (Token::Type)ProcedureTokenType::double_literal, res };
			}
			else {
				return Token{ preambleIndex, (Token::Type)ProcedureTokenType::integer_literal, res };
			}
		}
		else if (ch == '(' or ch == ')' or ch == '[' or ch == ']') {
			if (not loc.empty()) return returnLeftOvers();

			loc = loc.moveStartToEnd();
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::parenthesis,res };
		}
		else if (ch == ';') {
			if(not loc.empty()) return returnLeftOvers();

			loc = loc.moveStartToEnd();
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::semicolon,res };
		}
		else if (ch == ',') {
			if (not loc.empty()) return returnLeftOvers();
			
			
			loc = loc.moveStartToEnd();
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::comma,res };
		}
		else if (ch == ':') {
			if (not loc.empty()) return returnLeftOvers();

			loc = loc.moveStartToEnd();
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex,(Token::Type)ProcedureTokenType::colon,res };
		}
		else if (not isSpace(ch) and (isSpace(next_ch) or not isCharIdentifier(next_ch))) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{ preambleIndex, (Token::Type)ProcedureTokenType::id, res };
		}
		else {
			loc += ch;
		}
	}
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