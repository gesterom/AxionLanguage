#pragma once
#include "CodeLocation.h"


#ifdef _WIN32
#define createToken(x,y)  Token{ .kind = (x), .value = (y), .file = (__FILE__), .line = (__LINE__), .func = (__func__) }
#elif unix// __CYGWIN__
#define createToken(x,y)  Token{ .kind = (x), .value = (y), .file = (__FILE__), .line = (__LINE__), .func = (__PREETY_FUNCTION__) }
#else
#define createToken(x,y)  Token{ .kind = (x), .value = (y), .file = (__FILE__), .line = (__LINE__), .func = ("<dontSuport_function_names>") }
#endif

//#define createToken(x,y) Token{ (x),(y) }

struct Token {
	enum Type : int64_t
	{
		error = 0,
		unknown = 1,
		preamble = 2,
		string_literal = 3,
		file_scope_atribute_name = 4,
		file_scope_atribute_value = 5,
		atribute_name = 6,
		atribute_value = 7,
		parenthesis = 8,
		comment = 9,
		atom = 10,
		count = 0xFFFFFFFF
	};
	//using PreambleType = int32_t;
	//PreambleType preamble_token = -1; // index in preamble array, when -1 then it is meta token
	union{
		Type kind;
		struct {
			int32_t preamble_token;
			int32_t TypeIndex;
		};
	};
	//combine preamble_token and kind into one
	CodeLocation value;

	#ifdef createToken
		std::string file;
		int32_t line;
		std::string func;
	#endif // createToken

};

int32_t getPreamble(Token::Type k);
int32_t getTokenType(Token::Type k);
int64_t createTokenType(int32_t pre,int32_t k);



/*
#ifdef _WIN32
#define newErrorT(x,y,z) ErrorT{(__FILE__),(__LINE__),(__func__),(x),(y),(z)}
#elif unix// __CYGWIN__
#define newErrorT(x,y,z) ErrorT{(__FILE__),(__LINE__),(__PREETY_FUNCTION__),(x),(y),(z)}
#else
#define newErrorT(x,y,z) ErrorT{(__FILE__),(__LINE__),("<dontSuport_function_names>"),(x),(y),(z)}
#endif
*/



struct ErrorT {
	#ifdef newErrorT
	std::string fileName;
	int32_t line;
	std::string func_name;
	#endif
	CodeLocation loc;
	std::string oneLinerError;
	std::string msg;
};

