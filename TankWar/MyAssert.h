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
#define ASSERT(expr)//不求值
#endif

//这个宏适用于一些代码没有实现的时候，抛出这个异常，防止忘记一些代码的实现
#define THROW_UNIMPLEMENT_EXCEPTION(pChar)\
	{\
		throw SimpleException(pChar, __FILE__, __LINE__);\
	}
