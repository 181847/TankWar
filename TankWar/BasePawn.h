#pragma once
#include "Scence.h"


//��һ�����������һ��Pawn�����ͣ�
//PawnType�����PawnMaster�л�á�
typedef unsigned int PawnType;

//0��ʾ����Ϊ�ա�
#define PAWN_TYPE_NONE 0

//һ���򵥵�Pawn�����ṹ�������Pawn��Ҫ�̳�����ṹ��
//�����Լ�����������������������ڴ���������Pawn�ĳ�ʼ���ԡ�
struct PawnProperty 
{
};

//����Pawn�ĸ��ࡣ
class BasePawn
{
public:
	//ָ�����Pawn���õ���ControlItem��������
	BasePawn();
	~BasePawn();
	//��ȡPawn�����ͣ�����PawnType����ĳһ��Pawn�������ֱ������ⲿ��PawnMaster������������ԣ�
	//���������ȡ���͵�ʵ�ֱ��뽻�����࣬
	//�����BasePawn�����������̬���ԣ�����ʹ�����������Type����һ���ġ�
	//���Ա��������Լ�����һ����̬���ԣ���ʾType��
	static PawnType GetType();
};
