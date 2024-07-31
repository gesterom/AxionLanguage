#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "CodeLocation.h"
#include "MetaLexer.h"
#include "TODO.h"
#include "Token.h"

class ILexer {
public:
	//virtual void reset() = 0;
	virtual void setPreambleIndex(int64_t x) = 0;
	virtual std::optional<Token> lexHead(CodeLocation& loc) = 0;
	virtual std::optional<Token> lexBody(CodeLocation& loc) = 0;
	virtual std::string to_string(Token::Type kind) const = 0;
	virtual ~ILexer() noexcept {}
};

// operatotrs on steatements for examle if expresion statement else statement 
// 
// statement := `if` bool_expresion statement [`else` statement]
// statement := `while` bool_expresion statement
// statement := `for` iterator_expresion statement
// statement := `do` statement `while` bool_expresion `;`
// statement := expresion `;`
// statement := `{` [statement..] `}`
// statement := `twice` statement
// 
// {} = blok kodu = statement with list of statements
// expresion = literal,operation, atom.atom is a operation on atoms
// expresion := expresion infix_op expresion
// expresion := prefix_op expresion
// expresion := expresion sufix_op
// expresion := `ifx` bool_expresion `then` expresion `else` expresion
// expresion := `send` message_expresion `to` agent_expresion
// expresion := `goto` label_atom
// 

//type.implicit_cast TypeFrom -> TypeTo
/* 
{

}
*/


class IParser {
	public:
	using TODODefineType = void;
	virtual TODODefineType parse(const std::vector<Token>& head,const std::vector<Token>& body) = 0;
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