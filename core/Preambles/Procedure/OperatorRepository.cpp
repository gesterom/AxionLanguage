#include "Preambles/Procedure/OperatorRepository.h"

#include <algorithm>

using namespace Preamble::Procedure;

void Preamble::Procedure::OperatorRepository::insertIntoLexer(std::string representation)
{
	lexer.push_back(representation);
	std::sort(lexer.begin(), lexer.end(), [](const auto a, const auto b) {return a.size() > b.size(); });
}

/*```cpp
{"?", 1, suffix},      // Optional type: Type?
{"?", 1, prefix},      // Conditional operator (future use)
{"++", 2, prefix},     // Increment
{"--", 2, prefix},     // Decrement
{"++", 2, suffix},     // Increment
{"--", 2, suffix},     // Decrement

// Prefix Operators (highest precedence)
{"+", 3, prefix},      // Unary plus
{"-", 3, prefix},      // Unary minus
{"!", 3, prefix},      // Logical NOT
{"not", 3, prefix},    // Alternative for logical NOT
{"~", 3, prefix},      // Bitwise NOT (complement)
{"&", 3, prefix},      // Address-of
{"*", 3, prefix},      // Pointer dereference

// Arithmetic Operators
{"+", 4, infix},       // Addition
{"-", 4, infix},       // Subtraction

// Multiplicative Operators
{"*", 5, infix},       // Multiplication
{"/", 5, infix},       // Division
{"%", 5, infix},       // Modulus

// Comparison Operators / Membership Test
{"<", 6, infix},       // Less than
{"<=", 6, infix},      // Less than or equal
{">", 6, infix},       // Greater than
{">=", 6, infix},      // Greater than or equal
{"==", 6, infix},      // Equality
{"!=", 6, infix},      // Inequality
{"in", 6, infix},      // Membership test

// Bitwise Shift Operators
{"<<", 7, infix},      // Bitwise left shift
{">>", 7, infix},      // Bitwise right shift

// Bitwise Operators
{"&", 8, infix},       // Bitwise AND
{"^", 9, infix},       // Bitwise XOR
{"|", 10, infix},      // Bitwise OR

// Logical AND/OR
{"&&", 11, infix},     // Logical AND
{"and", 11, infix},    // Logical AND (alternative)
{"||", 12, infix},     // Logical OR
{"or", 12, infix},     // Logical OR (alternative)

// Assignment Operators (including custom with borrow checker)
{"=", 13, infix},      // Assignment
{"+=", 13, infix},     // Add and assign
{"-=", 13, infix},     // Subtract and assign
{"*=", 13, infix},     // Multiply and assign
{"/=", 13, infix},     // Divide and assign
{"%=", 13, infix},     // Modulus and assign
{"<<=", 13, infix},    // Left shift and assign
{">>=", 13, infix},    // Right shift and assign
{"&=", 13, infix},     // Bitwise AND and assign
{"|=", 13, infix},     // Bitwise OR and assign
{"^=", 13, infix},     // Bitwise XOR and assign
{"copy", 13, prefix},  // Copy (only with borrow checker)
{"move", 13, prefix},  // Move (only with borrow checker)

// Member Access / Type Casting / Result Type
{"->", 14, infix},     // Member access through pointer
{".", 14, infix},      // Member access
{"as", 14, infix},     // Type casting
{"?", 14, infix},      // Result type: TypeA?TypeErr
```*/

