#pragma once

#include <string>
#include <vector>

enum class OperandType {
	Operand_null, Operand_atom, Operand_ptr, Operand_bool, Operand_string, Operand_int, Operand_float, Operand_int_sized
};

struct OperatorDefinition {
	enum Type {
		prefix, infix, postfix
	};
	std::vector<std::string> representation;
	uint8_t presedence;
	bool left_acocieted;
	Type type;
	OperandType LeftType = OperandType::Operand_null;
	OperandType RightType = OperandType::Operand_null;
	OperandType ResultType = OperandType::Operand_null;
	//TODO add what types are implemented
	// type A,type B => Type C
};

/* Type modifiers/operators
	Type[] = dynamic array of T (basicly pointer + size)
	Type[com_time_size] = static size array of Ts
	Type[enum_type] = static size array of Ts indexed by enum
	Type[range_type] = static size array of Ts indexed by range type
	Type[distinct] = static size array of Ts indexed by uniq index_type => create array and type.distinct type_array_name = size_t;
	Type[*] = [1,2,3,4] = deduce size from init (in this case 4)
	(Args)->Return = function
	Type& = reference
	Type* = pointer
	Type? = optional<Type>
	Type?Error = Result<Type,Error>
	TypeA|TypeB|TypeC = typed union
	TypeA,TypeB,TypeC = tuple => Int,String->Int; (int,int),string != int,int,string
	const Type = constant varible
	(const Type)[] = array of consts
	const Type? = const optional<T>
	(const Type)? = optional<const T>
	mut Type = mutable varible
*/

/* build in Types*//*
	int
	int8
	int16
	int32
	int64
	uint8
	uint16
	uint32
	uint64
	bool
	string
	float
	ptr,
	id/atom
*/

/* build in literals *//*
	null
	false
	true
*/

