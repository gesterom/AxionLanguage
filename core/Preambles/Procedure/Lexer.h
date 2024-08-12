#pragma once

#include "PreambleDefinition.h"
#include "Preambles/Procedure/OperatorRepository.h"	

namespace Preamble {
	namespace Procedure {

		enum class ProcedureTokenType : int64_t { // 0x00 00 00 01 00 00 00 03
			keyword = Token::count + 1,
			comma,
			colon,
			semicolon,
			integer_literal,
			double_literal,
			bool_literal,
			operator_t,
			return_type_arrow,
		};

		class Lexer : public ILexer {
			int32_t preambleIndex = -2;
			int64_t paramCount = 0;
			OperatorRepository& repo;
		public:
			Lexer(OperatorRepository& repo);
			//virtual void reset() override;
			virtual void setPreambleIndex(int32_t x) override;
			virtual std::optional<Token> lexHead(CodeLocation& loc) override;
			virtual std::optional<Token> lexBody(CodeLocation& loc) override;
			virtual std::string to_string(Token::Type kind) const override;
			virtual ~Lexer() override;
		};
	}
}