Preamble::Procedure::OperatorRepository::OperatorRepository()
{
	// Optional type: Type?
	this->addSuffix("?", 1, true, nullptr); // optional
	this->addInfix("?", 1, true, nullptr); // result type
	this->addInfix("->", 2, true, nullptr); // function type

	this->addInfix(".", 3, true, nullptr);

	// Increment and Decrement
	this->addPrefix("++", 4, true, nullptr);
	this->addPrefix("--", 4, true, nullptr);
	this->addSuffix("++", 4, false, nullptr);
	this->addSuffix("--", 4, false, nullptr);
	this->addInfix("as", 5, true, nullptr);  // Type casting

	// Prefix Operators (highest precedence)
	this->addPrefix("+", 6, false, nullptr);  // Unary plus
	this->addPrefix("-", 6, false, nullptr);  // Unary minus
	this->addPrefix("!", 6, false, nullptr);  // Logical NOT
	this->addPrefix("not", 6, false, nullptr); // Alternative for logical NOT
	this->addPrefix("~", 6, false, nullptr);  // Bitwise NOT (complement)
	this->addPrefix("bnot", 6, false, nullptr);  // Bitwise NOT (complement)
	this->addPrefix("&", 6, false, nullptr);  // Address-of
	this->addPrefix("*", 6, false, nullptr);  // Pointer dereference

	this->insertInfix("**", 7, false, nullptr);  // Power

	// Multiplicative Operators
	this->addInfix("*", 8, true, nullptr);  // Multiplication
	this->addInfix("/", 8, true, nullptr);  // Division
	this->addInfix("%", 8, true, nullptr);  // Modulus

	// Arithmetic Operators
	this->addInfix("+", 9, true, nullptr);  // Addition
	this->addInfix("-", 9, true, nullptr);  // Subtraction

	this->addInfix("..", 10, true, nullptr);  // exclusive Range
	this->addInfix("...", 10, true, nullptr);  // Inclusive Range // (4+3)..10

	this->addInfix("<<", 11, true, nullptr);  // Bitwise left shift
	this->addInfix(">>", 11, true, nullptr);  // Bitwise right shift

	this->addInfix("<=>", 12, true, nullptr);  // Three-way comparison operator

	// Comparison Operators / Membership Test
	this->addInfix("<", 13, true, nullptr);  // Less than
	this->addInfix("<=", 13, true, nullptr);  // Less than or equal
	this->addInfix(">", 13, true, nullptr);  // Greater than
	this->addInfix(">=", 13, true, nullptr);  // Greater than or equal

	this->addInfix("==", 14, true, nullptr);  // Equality
	this->addInfix("!=", 14, true, nullptr);  // Inequality
	this->addInfix("in", 14, true, nullptr);  // Membership test

	// Bitwise Shift Operators

	// Bitwise Operators
	this->addInfix("&", 15, true, nullptr);  // Bitwise AND
	this->addInfix("band", 15, true, nullptr);  // Bitwise AND
	this->addInfix("^", 16, true, nullptr);  // Bitwise XOR
	this->addInfix("bxor", 16, true, nullptr);  // Bitwise XOR
	this->addInfix("|", 17, true, nullptr);  // Bitwise OR
	this->addInfix("bor", 17, true, nullptr);  // Bitwise OR

	// Logical AND/OR
	this->addInfix("&&", 18, true, nullptr);  // Logical AND
	this->addInfix("and", 18, true, nullptr); // Logical AND (alternative)
	this->addInfix("xor", 19, true, nullptr);  // Logical XOR (alternative)
	this->addInfix("||", 20, true, nullptr);  // Logical OR
	this->addInfix("or", 20, true, nullptr);  // Logical OR (alternative)

	// Assignment Operators (including custom with borrow checker)

	this->addInfix("=", 21, false, nullptr);  // Assignment
	this->addInfix("+=", 21, false, nullptr);  // Add and assign
	this->addInfix("-=", 21, false, nullptr);  // Subtract and assign
	this->addInfix("*=", 21, false, nullptr);  // Multiply and assign
	this->addInfix("/=", 21, false, nullptr);  // Divide and assign
	this->addInfix("%=", 21, false, nullptr);  // Modulus and assign
	this->addInfix("<<=", 21, false, nullptr);  // Left shift and assign
	this->addInfix(">>=", 21, false, nullptr);  // Right shift and assign
	this->addInfix("&=", 21, false, nullptr);  // Bitwise AND and assign
	this->addInfix("|=", 21, false, nullptr);  // Bitwise OR and assign
	this->addInfix("^=", 21, false, nullptr);  // Bitwise XOR and assign

	// Member Access / Type Casting / Result Type
	//this->addInfix("->", 14, true, nullptr);  // Member access through pointer
	// 
	//this->insertPrefix("copy", this->getPrecedence("="), false, nullptr);  // Copy (only with borrow checker)
	//this->insertPrefix("move", this->getPrecedence("="), false, nullptr);  // Move (only with borrow checker)
}

void OperatorRepository::addPrefix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Prefix;
	op.codeGen = func;
	operators.emplace_back(op);
}
void OperatorRepository::insertPrefix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Prefix;
	op.codeGen = func;
	for (auto& item : operators) {
		if (item.precedence >= precedence)
			item.precedence++;
	}
	operators.emplace_back(op);
}
void OperatorRepository::addInfix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Infix;
	op.codeGen = func;
	operators.emplace_back(op);
}
void OperatorRepository::insertInfix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Infix;
	op.codeGen = func;
	for (auto& item : operators) {
		if (item.precedence >= precedence)
			item.precedence++;
	}
	operators.emplace_back(op);
}
void OperatorRepository::addSuffix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Suffix;
	op.codeGen = func;
	operators.emplace_back(op);
}
void OperatorRepository::insertSuffix(std::string representation, int32_t precedence, bool leftAssociativity, codeGenOperator func) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Suffix;
	op.codeGen = func;
	for (auto& item : operators) {
		if (item.precedence >= precedence)
			item.precedence++;
	}
	operators.emplace_back(op);
}
const std::vector<std::string>& OperatorRepository::getOperatorLexList() const {
	return this->lexer;
}
int32_t OperatorRepository::getPrecedencePrefix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Prefix) return i.precedence;
	}
	return -1;
}

int32_t OperatorRepository::getPrecedenceInfix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Infix) return i.precedence;
	}
	return -1;
}

int32_t OperatorRepository::getPrecedenceSuffix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Suffix) return i.precedence;
	}
	return -1;
}

bool OperatorRepository::isleftAssociativityPrefx(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Prefix) return i.leftAssociativity;
	}
	return false;
}

bool OperatorRepository::isleftAssociativityInfix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Infix) return i.leftAssociativity;
	}
	return false;
}

bool OperatorRepository::isleftAssociativitySuffix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Suffix) return i.leftAssociativity;
	}
	return false;
}

bool OperatorRepository::isPrefix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Prefix) return true;
	}
	return false;
}
bool OperatorRepository::isInfix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Infix) return true;
	}
	return false;
}
bool OperatorRepository::isSufix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Suffix) return true;
	}
	return false;
}
//codeGenOperator OperatorRepository::getCodeGen(std::string rep) const {
//	auto it = operators.find(rep);
//	if (it == operators.end()) return nullptr;
//	return it->second.codeGen;
//}

