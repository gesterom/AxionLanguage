#include "MetaLexer.h"

#include <format>

#include "PreambleDefinition.h"
#include "StringUtility.h"
#include "TODO.h"

MetaLexer::MetaLexer(const PreambleRepository& repo, std::string file_name) : loc(file_name), tempLoc(loc), repo(repo) {}

std::optional<Token> MetaLexer::lex() {
	CodeLocation line = loc;
	uint8_t last_ch = '\0';
	uint8_t ch = '\0';
	if (not loc.is_good() and not tempLoc.is_good()) return std::nullopt;
	do {
		last_ch = ch;
		ch = loc.look(0);
		uint8_t next_ch = loc.look(1);
		switch (mode)
		{
		case LexerMode::idle:
			if (ch == '#') { mode = LexerMode::atribute_name; loc += ch; loc = loc.moveStartToEnd(); }
			else if (ch == '/' and next_ch == '/') { mode = LexerMode::comment; loc = loc.moveStartToEnd();  loc += ch; }
			else if (ch == '/' and next_ch == '*') { mode = LexerMode::comment; loc = loc.moveStartToEnd();  loc += ch; }
			else if (validPreambleChar(ch)) { mode = LexerMode::preamble; loc = loc.moveStartToEnd(); loc += ch; }
			else if (isSpace(ch)) loc += ch;
			else { mode = LexerMode::error_recovery; loc = loc.moveStartToEnd(); loc += ch; }
			break;
		case LexerMode::error_recovery:
			TODO("implement error recovery (search for known preamble) ");
			break;
		case LexerMode::comment:
			if (ch == '\n') {
				auto res = loc;
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = afterComment;
				return Token{ -1,Token::Type::comment,res };
			}
			else {
				loc += ch;
			}
			break;
		case LexerMode::multilineComment:
			if (last_ch == '*' and ch == '/') {
				commentLevel--;
				loc += ch;
				if (commentLevel == 0) {
					auto res = loc;
					loc = loc.moveStartToEnd();
					mode = afterComment;
					return Token{ -1,Token::Type::comment,res };
				}
			}
			else if (ch == '/' and next_ch == '*') {
				commentLevel++;
				loc += ch;
			}
			else {
				loc += ch;
			}
			break;
		case LexerMode::string_literal:
			if (last_ch == '\\' and ch == start_string_lieral) {
				loc += ch;
			}
			else if (ch == start_string_lieral) {
				auto res = loc;
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = afterComment;
				return Token{ -1,Token::Type::string_literal,res };
			}
			else {
				loc += ch;
			}
			break;
		case LexerMode::emit_body_closing_bracker: {
			auto res = loc += ch;
			mode = afterComment;
			return Token{ -1,Token::Type::parenthesis,res };
		}break;
		case LexerMode::file_atribute_name:
			if (last_ch == '#' and ch == '#') { loc += ch; loc = loc.moveStartToEnd(); }
			else if (ch == '\n') {
				auto res = loc;
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = LexerMode::idle;
				//file_scope_atributes.emplace(res, res.moveStartToEnd());
				return Token{ -1,Token::Type::file_scope_atribute_name,loc };
			}
			else if (ch == '=') {
				auto res = loc;
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = LexerMode::file_atribute_value;
				return Token{ -1,Token::Type::file_scope_atribute_name,res };
			}
			else {
				loc += ch;
			}
			break;
		case LexerMode::file_atribute_value:
			if (ch == '\n') {
				auto res = loc;
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = LexerMode::idle;
				//file_scope_atributes.emplace(atributeNamePartial, res);
				return Token{ -1,Token::Type::file_scope_atribute_value,res };
			}
			else {
				loc += ch;
			}
			break;
		case LexerMode::atribute_name:
			if (loc.val() == "" and ch == '#')
			{
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = LexerMode::file_atribute_name;
			}
			else if (ch == '\n') {
				auto res = loc;
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = LexerMode::idle;
				//partial.atributes.emplace(res, res.moveStartToEnd());
				return Token{ -1,Token::Type::atribute_name,res };
			}
			else if (ch == '=') {
				auto res = loc;
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = LexerMode::atribute_value;
				return Token{ -1,Token::Type::atribute_name,res };
			}
			else {
				loc += ch;
			}
			break;
		case LexerMode::atribute_value:
			if (ch == '\n') {
				auto res = loc;
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = LexerMode::idle;
				//partial.atributes.emplace(atributeNamePartial, res);
				return Token{ -1,Token::Type::atribute_value,res };
			}
			else {
				loc += ch;
			}
			break;
		case LexerMode::preamble:
			if (validPreambleChar(ch)) {
				loc += ch;
			}
			else if (isSpace(ch)) {
				auto res = loc;
				loc = loc.moveStartToEnd();
				loc += ch;
				mode = LexerMode::head;
				preambleIndex = repo.getPeambuleIndex(res);
				return Token{ -1,Token::Type::preamble,res };
			}
			else {
				TODO("Not valid preamble name");
			}
			break;
		case LexerMode::head:
			if (repo.get(preambleIndex)->lexer == nullptr) {
				auto res = loc;
				mode = LexerMode::error_recovery;
				loc += ch;
				loc = loc.moveStartToEnd();
				return Token{ -1,Token::Type::error,res };
			}
			else if (ch == '{') {
				this->tempLoc = loc.asLimiter();
				loc = loc.moveStartToEnd();
				mode = LexerMode::head_specific;
				nextMode = LexerMode::body;
			}
			else if (ch == '}') {
				mode = LexerMode::error_recovery;
			}
			else if (ch == '"') {
				loc += ch;
				loc = loc.moveStartToEnd();
				start_string_lieral = '"';
				mode = LexerMode::head_specific;
				nextMode = LexerMode::string_literal;
				afterComment = LexerMode::head;
			}
			else if (ch == '\'') {
				loc += ch;
				loc = loc.moveStartToEnd();
				start_string_lieral = '\'';
				mode = LexerMode::head_specific;
				nextMode = LexerMode::string_literal;
				afterComment = LexerMode::head;
			}
			else if (ch == '/' and next_ch == '/') {
				this->tempLoc = loc.asLimiter();
				loc = loc.moveStartToEnd();
				mode = LexerMode::head_specific;
				nextMode = LexerMode::comment;
				afterComment = LexerMode::head;
			}
			else if (ch == '/' and next_ch == '*') {
				this->tempLoc = loc.asLimiter();
				loc = loc.moveStartToEnd();
				mode = LexerMode::head_specific;
				nextMode = LexerMode::multilineComment;
				afterComment = LexerMode::head;
			}
			else {
				loc += ch;
			}
			break;
		case LexerMode::head_specific: {
			auto res = repo.get(preambleIndex)->lexer->lexHead(tempLoc);
			if (res != std::nullopt) return res;
			auto res2 = loc;
			loc = loc.moveStartToEnd();
			mode = nextMode;
		}break;
		case LexerMode::body:
			if (repo.get(preambleIndex)->lexer == nullptr) {
				auto res = loc;
				mode = LexerMode::error_recovery;
				loc += ch;
				loc = loc.moveStartToEnd();
				return Token{ -1,Token::Type::error,res };
			}
			else if (ch == '"') {
				this->tempLoc = loc.asLimiter();
				loc += ch;
				loc = loc.moveStartToEnd();
				start_string_lieral = '"';
				mode = LexerMode::body_specific;
				nextMode = LexerMode::string_literal;
				afterComment = LexerMode::body;
			}
			else if (ch == '\'') {
				this->tempLoc = loc.asLimiter();
				loc += ch;
				loc = loc.moveStartToEnd();
				start_string_lieral = '\'';
				mode = LexerMode::body_specific;
				nextMode = LexerMode::string_literal;
				afterComment = LexerMode::body;
			}
			else if (ch == '{' and loc.size() == 0) {
				this->parentisisCounter++;
				auto res = loc += ch;
				loc = loc.moveStartToEnd();
				return Token{ -1,Token::Type::parenthesis,res };
			}
			else if (ch == '{' and loc.size() != 0) {
				tempLoc = loc.asLimiter();
				loc = loc.moveStartToEnd();
				mode = LexerMode::body_specific;
				nextMode = LexerMode::body;
			}
			else if (ch == '}') {
				this->parentisisCounter--;
				if (this->parentisisCounter <= 0) {
					tempLoc = loc.asLimiter();
					loc = loc.moveStartToEnd();
					mode = LexerMode::body_specific;
					nextMode = LexerMode::emit_body_closing_bracker;
					afterComment = LexerMode::idle;
				}
				else
					loc += ch;
			}
			else if (ch == '/' and next_ch == '/') {
				this->tempLoc = loc.asLimiter();
				loc = loc.moveStartToEnd();
				mode = LexerMode::body_specific;
				nextMode = LexerMode::comment;
				afterComment = LexerMode::body;
			}
			else if (ch == '/' and next_ch == '*') {
				this->tempLoc = loc.asLimiter();
				loc = loc.moveStartToEnd();
				mode = LexerMode::body_specific;
				nextMode = LexerMode::multilineComment;
				afterComment = LexerMode::body;
			}
			else {
				loc += ch;
			}break;
		case LexerMode::body_specific: {
			auto res = repo.get(preambleIndex)->lexer->lexBody(tempLoc);
			if (res != std::nullopt) return res;
			mode = nextMode;
		}break;
		default:
			TODO("unexpectet internal error ? ");
			break;
		}
	} while (loc.is_good() or tempLoc.is_good());
	return std::nullopt;
}