#pragma once
#include "CodeLocation.h"

struct Token {
	enum Type
	{
		error = -1,
		unknown,
		preambule,
		file_scope_atribute_name,
		file_scope_atribute_value,
		atribute_name,
		atribute_value,
		paranthis,
		comment,
	};
	using PreambuleType = int64_t;
	PreambuleType preambule_token = -1; // index in preambule array, when -1 then it is meta token
	Type kind;
	CodeLocation value;
};