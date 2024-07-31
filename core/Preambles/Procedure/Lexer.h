#pragma once

#include "PreambleDefinition.h"

namespace Preamble {
	namespace Procedure {
		class Lexer : public ILexer {
			int64_t preambleIndex = -2;
			int64_t paramCount = 0;
		public:
			Lexer();
			//virtual void reset() override;
			virtual void setPreambleIndex(int64_t x) override;
			virtual std::optional<Token> lexHead(CodeLocation& loc) override;
			virtual std::optional<Token> lexBody(CodeLocation& loc) override;
			virtual std::string to_string(Token::Type kind) const override;
			virtual ~Lexer() override;
		};
	}
}