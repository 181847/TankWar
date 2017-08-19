#pragma once
#include "Scence.h"


//��һ�����������һ��Pawn�����͡�
typedef unsigned int PawnType;

#define PAWN_TYPE_NONE 0
#define PAWN_TYPE_PLAYER 1
#define PAWN_TYPE_MECHANICAL_SPIDER 2
#define PAWN_TYPE_ARMORED_CAR 3
#define PAWN_TYPE_SCATTER 4
#define PAWN_TYPE_FROZEN_PLANE 5
#define PAWN_TYPE_AMMO 6

//��ȡPawn�ĸ���������
#define P_Root_Control(pBasePawn) (pBasePawn->m_pRootControl)

//��ǰ��������
class BasePawn;

//һ���򵥵�Pawn�����ṹ�������Pawn��Ҫ�̳�����ṹ��
//�����Լ�����������������������ڴ���������Pawn�ĳ�ʼ���ԡ�
struct PawnDesc 
{
	PawnType type;
};

//����ָ������pawn�Ĵ���ṹ��
//����ṹ����PawnMaster�У�
//�ܹ����д���Pawn����
class PawnCommandTemplate
{
public:
	//pDesc��һ����̬������ڴ棬CreatePawn��Ҫ�����ͷ�����ڴ档
	virtual BasePawn* CreatePawn(PawnDesc* pDesc, Scence* pScence) = 0;
	//pPawn��һ����̬������ڴ棬�����ٶ�Ӧ��Pawn֮��DestoryPawn��Ҫ�����ͷ�����ڴ档
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence) = 0;
};

//����Pawn�ĸ��ࡣ
class BasePawn
{
public:
	BasePawn();
	~BasePawn();
	//��ȡPawn�����͡�
	PawnType GetType();
	//����Pawn�����ɴ���ṹ������ṹ����PawnMaster�У�ʹ��PawnMaster�ܹ���������Pawn��
	virtual PawnCommandTemplate* GeneratePawnCommandTemplate() = 0;

protected:
	//Pawn�����͡�
	PawnType m_type = PAWN_TYPE_NONE;
	//Pawn�ĸ���������һ��Pawn�����пɿ���Item������������ϡ�
	ControlItem* m_pRootControl = nullptr;
};
