#pragma once
#include <exception>
class  SimpleException: std::exception
{
public:
	const char* expr;
	const char* file;
	const int line;

public:
	SimpleException(const char *e, const char* f, const int line)
		:expr(e), file(f), line(line)
	{
	}

	inline std::string SimpleException::ToString()
	{
		char lineString[40];
		std::string returnval;
		_itoa_s(line, lineString, 10);
		returnval += "\nFile:\n";
		returnval += file;
		returnval += "\nLine:\n";
		returnval += lineString;
		returnval += "\nExpression:\n";
		return returnval;
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
