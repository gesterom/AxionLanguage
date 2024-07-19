#include "Preambules/Function/Lexer.h"

enum class FunctionTokenType
{
	unknown = 0,
	id,
	op,
	paranthis,
	comma,
	if_keyword,
	then_keyword,
	else_keyword,
};
Preambule::Function::Lexer::Lexer() {}
void Preambule::Function::Lexer::reset(){}
void Preambule::Function::Lexer::setPreambuleIndex(int64_t x) { this->preambuleIndex = x;}
std::pair<std::optional<Token>, LexerMode> Preambule::Function::Lexer::lexHead(CodeLocation& loc, uint8_t ch){
	uint8_t last_ch = '\0';
	uint8_t next_ch = loc.peek(2)[1];
	do {		
		if (ch == '{') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{ -1,Token::Type::paranthis,res }, LexerMode::body };
		}
		else if (next_ch == '(' and loc.val() != "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambuleIndex,(Token::Type)FunctionTokenType::id,res},LexerMode::head };
		}
		else if ( (ch == '(' or ch == ')') and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambuleIndex,(Token::Type)FunctionTokenType::paranthis,res},LexerMode::head };
		}
		else if (ch == ',' and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambuleIndex,(Token::Type)FunctionTokenType::comma,res},LexerMode::head };
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc+=ch;
			loc = loc.moveStartToEnd();
			return { Token{preambuleIndex,(Token::Type)FunctionTokenType::id,res},LexerMode::head };
		}
		else if ( not isCharIdentifier(ch) and isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambuleIndex,(Token::Type)FunctionTokenType::op,res},LexerMode::head };
		}
		else {
			loc+=ch;
		}
		last_ch = ch;
		ch = loc.peek();
		next_ch = loc.peek(2)[1];
	}while(loc.is_good());
	return { std::nullopt,LexerMode::idle };
}
std::pair<std::optional<Token>, LexerMode> Preambule::Function::Lexer::lexBody(CodeLocation& loc, uint8_t ch){
	uint8_t last_ch = '\0';
	uint8_t next_ch = loc.peek(2)[1];
	do {
		if (ch == '}') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{ -1,Token::Type::paranthis,res }, LexerMode::idle };
		}
		else if (next_ch == '(' and loc.val() != "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambuleIndex,(Token::Type)FunctionTokenType::id,res},LexerMode::body };
		}
		else if ((ch == '(' or ch == ')') and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambuleIndex,(Token::Type)FunctionTokenType::paranthis,res},LexerMode::body };
		}
		else if (next_ch == ',' and loc.val() != "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			ASSERT(false,"FUCK");
		}
		else if (ch == ',' and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambuleIndex,(Token::Type)FunctionTokenType::comma,res},LexerMode::body };
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambuleIndex,(Token::Type)FunctionTokenType::id,res},LexerMode::body };
		}
		else if (not isSpace(ch) and not isCharIdentifier(ch) and isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambuleIndex,(Token::Type)FunctionTokenType::op,res},LexerMode::body };
		}
		else {
			loc += ch;
		}
		last_ch = ch;
		ch = loc.peek();
		next_ch = loc.peek(2)[1];
	} while (loc.is_good());
	return { std::nullopt,LexerMode::idle };
}
std::string Preambule::Function::Lexer::to_string(Token::Type kind) const{
	switch ((FunctionTokenType)kind) {
		case FunctionTokenType::unknown: return "unknown";
		case FunctionTokenType::id: return "id";
		case FunctionTokenType::op: return "op";
		case FunctionTokenType::paranthis: return "paranthis";
		case FunctionTokenType::comma: return "comma";
		case FunctionTokenType::if_keyword: return "if_keyword";
		case FunctionTokenType::then_keyword: return "then_keyword";
		case FunctionTokenType::else_keyword: return "else_keyword";
		default: return "<Unknown>";
	}
}
Preambule::Function::Lexer::~Lexer(){}