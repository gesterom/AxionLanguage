#pragma once

#include "CodeLocation.h"
#include "MetaLexer.h"
#include "PreambleDefinition.h"
#include "Token.h"
#include <optional>

namespace Preamble {
	namespace Function {

		enum class FunctionTokenType : int64_t
		{
			parenthesis = Token::count + 1,
			comma,
			operator_t,
			if_keyword,
			then_keyword,
			else_keyword,
		};

		class Lexer : public ILexer {
			int32_t preambleIndex = -2;
		public:
			Lexer();
			//virtual void reset() override;
			virtual void setPreambleIndex(int32_t x) override;
			virtual std::optional<Token> lexHead(CodeLocation& loc) override;
			virtual std::optional<Token> lexBody(CodeLocation& loc) override;
			virtual std::string to_string(Token::Type kind) const override;
			virtual ~Lexer() override;
		};
	}
}
