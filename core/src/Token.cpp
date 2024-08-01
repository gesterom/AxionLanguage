#include "Token.h"

int32_t getPreamble(Token::Type k)
{
	return 0xFFFFFFFF & (k>>32) ;
}

int32_t getTokenType(Token::Type k)
{
	return 0xFFFFFFFF & k;
}

int64_t createTokenType(int32_t pre, int32_t k)
{
	return ((int64_t)pre<<32) | ((int64_t)k);
}
