#include "Preambles/Function/Lexer.h"

#include "StringUtility.h"

enum class FunctionTokenType
{
	unknown = 0,
	id,
	op,
	parenthesis,
	comma,
	if_keyword,
	then_keyword,
	else_keyword,
};
Preamble::Function::Lexer::Lexer() {}
void Preamble::Function::Lexer::reset(){}
void Preamble::Function::Lexer::setPreambleIndex(int64_t x) { this->preambleIndex = x;}
std::optional<Token> Preamble::Function::Lexer::lexHead(CodeLocation& loc){
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	uint8_t next_ch = '\0';
	do {
		last_ch = ch;
		ch = loc.look(0);
		next_ch = loc.look(1);
		if (next_ch == '(' and loc.val() != "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{preambleIndex,(Token::Type)FunctionTokenType::id,res};
		}
		else if ( (ch == '(' or ch == ')') and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{preambleIndex,(Token::Type)FunctionTokenType::parenthesis,res};
		}
		else if (ch == ',' and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{preambleIndex,(Token::Type)FunctionTokenType::comma,res};
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc+=ch;
			loc = loc.moveStartToEnd();
			return Token{preambleIndex,(Token::Type)FunctionTokenType::id,res};
		}
		else if ( not isCharIdentifier(ch) and isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{preambleIndex,(Token::Type)FunctionTokenType::op,res};
		}
		else {
			loc+=ch;
		}
	}while(loc.is_good());
	return std::nullopt;
}
std::optional<Token> Preamble::Function::Lexer::lexBody(CodeLocation& loc){
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	uint8_t next_ch = '\0';
	do {
		last_ch = ch;
		ch = loc.look(0);
		next_ch = loc.look(1);
		if (next_ch == '(' and loc.val() != "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{preambleIndex,(Token::Type)FunctionTokenType::id,res};
		}
		else if ((ch == '(' or ch == ')') and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{preambleIndex,(Token::Type)FunctionTokenType::parenthesis,res};
		}
		else if (next_ch == ',' and loc.val() != "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			ASSERT(false,"FUCK");
		}
		else if (ch == ',' and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{preambleIndex,(Token::Type)FunctionTokenType::comma,res};
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{preambleIndex,(Token::Type)FunctionTokenType::id,res};
		}
		else if (not isSpace(ch) and not isCharIdentifier(ch) and isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return Token{preambleIndex,(Token::Type)FunctionTokenType::op,res};
		}
		else {
			loc += ch;
		}
	} while (loc.is_good());
	return std::nullopt;
}
std::string Preamble::Function::Lexer::to_string(Token::Type kind) const{
	switch ((FunctionTokenType)kind) {
		case FunctionTokenType::unknown: return "unknown";
		case FunctionTokenType::id: return "id";
		case FunctionTokenType::op: return "op";
		case FunctionTokenType::parenthesis: return "parenthesis";
		case FunctionTokenType::comma: return "comma";
		case FunctionTokenType::if_keyword: return "if_keyword";
		case FunctionTokenType::then_keyword: return "then_keyword";
		case FunctionTokenType::else_keyword: return "else_keyword";
		default: return "<Unknown>";
	}
}
Preamble::Function::Lexer::~Lexer(){}