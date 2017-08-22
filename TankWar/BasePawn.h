#pragma once
#include "Scence.h"


//��һ�����������һ��Pawn�����ͣ�
//PawnType�����PawnMaster�л�á�
typedef unsigned int PawnType;
//0��ʾ����Ϊ�ա�
#define PAWN_TYPE_NONE 0

//��ǰ��������
class BasePawn;

//һ���򵥵�Pawn�����ṹ�������Pawn��Ҫ�̳�����ṹ��
//�����Լ�����������������������ڴ���������Pawn�ĳ�ʼ���ԡ�
struct PawnProperty 
{
};

//����ָ������pawn�Ĵ���ṹ��
//����ṹ����PawnMaster�У�
//�ܹ����д���Pawn����
class PawnCommandTemplate
{
public:
	//pDesc��һ����̬������ڴ棬CreatePawn��Ҫ�����ͷ�����ڴ档
	virtual BasePawn* CreatePawn(PawnProperty* pProperty, Scence* pScence) = 0;
	//pPawn��һ����̬������ڴ棬�����ٶ�Ӧ��Pawn֮��DestoryPawn��Ҫ�����ͷ�����ڴ档
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence) = 0;
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
	//ע��Pawn��PawnMaster��PawnMaster�ܹ�ʵ���Զ�����Pawn����
	//Ϊ�˴ﵽ���Ŀ�ģ�Pawn����Ҫ����һ���൱�ڹ�����Ķ���ָ�룬
	//Ȼ����PawnMaster�У����һ��һ��PawnType��
	static void RegisterPawnMaster(PawnMaster * pPawnMaster);
};
