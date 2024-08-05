#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>	

#include "AST.h"
#include "CodeLocation.h"
#include "MetaLexer.h"
#include "TODO.h"
#include "Token.h"
#include "TokenStream.h"	

class ILexer {
public:
	//virtual void reset() = 0;
	virtual void setPreambleIndex(int32_t x) = 0;
	virtual std::optional<Token> lexHead(CodeLocation& loc) = 0;
	virtual std::optional<Token> lexBody(CodeLocation& loc) = 0;
	virtual std::string to_string(Token::Type kind) const = 0;
	virtual ~ILexer() noexcept {}
};

//type.implicit_cast TypeFrom -> TypeTo
//type.explicit_cast TypeFrom -> TypeTo (used in "exp as atom" expresion/operator)
/*
{

}
*/

class IParser {
public:
	virtual Ast parse(TokenStream& head, TokenStream& body) = 0;
	virtual std::string NodeKind_toString(uint64_t n) const = 0;
	virtual ~IParser() noexcept {}
};

//responsible for delete of lexer
class PreambleDefinition {
public:
	std::string representation;
	ILexer* lexer = nullptr;
	IParser* parser = nullptr;
	~PreambleDefinition() noexcept {
		//if (lexer != nullptr)
		delete lexer;
		//if (parser != nullptr)
		delete parser;
	}
};