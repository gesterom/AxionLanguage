#include "PreambuleRepository.h"

#include "Preambules/Function/Lexer.h"

#include "PreambuleDefinition.h"

std::optional<Token> nop_lexer(int64_t preambuleIndex, LexerMode& mode, CodeLocation& loc, std::istream& in) {
	return std::nullopt;
}

std::string unknow_to_string(Token::Type t) {
	return "<Unknow>";
}

std::string PreambuleRepository::to_string(Token::PreambuleType t) const
{
	if (t < 0) return "<Meta>";
	else if (vec.size() - 1 > 100000) TODO("Too much preambles");
	else if (t < (int64_t)(vec.size() - 1 % 100000)) return vec.at(t)->representation;
	return "<Incorect Preambule Index>";
}

std::string meta_to_string(Token::Type t)
{
	switch (t)
	{
	case Token::error: return "error";
	case Token::unknown: return "unknown";
	case Token::preambule: return "preambule";
	case Token::file_scope_atribute_name: return "file_scope_atribute_name";
	case Token::file_scope_atribute_value: return "file_scope_atribute_value";
	case Token::atribute_name: return "atribute_name";
	case Token::atribute_value: return "atribute_value";
	case Token::paranthis: return "paranthis";
	case Token::comment: return "comment";
	default: return "invalid";
	}
}

std::string PreambuleRepository::to_string(Token::PreambuleType pre, Token::Type t) const
{
	if (pre < 0) return meta_to_string((Token::Type)t);
	else if (vec.size() - 1 > 100000) TODO("Too much preambles");
	else if (pre < (int64_t)(vec.size() - 1 % 100000)) return vec.at(pre)->lexer->to_string(t);
	return "<Incorect Preambule Index>";
}

PreambuleRepository::PreambuleRepository()
{
	auto addPreambule = [&](PreambuleDefinition* pd){
		vec.push_back(pd);
		if(vec.at(vec.size() - 1)->lexer != nullptr)
			vec.at(vec.size() - 1)->lexer->setPreambuleIndex(vec.size() - 1);
	};

	addPreambule(new PreambuleDefinition{ "procedure",nullptr });
	addPreambule(new PreambuleDefinition{ "function",new Preambule::Function::Lexer() }); 
	addPreambule(new PreambuleDefinition{ "type",nullptr });
	addPreambule(new PreambuleDefinition{ "type.interface",nullptr }); //
	addPreambule(new PreambuleDefinition{ "type.agent",nullptr }); //
	addPreambule(new PreambuleDefinition{ "script",nullptr }); // compile time procedur
	addPreambule(new PreambuleDefinition{ "script.compile",nullptr }); // compile time procedur
	addPreambule(new PreambuleDefinition{ "compile",nullptr }); // runed when compilin
	addPreambule(new PreambuleDefinition{ "sql",nullptr }); // sql scrip
	addPreambule(new PreambuleDefinition{ "sql.query",nullptr }); // sql quer
	addPreambule(new PreambuleDefinition{ "sql.migration",nullptr });
}

std::vector<PreambuleDefinition*> PreambuleRepository::get() const
{
	std::vector<PreambuleDefinition*> res;
	for (auto i : vec) {
		res.push_back(i);
	}
	return res;
}

PreambuleDefinition* PreambuleRepository::get(int64_t index) const
{
	ASSERT(vec.size() < 100000, "TOO large");
	ASSERT(index >= 0 && index < (int64_t)vec.size(),"out of bound");
	return vec[index];
}

int64_t PreambuleRepository::getPeambuleIndex(std::string representation) const {
	int i = 0;
	for (const auto& elem : vec) {
		if (elem->representation == representation) {
			return i;
		}
		i++;
	}
	TODO("preambule not found");
}

PreambuleRepository::~PreambuleRepository()
{
	for (auto i : vec) {
		delete i;
	}
}
