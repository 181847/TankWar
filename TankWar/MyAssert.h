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
#define ASSERT(expr)//����ֵ
#endif

//�����������һЩ����û��ʵ�ֵ�ʱ���׳�����쳣����ֹ����һЩ�����ʵ��
#define THROW_UNIMPLEMENT_EXCEPTION(pChar)\
	{\
		throw SimpleException(pChar, __FILE__, __LINE__);\
	}
