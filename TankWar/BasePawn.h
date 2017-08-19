#pragma once
#include "Scence.h"

/*
//用一个整数来标记一个Pawn的类型。
typedef unsigned int PawnType;

#define PAWN_TYPE_NONE 0
#define PAWN_TYPE_PLAYER 1
#define PAWN_TYPE_MECHANICAL_SPIDER 2
#define PAWN_TYPE_ARMORED_CAR 3
#define PAWN_TYPE_SCATTER 4
#define PAWN_TYPE_FROZEN_PLANE 5
#define PAWN_TYPE_AMMO 6

//类前导声明。
class BasePawn;

//一个简单的Pawn描述结构，子类的Pawn需要继承这个结构，
//创建自己的描述，这个描述将会用于创建和设置Pawn的初始属性。
struct PawnDesc 
{
	PawnType type;
};

//创建指定类型pawn的代理结构，
//这个结构用于PawnMaster中，
//能够自行创建Pawn对象。
class PawnCommandTemplate
{
public:
	//pDesc是一个动态分配的内存，CreatePawn需要自行释放这个内存。
	void virtual CreatePawn(PawnDesc* pDesc, Scence* pScence) = 0;
	//pPawn是一个动态分配的内存，在销毁对应的Pawn之后，DestoryPawn需要自行释放这个内存。
	void virtual DestoryPawn(BasePawn* pPawn, Scence* pScence) = 0;
};

//所有Pawn的父类。
class BasePawn
{
public:
	BasePawn();
	~BasePawn();
	PawnType GetType();

protected:
	//Pawn的类型。
	PawnType m_type = PAWN_TYPE_NONE;
	//Pawn的根控制器，一个Pawn的所有可控制Item都在这个链表上。
	ControlItem* p_mRootControl = nullptr;
};
*/
