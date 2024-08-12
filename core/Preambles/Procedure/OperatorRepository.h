#pragma once

#include <string>
#include <vector>

#include <AST.h>

typedef std::string(*codeGenOperator)(Ast& ast, Ast::NodeIndex left, Ast::NodeIndex right);

namespace Preamble {
	namespace Procedure {
		class OperatorRepository
		{
			struct OperatorDefinition {
				std::string representation = "<ERROR>";
				int32_t precedence = 0;
				bool leftAssociativity = false;
				enum Type
				{
					error, Prefix, Infix, Suffix
				};
				Type type = error;
				codeGenOperator codeGen = nullptr;
			};
			std::vector<std::string> lexer;
			std::vector<OperatorDefinition> operators;

			void insertIntoLexer(std::string);

		public:
			OperatorRepository();
			void addPrefix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func);
			void insertPrefix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func);
			void addInfix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func);
			void insertInfix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func);
			void addSuffix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func);
			void insertSuffix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func);
			const std::vector<std::string>& getOperatorLexList() const;
			int32_t getPrecedencePrefix(std::string) const;
			int32_t getPrecedenceInfix(std::string) const;
			int32_t getPrecedenceSuffix(std::string) const;
			bool isleftAssociativityPrefx(std::string) const;
			bool isleftAssociativityInfix(std::string) const;
			bool isleftAssociativitySuffix(std::string) const;
			bool isPrefix(std::string) const;
			bool isInfix(std::string) const;
			bool isSufix(std::string) const;
			//codeGenOperator getCodeGen(std::string) const;
		};
	}
}
