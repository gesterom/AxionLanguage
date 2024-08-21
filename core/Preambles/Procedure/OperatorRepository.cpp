#include "Preambles/Procedure/OperatorRepository.h"

#include <algorithm>

void OperatorRepository::insertIntoLexer(std::string representation)
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

OperatorRepository::OperatorRepository()
{
	// Optional type: Type?

	int32_t iota = 0;

	this->addSuffix("?", ++iota, true); // optional
	this->addInfix("?", iota, true); // result type
	this->addInfix("->", ++iota, true); // function type

	this->addInfix(".", ++iota, true);

	// defineNewOperator prefix ++ insert . true ;
	// operator ++ for int
	// false not  
	// ptr* 
	// a++; ++a;
	// Increment and Decrement
	// 
	// ENUM COLORS{RED,YELLOW,BLUE}
	// let array : [int: COLORS] = [VAL_1,VAL_2,VAL_3,VAL_4];
	// let array : int[*:COLORS] // size 8 8 8 8 
	// let array : int[4] = [1,2,3,4]; // size 1 2 3 4
	// let array : int[*:COLORS] = [1,2,3,4];//indexed by COLORS // size 1 2 3 4
	// let array : int[dynamic:COLORS] = [1,2,3,4]; // size ptr
	// let array : vec(int,COLORS,context.HEAP_ALLOC) = [1,2,3,4]; // size cap ptr 
	// 
	// let array : int[COLORS] = [1,2,3,4];
	// let i : array.id = 3 as array.id;
	// array[i] == 1 
	// array[3] // error
	// 
	this->addSuffix("++", ++iota, true);
	this->addSuffix("--", iota, true);
	this->addPrefix("++", ++iota, false);
	this->addPrefix("--", iota, false);
	this->addInfix("as", ++iota, false);  // Type casting

	// Prefix Operators (highest precedence)
	//this->addPrefix("+", ++iota, false, nullptr);  // Unary plus
	this->addPrefix("-", iota, false);  // Unary minus
	this->addPrefix("!", iota, false);  // Logical NOT
	this->addPrefix("not", iota, false); // Alternative for logical NOT
	this->addPrefix("~", iota, false);  // Bitwise NOT (complement)
	this->addPrefix("bnot", iota, false);  // Bitwise NOT (complement)
	this->addPrefix("&", iota, false);  // Address-of
	this->addPrefix("*", iota, false);  // Pointer dereference

	this->insertInfix("**", ++iota, false);  // Power
	// APL - 
	// Multiplicative Operators
	this->addInfix("*", ++iota, true);  // Multiplication
	this->addInfix("/", iota, true);  // Division
	this->addInfix("%", iota, true);  // Modulus

	// Arithmetic Operators
	this->addInfix("+", ++iota, true);  // Addition
	this->addInfix("-", iota, true);  // Subtraction

	this->addInfix("..", ++iota, true);  // exclusive Range
	this->addInfix("...", iota, true);  // Inclusive Range // (4+3)..10

	// Bitwise Shift Operators
	this->addInfix("<<", ++iota, true);  // Bitwise left shift
	this->addInfix(">>", iota, true);  // Bitwise right shift

	// Bitwise Operators
	this->addInfix("&", ++iota, true);  // Bitwise AND
	this->addInfix("band", iota, true);  // Bitwise AND
	this->addInfix("^", ++iota, true);  // Bitwise XOR
	this->addInfix("bxor", iota, true);  // Bitwise XOR
	this->addInfix("|", ++iota, true);  // Bitwise OR
	this->addInfix("bor", iota, true);  // Bitwise OR

	// Logical AND/OR
	this->addInfix("&&", ++iota, true);  // Logical AND
	this->addInfix("and", iota, true); // Logical AND (alternative)
	this->addInfix("xor", ++iota, true);  // Logical XOR (alternative)
	this->addInfix("||", ++iota, true);  // Logical OR
	this->addInfix("or", iota, true);  // Logical OR (alternative)

	this->addInfix("<=>", ++iota, true);  // Three-way comparison operator
	// Comparison Operators / Membership Test
	this->addInfix("<", ++iota, true);  // Less than
	this->addInfix("<=", iota, true);  // Less than or equal
	this->addInfix(">", iota, true);  // Greater than
	this->addInfix(">=", iota, true);  // Greater than or equal

	this->addInfix("==", ++iota, true);  // Equality
	this->addInfix("!=", iota, true);  // Inequality
	this->addInfix("in", iota, true);  // Membership test

	// Assignment Operators (including custom with borrow checker)
	this->addInfix("=", ++iota, false);  // Assignment
	this->addInfix("+=", iota, false);  // Add and assign
	this->addInfix("-=", iota, false);  // Subtract and assign
	this->addInfix("*=", iota, false);  // Multiply and assign
	this->addInfix("/=", iota, false);  // Divide and assign
	this->addInfix("%=", iota, false);  // Modulus and assign
	this->addInfix("<<=", iota, false);  // Left shift and assign
	this->addInfix(">>=", iota, false);  // Right shift and assign
	this->addInfix("&=", iota, false);  // Bitwise AND and assign
	this->addInfix("|=", iota, false);  // Bitwise OR and assign
	this->addInfix("^=", iota, false);  // Bitwise XOR and assign

	// Member Access / Type Casting / Result Type
	//this->addInfix("->", 14, true, nullptr);  // Member access through pointer
	// 
	//this->insertPrefix("copy", this->getPrecedence("="), false, nullptr);  // Copy (only with steel* checker)
	//this->insertPrefix("move", this->getPrecedence("="), false, nullptr);  // Move (only with steel* checker)
	// weasel treesiter main.w -> weaselParser.c -> weaselParser.so 
}

void OperatorRepository::addPrefix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Prefix;
	operators.emplace_back(op);
}
void OperatorRepository::insertPrefix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Prefix;
	for (auto& item : operators) {
		if (item.precedence >= precedence)
			item.precedence++;
	}
	operators.emplace_back(op);
}
void OperatorRepository::addInfix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Infix;
	operators.emplace_back(op);
}
void OperatorRepository::insertInfix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Infix;
	for (auto& item : operators) {
		if (item.precedence >= precedence)
			item.precedence++;
	}
	operators.emplace_back(op);
}
void OperatorRepository::addSuffix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Suffix;
	operators.emplace_back(op);
}
void OperatorRepository::insertSuffix(std::string representation, OperatorPrecedence precedence, bool leftAssociativity) {
	if (precedence <= 0) precedence = 1;
	insertIntoLexer(representation);
	OperatorDefinition op;
	op.representation = representation;
	op.precedence = precedence;
	op.leftAssociativity = leftAssociativity;
	op.type = OperatorDefinition::Suffix;
	for (auto& item : operators) {
		if (item.precedence >= precedence)
			item.precedence++;
	}
	operators.emplace_back(op);
}
const std::vector<std::string>& OperatorRepository::getOperatorLexList() const {
	return this->lexer;
}
OperatorPrecedence OperatorRepository::getPrecedencePrefix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Prefix) return i.precedence;
	}
	return -1;
}

OperatorPrecedence OperatorRepository::getPrecedenceInfix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Infix) return i.precedence;
	}
	return -1;
}

OperatorPrecedence OperatorRepository::getPrecedenceSuffix(std::string rep) const {
	for (const auto& i : operators) {
		if (i.representation == rep and i.type == OperatorDefinition::Suffix) return i.precedence;
	}
	return -1;
}

bool OperatorRepository::isleftAssociativityPrefix(std::string rep) const {
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
bool OperatorRepository::isSuffix(std::string rep) const {
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

