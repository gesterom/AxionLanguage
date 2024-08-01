#include "PreambleRepository.h"

#include "Preambles/Function/Lexer.h"
#include "Preambles/Procedure/Lexer.h"
#include "Preambles/Type/Lexer.h"		

#include "Preambles/Procedure/Parser.h"

#include "PreambleDefinition.h"

#include "format"

std::optional<Token> nop_lexer(int64_t preambleIndex, LexerMode& mode, CodeLocation& loc, std::istream& in) {
	return std::nullopt;
}


std::string PreambleRepository::prambleName(Token::Type pre) const
{
	if (pre <= Token::count) return "<Meta>";
	else if (vec.size() - 1 > 100000) TODO("Too much preambles");
	else if (getPreamble(pre) < (int64_t)(vec.size() - 1 % 100000)) return vec.at( getPreamble(pre) )->representation;
	return "<Incorect Preamble Index>";
}

std::string meta_to_string(Token::Type t)
{
	switch (t)
	{
	case Token::error: return "error";
	case Token::unknown: return "unknown";
	case Token::preamble: return "preamble";
	case Token::file_scope_atribute_name: return "file_scope_atribute_name";
	case Token::file_scope_atribute_value: return "file_scope_atribute_value";
	case Token::atribute_name: return "atribute_name";
	case Token::atribute_value: return "atribute_value";
	case Token::parenthesis: return "parenthesis";
	case Token::comment: return "comment";
	case Token::string_literal: return "string_literal";
	case Token::atom: return "atom";
	case Token::count: return "Invalid :\"Count\"";
	default: return "<invalid>";
	}
}

std::string PreambleRepository::to_string(Token::Type t) const
{
	if (t <= Token::count) return meta_to_string(t);
	else if (vec.size() - 1 > 100000) TODO("Too much preambles");
	else if (getPreamble(t) < (int64_t)(vec.size() - 1 % 100000)) return vec.at(getPreamble(t))->lexer->to_string(t);
	return "<Incorect Preamble Index>";
}

PreambleRepository::PreambleRepository()
{
	auto addPreamble = [&](PreambleDefinition* pd) {
		vec.push_back(pd);
		if (vec.at(vec.size() - 1)->lexer != nullptr)
			vec.at(vec.size() - 1)->lexer->setPreambleIndex((int32_t)vec.size() - 1);
		};

	addPreamble(new PreambleDefinition{ "<Meta>"});
	addPreamble(new PreambleDefinition{ "procedure",new Preamble::Procedure::Lexer(),new Preamble::Procedure::Parser()});
	addPreamble(new PreambleDefinition{ "function",new Preamble::Function::Lexer() });
	addPreamble(new PreambleDefinition{ "type",new Preamble::Type::Lexer() });
	addPreamble(new PreambleDefinition{ "type.distinct",new Preamble::Type::Lexer() });
	addPreamble(new PreambleDefinition{ "type.alias",new Preamble::Type::Lexer() });
	addPreamble(new PreambleDefinition{ "type.interface",nullptr }); //
	addPreamble(new PreambleDefinition{ "build",new Preamble::Procedure::Lexer(),new Preamble::Procedure::Parser() }); // runed to compile similar to nobuild(https://github.com/tsoding/nobuild)
	//addPreamble(new PreambleDefinition{ "build.procedure",nullptr }); // build time procedure
	addPreamble(new PreambleDefinition{ "sql",nullptr }); // sql scrip
	addPreamble(new PreambleDefinition{ "sql.query",nullptr }); // sql quer
	addPreamble(new PreambleDefinition{ "sql.migration",nullptr });
}

std::vector<PreambleDefinition*> PreambleRepository::get() const
{
	std::vector<PreambleDefinition*> res;
	for (auto i : vec) {
		res.push_back(i);
	}
	return res;
}

PreambleDefinition* PreambleRepository::get(int32_t index) const
{
	ASSERT(vec.size() < 100000, "TOO large");
	ASSERT(index >= 0 && index < (int64_t)vec.size(), "out of bound");
	return vec[index];
}

int32_t PreambleRepository::getPeambuleIndex(CodeLocation representation) const {
	int i = 0;

	for (const auto& elem : vec) {
		if (elem->representation == representation.val()) {
			return i;
		}
		i++;
	}
	TODO(std::format("preamble '{}' not found ! in {} ", representation.val(), representation.start()));
}

PreambleRepository::~PreambleRepository()
{
	for (auto i : vec) {
		delete i;
	}
}
