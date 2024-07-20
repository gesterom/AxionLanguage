#pragma once
#include "CodeLocation.h"

struct Token {
	enum Type
	{
		error = -1,
		unknown,
		preamble,
		file_scope_atribute_name,
		file_scope_atribute_value,
		atribute_name,
		atribute_value,
		parenthesis,
		comment,
	};
	using PreambleType = int64_t;
	PreambleType preamble_token = -1; // index in preamble array, when -1 then it is meta token
	Type kind;
	CodeLocation value;
};