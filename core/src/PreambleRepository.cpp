#include "PreambleRepository.h"

#include "Preambles/Function/Lexer.h"
#include "Preambles/Type/Lexer.h"		

#include "PreambleDefinition.h"

std::optional<Token> nop_lexer(int64_t preambleIndex, LexerMode& mode, CodeLocation& loc, std::istream& in) {
	return std::nullopt;
}

std::string unknow_to_string(Token::Type t) {
	return "<Unknow>";
}

std::string PreambleRepository::to_string(Token::PreambleType t) const
{
	if (t < 0) return "<Meta>";
	else if (vec.size() - 1 > 100000) TODO("Too much preambles");
	else if (t < (int64_t)(vec.size() - 1 % 100000)) return vec.at(t)->representation;
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
	default: return "invalid";
	}
}

std::string PreambleRepository::to_string(Token::PreambleType pre, Token::Type t) const
{
	if (pre < 0) return meta_to_string((Token::Type)t);
	else if (vec.size() - 1 > 100000) TODO("Too much preambles");
	else if (pre < (int64_t)(vec.size() - 1 % 100000)) return vec.at(pre)->lexer->to_string(t);
	return "<Incorect Preamble Index>";
}

PreambleRepository::PreambleRepository()
{
	auto addPreamble = [&](PreambleDefinition* pd){
		vec.push_back(pd);
		if(vec.at(vec.size() - 1)->lexer != nullptr)
			vec.at(vec.size() - 1)->lexer->setPreambleIndex(vec.size() - 1);
	};

	addPreamble(new PreambleDefinition{ "procedure",nullptr });
	addPreamble(new PreambleDefinition{ "function",new Preamble::Function::Lexer() });
	addPreamble(new PreambleDefinition{ "type",new Preamble::Type::Lexer()});
	addPreamble(new PreambleDefinition{ "type.interface",nullptr }); //
	addPreamble(new PreambleDefinition{ "type.agent",nullptr }); //
	addPreamble(new PreambleDefinition{ "script",nullptr }); // compile time procedur
	addPreamble(new PreambleDefinition{ "script.compile",nullptr }); // compile time procedur
	addPreamble(new PreambleDefinition{ "compile",nullptr }); // runed when compilin
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

PreambleDefinition* PreambleRepository::get(int64_t index) const
{
	ASSERT(vec.size() < 100000, "TOO large");
	ASSERT(index >= 0 && index < (int64_t)vec.size(),"out of bound");
	return vec[index];
}

int64_t PreambleRepository::getPeambuleIndex(std::string representation) const {
	int i = 0;
	for (const auto& elem : vec) {
		if (elem->representation == representation) {
			return i;
		}
		i++;
	}
	TODO("preamble not found");
}

PreambleRepository::~PreambleRepository()
{
	for (auto i : vec) {
		delete i;
	}
}
