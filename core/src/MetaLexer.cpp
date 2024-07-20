#include "MetaLexer.h"

#include <format>

#include "PreambleDefinition.h"
#include "TODO.h"
#include "StringUtility.h"

MetaLexer::MetaLexer(const PreambleRepository& repo,std::string file_name) : loc(file_name), file(file_name), repo(repo) {}

std::optional<Token> MetaLexer::lex() {
	CodeLocation line = loc;
	if (not loc.is_good()) return std::nullopt;
	do {
		last_ch = ch;
		ch = loc.peek();
		next_ch = loc.peek(2)[1];
		switch (mode)
		{
		case LexerMode::idle:
			if (ch == '#') { mode = LexerMode::atribute_name; loc += ch; loc = loc.moveStartToEnd(); }
			else if (ch == '/' and next_ch == '/') {mode = LexerMode::comment; loc = loc.moveStartToEnd();  loc += ch;}
			else if (validPreambleChar(ch)) { mode = LexerMode::preamble; loc = loc.moveStartToEnd(); loc += ch; }
			else if (isSpace(ch)) loc += ch;
			else TODO("idle not recoginized character");
			break;
		case LexerMode::comment:
			if(ch == '\n') {
				mode = LexerMode::idle;
				auto res = loc;
				loc += ch;
				loc = loc.moveStartToEnd();
				mode = LexerMode::idle;
				return Token{ -1,Token::Type::comment,res};
			}
			else {
				loc+=ch;
			}
			break;
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
				preambleIndex = repo.getPeambuleIndex(res.val());
				return Token{ -1,Token::Type::preamble,res };
			}
			else {
				TODO("Not valid preamble name");
			}
			break;
		case LexerMode::head:{
			ASSERT(repo.get(preambleIndex)->lexer != nullptr, std::format("TODO {}",repo.get(preambleIndex)->representation));
			auto [res,mode_] = repo.get(preambleIndex)->lexer->lexHead(loc,ch);
			loc = loc.moveStartToEnd();
			mode = mode_;
			return res;
			}break;
		case LexerMode::body:{
			auto [res, mode_] = repo.get(preambleIndex)->lexer->lexBody(loc,ch);
			loc = loc.moveStartToEnd();
			mode = mode_;
			return res;
			}break;
		default:
			TODO("unexpectet internal error ? ");
			break;
		}
	} while (loc.is_good());
	return std::nullopt;
}