#pragma once
#include <exception>
class  SimpleException: std::exception
{
public:
	char* expr;
	char* file;
	int line;

public:
	SimpleException(char *e, char* f, int line)
	{
		expr = e;
		file = f;
		this->line = line;
	}
};


#if ASSERTIONS_ENABLED
#define ASSERT(expr) \
	if(expr){}\
	else\
	{\
		throw SimpleException( #expr, __FILE__, __LINE__);\
	}
#else
#define ASSERT(expr)//≤ª«Û÷µ
#endif


