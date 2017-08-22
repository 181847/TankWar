#pragma once
#include "Scence.h"


//用一个整数来标记一个Pawn的类型，
//PawnType必须从PawnMaster中获得。
typedef unsigned int PawnType;
//0表示类型为空。
#define PAWN_TYPE_NONE 0

//类前导声明。
class BasePawn;

//一个简单的Pawn描述结构，子类的Pawn需要继承这个结构，
//创建自己的描述，这个描述将会用于创建和设置Pawn的初始属性。
struct PawnProperty 
{
};

//创建指定类型pawn的代理结构，
//这个结构用于PawnMaster中，
//能够自行创建Pawn对象。
class PawnCommandTemplate
{
public:
	//pDesc是一个动态分配的内存，CreatePawn需要自行释放这个内存。
	virtual BasePawn* CreatePawn(PawnProperty* pProperty, Scence* pScence) = 0;
	//pPawn是一个动态分配的内存，在销毁对应的Pawn之后，DestoryPawn需要自行释放这个内存。
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence) = 0;
};

//所有Pawn的父类。
class BasePawn
{
public:
	//指定这个Pawn中用到的ControlItem的数量。
	BasePawn();
	~BasePawn();
	//获取Pawn的类型，由于PawnType属于某一个Pawn，但是又必须由外部的PawnMaster来配置这个属性，
	//所以这个获取类型的实现必须交给子类，
	//如果在BasePawn中设置这个静态属性，将会使得子类的所有Type都是一样的。
	//所以必须子类自己声明一个静态属性，表示Type。
	static PawnType GetType();
	//注册Pawn到PawnMaster、PawnMaster能够实现自动生成Pawn对象，
	//为了达到这个目的，Pawn类需要生成一个相当于工厂类的对象指针，
	//然后传入PawnMaster中，并且获得一个PawnType。
	static void RegisterPawnMaster(PawnMaster * pPawnMaster);
};
