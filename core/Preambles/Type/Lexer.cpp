#include "Preambles/Type/Lexer.h"
#include "TODO.h"
#include "StringUtility.h"

enum class TypeTokenType {
	unknown,
	id,
	keyword,
	colon,
	semicolon,
	parenthesis,
};

Preamble::Type::Lexer::Lexer(){
	
}
void Preamble::Type::Lexer::reset() {
	
}
void Preamble::Type::Lexer::setPreambleIndex(int64_t x) {
	this->preambleIndex = x;
}
std::pair<std::optional<Token>, LexerMode> Preamble::Type::Lexer::lexHead(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = loc.look(0);
	uint8_t next_ch = loc.look(1);
	do {
		if (ch == '{') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{ -1,Token::Type::parenthesis,res }, LexerMode::body };
		}
		else if ((ch == '(' or ch == ')') and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambleIndex,(Token::Type)TypeTokenType::parenthesis,res},LexerMode::head };
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambleIndex,(Token::Type)TypeTokenType::id,res},LexerMode::head };
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
std::pair<std::optional<Token>, LexerMode> Preamble::Type::Lexer::lexBody(CodeLocation& loc) {
	uint8_t last_ch = '\0';
	uint8_t ch = loc.look(0);
	uint8_t next_ch = loc.look(1);
	do {
		if (ch == '}') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{ -1,Token::Type::parenthesis,res }, LexerMode::idle };
		}
		else if ((ch == '(' or ch == ')' or ch =='[' or ch ==']') and loc.val() == "") {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambleIndex,(Token::Type)TypeTokenType::parenthesis,res},LexerMode::body };
		}
		else if (ch == ';') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambleIndex,(Token::Type)TypeTokenType::semicolon,res},LexerMode::body };
		}
		else if (ch == ':') {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			return { Token{preambleIndex,(Token::Type)TypeTokenType::colon,res},LexerMode::body };
		}
		else if (isCharIdentifier(ch) and not isCharIdentifier(next_ch)) {
			auto res = loc += ch;
			loc = loc.moveStartToEnd();
			if (to_lowercase(res.val()) == "let") {
				return { Token{preambleIndex,(Token::Type)TypeTokenType::keyword,res},LexerMode::body };
			}
			return { Token{preambleIndex,(Token::Type)TypeTokenType::id,res},LexerMode::body };
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
std::string Preamble::Type::Lexer::to_string(Token::Type kind) const {
	switch((TypeTokenType)kind){
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