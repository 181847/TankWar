#pragma once
#include "Scence.h"


//用一个整数来标记一个Pawn的类型，
//PawnType必须从PawnMaster中获得。
typedef unsigned int PawnType;

//0表示类型为空。
#define PAWN_TYPE_NONE 0

//一个简单的Pawn描述结构，子类的Pawn需要继承这个结构，
//创建自己的描述，这个描述将会用于创建和设置Pawn的初始属性。
struct PawnProperty 
{
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
};
