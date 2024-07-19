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
		virtual void setPreambuleIndex(int64_t x) = 0;
		virtual std::pair<std::optional<Token>, LexerMode> lexHead(CodeLocation& loc, uint8_t ch) = 0;
 		virtual std::pair<std::optional<Token>, LexerMode> lexBody(CodeLocation& loc, uint8_t ch) = 0;
		virtual std::string to_string(Token::Type kind) const = 0;
		virtual ~ILexer(){}
};
//responsible for delete of lexer
class PreambuleDefinition {
	public:
	std::string representation;
	ILexer* lexer = nullptr;
	~PreambuleDefinition(){
		if(lexer != nullptr)
			delete lexer;
	}
};