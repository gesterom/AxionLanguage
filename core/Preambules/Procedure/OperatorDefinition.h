#pragma once

#include <vector>
#include <string>

struct OperatorDefinition {
	std::vector<std::string> representation;
	uint8_t presedence;
	bool left_acocieted;
};

std::vector<OperatorDefinition> operators = {
	OperatorDefinition{{"||","or"}, 1, true}, // Logical OR
	OperatorDefinition{{"&&","and"}, 2, true}, // Logical AND
	OperatorDefinition{{"|","bor"}, 3, true}, // Bitwise OR
	OperatorDefinition{{"^","bxor"}, 4, true}, // Bitwise XOR
	OperatorDefinition{{"&","band"}, 5, true}, // Bitwise AND
	OperatorDefinition{{"=="}, 6, true}, // Equality
	OperatorDefinition{{"!="}, 6, true}, // Inequality
	OperatorDefinition{{"is"}, 6, true}, // Identity (Python)
	OperatorDefinition{{"is not"}, 6, true}, // Identity (Python)
	OperatorDefinition{{"<"}, 7, true}, // Less than
	OperatorDefinition{{">"}, 7, true}, // Greater than
	OperatorDefinition{{"<="}, 7, true}, // Less than or equal to
	OperatorDefinition{{">="}, 7, true}, // Greater than or equal to
	OperatorDefinition{{"in"}, 7, true}, // Membership (Python)
	OperatorDefinition{{"not in"}, 7, true}, // Membership (Python)
	OperatorDefinition{{"<=>"}, 7, true}, // Spaceship operator (PHP, Ruby)
	OperatorDefinition{{"<<","shl"}, 8, true}, // Left shift
	OperatorDefinition{{">>","shr"}, 8, true}, // Right shift
	OperatorDefinition{{">>>"}, 8, true}, // Zero-fill right shift (JavaScript)
	OperatorDefinition{{"+"}, 9, true}, // Addition
	OperatorDefinition{{"-"}, 9, true}, // Subtraction
	OperatorDefinition{{"*"}, 10, true}, // Multiplication
	OperatorDefinition{{"/"}, 10, true}, // Division
	OperatorDefinition{{"%","mod"}, 10, true}, // Modulus
	OperatorDefinition{{"**"}, 11, false}, // Exponentiation (Python)
	OperatorDefinition{{"++"}, 12, true}, // Increment
	OperatorDefinition{{"--"}, 12, true}, // Decrement
	OperatorDefinition{{".."}, 13, true}, // Range (inclusive) (Ruby)
	OperatorDefinition{{"..."}, 13, true}, // Range (exclusive) (Ruby)
	OperatorDefinition{{"not","!"}, 14, false}, // Logical NOT (Python)
	OperatorDefinition{{"~","bnot"}, 15, false}, // Bitwise NOT
	OperatorDefinition{{"="}, 17, false}, // Assignment
	OperatorDefinition{{"+="}, 17, false}, // Addition assignment
	OperatorDefinition{{"-="}, 17, false}, // Subtraction assignment
	OperatorDefinition{{"*="}, 17, false}, // Multiplication assignment
	OperatorDefinition{{"/="}, 17, false}, // Division assignment
	OperatorDefinition{{"%="}, 17, false}, // Modulus assignment
	OperatorDefinition{{"<<="}, 17, false}, // Left shift assignment
	OperatorDefinition{{">>="}, 17, false}, // Right shift assignment
	OperatorDefinition{{"&="}, 17, false}, // Bitwise AND assignment
	OperatorDefinition{{"|="}, 17, false}, // Bitwise OR assignment
	OperatorDefinition{{"^="}, 17, false}, // Bitwise XOR assignment
};