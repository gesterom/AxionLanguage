#pragma once
#include <functional>
#include <optional>
#include <string>
#include <memory>

#include "Token.h"
#include "CodeLocation.h"
#include "MetaLexer.h"
#include "TODO.h"

class ILexer {
	public:
		virtual void reset() = 0;
		virtual void setPreambleIndex(int64_t x) = 0;
		virtual std::optional<Token> lexHead(CodeLocation& loc) = 0;
 		virtual std::optional<Token> lexBody(CodeLocation& loc) = 0;
		virtual std::string to_string(Token::Type kind) const = 0;
		virtual ~ILexer(){}
};
//responsible for delete of lexer
class PreambleDefinition {
	public:
	std::string representation;
	ILexer* lexer = nullptr;
	~PreambleDefinition(){
		if(lexer != nullptr)
			delete lexer;
	}
};