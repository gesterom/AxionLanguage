#pragma once

#include <string>
#include <vector>


using OperatorPrecedence = int32_t;
class OperatorRepository
{
	struct OperatorDefinition {
		std::string representation = "<ERROR>";
		OperatorPrecedence precedence = 0;
		bool leftAssociativity = false;
		enum Type
		{
			error, Prefix, Infix, Suffix
		};
		Type type = error;
	};
	std::vector<std::string> lexer;
	std::vector<OperatorDefinition> operators;

	void insertIntoLexer(std::string);

public:
	OperatorRepository();
	void addPrefix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity);
	void insertPrefix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity);
	void addInfix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity);
	void insertInfix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity);
	void addSuffix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity);
	void insertSuffix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity);
	const std::vector<std::string>& getOperatorLexList() const;
	OperatorPrecedence getPrecedencePrefix(std::string) const;
	OperatorPrecedence getPrecedenceInfix(std::string) const;
	OperatorPrecedence getPrecedenceSuffix(std::string) const;
	bool isleftAssociativityPrefix(std::string) const;
	bool isleftAssociativityInfix(std::string) const;
	bool isleftAssociativitySuffix(std::string) const;
	bool isPrefix(std::string) const;
	bool isInfix(std::string) const;
	bool isSuffix(std::string) const;
	//codeGenOperator getCodeGen(std::string) const;
};