std::vector<OperatorDefinition> operators = {
	OperatorDefinition{ {"."}, 1, true, OperatorDefinition::infix,OperandType::Operand_atom,OperandType::Operand_atom,OperandType::Operand_atom}, // Scope resolution and member access
	// copy person.name ++
	OperatorDefinition{ {"--"}, 2, true, OperatorDefinition::postfix,OperandType::Operand_int,OperandType::Operand_null,OperandType::Operand_null}, // Decrement
	OperatorDefinition{ {"++"}, 2, true, OperatorDefinition::postfix,OperandType::Operand_int,OperandType::Operand_null,OperandType::Operand_null}, // Increment


	OperatorDefinition{ {"++"}, 3, true,OperatorDefinition::prefix,OperandType::Operand_null,OperandType::Operand_int , OperandType::Operand_null}, // Increment
	OperatorDefinition{ {"--"}, 3, true, OperatorDefinition::prefix,OperandType::Operand_null,OperandType::Operand_int , OperandType::Operand_null},// Decrement

	OperatorDefinition{{"+"}, 3, true,OperatorDefinition::prefix, OperandType::Operand_null,OperandType::Operand_int, OperandType::Operand_int}, // addition
	OperatorDefinition{{"+"}, 3, true,OperatorDefinition::prefix, OperandType::Operand_null,OperandType::Operand_float , OperandType::Operand_float}, // addition
	OperatorDefinition{{"-"}, 3, true, OperatorDefinition::prefix,OperandType::Operand_null,OperandType::Operand_int, OperandType::Operand_int}, // sub
	OperatorDefinition{{"-"}, 3, true, OperatorDefinition::prefix,OperandType::Operand_null,OperandType::Operand_float , OperandType::Operand_float}, // sub
	OperatorDefinition{{"not","!"}, 3, false,OperatorDefinition::prefix,OperandType::Operand_null,OperandType::Operand_bool,OperandType::Operand_bool}, // Logical NOT (Python)
	OperatorDefinition{{"~","bnot"}, 3, false,   OperatorDefinition::prefix,OperandType::Operand_null,OperandType::Operand_int_sized,OperandType::Operand_int_sized}, // Bitwise  NOT
	//OperatorDefinition{{"as"}, 3, true,OperatorDefinition::infix}, // cast // TODO will be implemented be type preamble

	//OperatorDefinition{{".."}, 4, true,OperatorDefinition::infix}, // Range (inclusive) (Ruby) // TODO will be implemented be type preamble
	//OperatorDefinition{{"..."}, 4, true,OperatorDefinition::infix}, // Range (exclusive) (Ruby) // TODO will be implemented be type preamble

	OperatorDefinition{{"**"}, 5, false,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int}, // Exponentiation (Python)
	OperatorDefinition{{"**"}, 5, false,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float}, // Exponentiation (Python)

	OperatorDefinition{{"*"}, 6, true,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int}, // Multiplication
	OperatorDefinition{{"*"}, 6, true,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float}, // Multiplication
	OperatorDefinition{{"/","div"}, 6, true,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int}, // Division
	OperatorDefinition{{"/","div"}, 6, true,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float}, // Division
	OperatorDefinition{{"%","mod"}, 6, true,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int}, // Modulus

	OperatorDefinition{{"+"}, 7, true,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int}, // add
	OperatorDefinition{{"+"}, 7, true,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float}, // add
	//OperatorDefinition{{"+"}, 7, true,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string}, // concatination
	OperatorDefinition{{"-"}, 7, true, OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int},// sub
	OperatorDefinition{{"-"}, 7, true, OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float},// sub

	OperatorDefinition{{"<<","shl"}, 8, true,OperatorDefinition::infix,OperandType::Operand_int_sized,OperandType::Operand_int,OperandType::Operand_int_sized}, // Left shift
	OperatorDefinition{{">>","shr"}, 8, true,OperatorDefinition::infix,OperandType::Operand_int_sized,OperandType::Operand_int,OperandType::Operand_int_sized}, // Right shift
	OperatorDefinition{{">>>"}, 8, true, OperatorDefinition::infix,OperandType::Operand_int_sized,OperandType::Operand_int,OperandType::Operand_int_sized}, // Zero-fill right shift (JavaScript)

	OperatorDefinition{{"<=>"}, 9, true, OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int}, // Spaceship operator (PHP, Ruby)
	OperatorDefinition{{"<=>"}, 9, true, OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_int}, // Spaceship operator (PHP, Ruby)
	OperatorDefinition{{"<=>"}, 9, true, OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_int}, // Spaceship operator (PHP, Ruby)

	//OperatorDefinition{{"is"}, 10, true,OperatorDefinition::infix}, // Identity (Python) // TODO will be implemented be "type" preamble
	OperatorDefinition{{"<"}, 10, true,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_bool}, // Less than
	OperatorDefinition{{"<"}, 10, true,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_bool}, // Less than
	OperatorDefinition{{"<"}, 10, true,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_bool}, // Less than
	OperatorDefinition{{">"}, 10, true,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_bool}, // Greater than
	OperatorDefinition{{">"}, 10, true,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_bool}, // Greater than
	OperatorDefinition{{">"}, 10, true,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_bool}, // Greater than
	OperatorDefinition{{"<="}, 10, true,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_bool},  // Less than or equal to
	OperatorDefinition{{"<="}, 10, true,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_bool},  // Less than or equal to
	OperatorDefinition{{"<="}, 10, true,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_bool},  // Less than or equal to
	OperatorDefinition{{">="}, 10, true,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_bool},  // Greater than or equal to
	OperatorDefinition{{">="}, 10, true,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_bool},  // Greater than or equal to
	OperatorDefinition{{">="}, 10, true,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_bool},  // Greater than or equal to
	//OperatorDefinition{{"in"}, 10, true,OperatorDefinition::infix}, // Membership (Python) // TODO will be implemented be "type" preamble

	OperatorDefinition{{"=="}, 11, true,OperatorDefinition::infix,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool}, // Equality
	OperatorDefinition{{"=="}, 11, true,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_bool}, // Equality
	OperatorDefinition{{"=="}, 11, true,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_bool}, // Equality
	OperatorDefinition{{"=="}, 11, true,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_bool}, // Equality
	OperatorDefinition{{"=="}, 11, true,OperatorDefinition::infix,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_bool}, // Equality

	OperatorDefinition{{"!="}, 11, true,OperatorDefinition::infix,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool}, // Inequality
	OperatorDefinition{{"!="}, 11, true,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_bool}, // Inequality
	OperatorDefinition{{"!="}, 11, true,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_bool},// Inequality
	OperatorDefinition{{"!="}, 11, true,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_bool},// Inequality
	OperatorDefinition{{"!="}, 11, true,OperatorDefinition::infix,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_bool}, // Inequality

	OperatorDefinition{{"&","band"}, 12, true,OperatorDefinition::infix,OperandType::Operand_int_sized,OperandType::Operand_int_sized,OperandType::Operand_int_sized}, // Bitwise AND

	OperatorDefinition{{"^","bxor"}, 13, true,OperatorDefinition::infix,OperandType::Operand_int_sized,OperandType::Operand_int_sized,OperandType::Operand_int_sized}, // Bitwise XOR

	OperatorDefinition{{"|","bor"}, 14, true,OperatorDefinition::infix,OperandType::Operand_int_sized,OperandType::Operand_int_sized,OperandType::Operand_int_sized}, // Bitwise OR

	OperatorDefinition{{"||","or"}, 15, true,OperatorDefinition::infix,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool}, // Logical OR

	OperatorDefinition{{"&&","and"}, 16, true,OperatorDefinition::infix,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool}, // Logical AND

	//OperatorDefinition{{"??"}, 17, false,OperatorDefinition::infix}, //  ?? the null-coalescing operators // TODO implemented by "type" preamble

	//OperatorDefinition{{"copy"}, 18, false,OperatorDefinition::prefix}, // copy opperator //TODO implemented by steal checker
	//OperatorDefinition{{"move"}, 18, false,OperatorDefinition::prefix}, //  move operator //TODO implemented by steal checker

	OperatorDefinition{{"="}, 19, false,OperatorDefinition::infix,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool}, // Assignment
	OperatorDefinition{{"="}, 19, false,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int}, // Assignment
	OperatorDefinition{{"="}, 19, false,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float}, // Assignment
	OperatorDefinition{{"="}, 19, false,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string}, // Assignment
	OperatorDefinition{{"="}, 19, false,OperatorDefinition::infix,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr}, // Assignment

	OperatorDefinition{{"+="}, 19, false,OperatorDefinition::infix,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool},  // Addition assignment
	OperatorDefinition{{"+="}, 19, false,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int},  // Addition assignment
	OperatorDefinition{{"+="}, 19, false,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float},  // Addition assignment
	OperatorDefinition{{"+="}, 19, false,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string},  // Addition assignment
	OperatorDefinition{{"+="}, 19, false,OperatorDefinition::infix,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr},  // Addition assignment

	OperatorDefinition{{"-="}, 19, false,OperatorDefinition::infix,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool},  // Subtraction assignment
	OperatorDefinition{{"-="}, 19, false,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int},  // Subtraction assignment
	OperatorDefinition{{"-="}, 19, false,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float},  // Subtraction assignment
	OperatorDefinition{{"-="}, 19, false,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string},  // Subtraction assignment
	OperatorDefinition{{"-="}, 19, false,OperatorDefinition::infix,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr},  // Subtraction assignment

	OperatorDefinition{{"*="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool },  // Multiplication assignment
	OperatorDefinition{ {"*="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int },  // Multiplication assignment
	OperatorDefinition{ {"*="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float },  // Multiplication assignment
	OperatorDefinition{ {"*="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string },  // Multiplication assignment
	OperatorDefinition{ {"*="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr },  // Multiplication assignment

	OperatorDefinition{ {"/="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool },  // Division assignment
	OperatorDefinition{ {"/="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int },  // Division assignment
	OperatorDefinition{ {"/="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float },  // Division assignment
	OperatorDefinition{ {"/="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string },  // Division assignment
	OperatorDefinition{ {"/="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr },  // Division assignment

	OperatorDefinition{ {"%="}, 19, false,OperatorDefinition::infix,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool },  // Modulus assignment
	OperatorDefinition{ {"%="}, 19, false,OperatorDefinition::infix,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int },  // Modulus assignment
	OperatorDefinition{ {"%="}, 19, false,OperatorDefinition::infix,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float },  // Modulus assignment
	OperatorDefinition{ {"%="}, 19, false,OperatorDefinition::infix,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string },  // Modulus assignment
	OperatorDefinition{ {"%="}, 19, false,OperatorDefinition::infix,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr },  // Modulus assignment

	OperatorDefinition{{"<<="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool },  // Left shift assignment
	OperatorDefinition{ {"<<="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int },  // Left shift assignment
	OperatorDefinition{ {"<<="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float },  // Left shift assignment
	OperatorDefinition{ {"<<="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string },  // Left shift assignment
	OperatorDefinition{ {"<<="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr },  // Left shift assignment

	OperatorDefinition{{">>="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool },  // Right shift assignment
	OperatorDefinition{ {">>="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int },  // Right shift assignment
	OperatorDefinition{ {">>="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float },  // Right shift assignment
	OperatorDefinition{ {">>="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string },  // Right shift assignment
	OperatorDefinition{ {">>="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr },  // Right shift assignment
	//OperatorDefinition{{"??="}, 19, false,OperatorDefinition::infix}, // ??= operators - the null-coalescing operators // TODO implemented by "type" preamble
	OperatorDefinition{{"&="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool },  // Bitwise AND assignment
	OperatorDefinition{ {"&="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int },  // Bitwise AND assignment
	OperatorDefinition{ {"&="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float },  // Bitwise AND assignment
	OperatorDefinition{ {"&="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string },  // Bitwise AND assignment
	OperatorDefinition{ {"&="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr },  // Bitwise AND assignment

	OperatorDefinition{{"|="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool },  // Bitwise OR assignment
	OperatorDefinition{ {"|="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int },  // Bitwise OR assignment
	OperatorDefinition{ {"|="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float },  // Bitwise OR assignment
	OperatorDefinition{ {"|="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string },  // Bitwise OR assignment
	OperatorDefinition{ {"|="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr },  // Bitwise OR assignment

	OperatorDefinition{{"^="}, 19, false,OperatorDefinition::infix  ,OperandType::Operand_bool,OperandType::Operand_bool,OperandType::Operand_bool },  // Bitwise XOR assignment
	OperatorDefinition{ {"^="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_int,OperandType::Operand_int,OperandType::Operand_int },  // Bitwise XOR assignment
	OperatorDefinition{ {"^="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_float,OperandType::Operand_float,OperandType::Operand_float },  // Bitwise XOR assignment
	OperatorDefinition{ {"^="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_string,OperandType::Operand_string,OperandType::Operand_string },  // Bitwise XOR assignment
	OperatorDefinition{ {"^="}, 19, false,OperatorDefinition::infix ,OperandType::Operand_ptr,OperandType::Operand_ptr,OperandType::Operand_ptr },  // Bitwise XOR assignment
};