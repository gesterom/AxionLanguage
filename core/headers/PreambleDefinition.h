#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>	

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

struct SyntaxDefinitionSteamtemnt {
	std::string constractionName;
	struct Type {
		enum {
			keyword,
			expresion,
			statement
		} kind;
		uint64_t requiredConstruction; // -1 means any
	};
	std::vector<Type> vec;
};

// `if` exp `:` stmt `else` stmt

struct SyntaxExpresion {
	struct Type {
		enum {
			keyword,
			expresion,
		} kind;
		std::string keyword_val_or_expresion_type;
	};
	std::vector<Type> vec;
	std::string constractionName;
};

using IndexInNodeArray = uint64_t;

struct SteatementNode {
	std::string nodeName; //nodeKind
	std::vector<IndexInNodeArray> children;
};

struct ExpresionNode { 
	std::string nodeName; //nodeKind
	std::vector<IndexInNodeArray> children;
};

class IParser {
	public:
	using TODODefineType = void;
	virtual TODODefineType parse(TokenStream& head,TokenStream& body) = 0;
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