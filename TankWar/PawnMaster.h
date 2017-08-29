#pragma once
#include "BasePawn.h"

typedef BYTE PawnCommandType;
#define PAWN_COMMAND_TYPE_NONE 0x00
#define PAWN_COMMAND_TYPE_CREATE 0x0f
#define PAWN_COMMAND_TYPE_DESTORY 0xff

//这个结构只用来在PawnCommandTemplate存储一个Pawn指针。
struct PawnUnit
{
	BasePawn* pSavedPawn;
};

//创建指定类型pawn的代理结构，
//这个结构用于PawnMaster中，
//能够自行创建Pawn对象。
class PawnCommandTemplate
{
public:
	//pDesc是一个动态分配的内存，CreatePawn需要自行释放这个内存。
	virtual BasePawn* CreatePawn(PawnUnit * saveUnit, PawnProperty* pProperty, Scence* pScence) = 0;
	//pPawn是一个动态分配的内存，在销毁对应的Pawn之后，DestoryPawn需要自行释放这个内存。
	//返回这个Pawn对应的存储单位。
	virtual PawnUnit* DestoryPawn(BasePawn* pPawn, Scence* pScence) = 0;

public:
	//存储所有从这个commandTemplate生成的pawn对象。
	LinkedAllocator<PawnUnit> Manager;
};

struct PawnCommand 
{
	PawnCommandType CommandType;

	union {
		struct
		{
			//要生成的Pawn类型，这个类型只能是在这个pawnMaster中注册过的类型。
			//注意，这个值对应于CommandTemplateList中数组的元素，
			//但是不能为0，所以获取元素的时候要注意减一来取到数组中对应的值。
			PawnType pawnType;
			//Pawn的属性设定指针，这个指针指向的内存由具体的Pawn类型分配和回收。
			PawnProperty* pProperty;
		} CreateCommand;

		struct  
		{
			PawnType pawnType;
			//要删除的Pawn对象的指针。
			BasePawn* pPawn_to_destory;
		} DestoryCommand;
	};
};

//这个类型对象专门用来自动化控制Pawn的生产。
class PawnMaster
{
public:
	PawnMaster(UINT MaxCommandNum, Scence* pScence);
	~PawnMaster();

	//注册一个Pawn的生成命令模板，返回为这个Pawn类型分配的PawnType。
	PawnType AddCommandTemplate(std::unique_ptr<PawnCommandTemplate> pCommandTemplate);
	//记录一个生成指定类型Pawn的指令。
	void CreatePawn(PawnType pawnType, PawnProperty* pProperty);
	//记录一个删除Pawn的指令。
	void DestroyPawn(PawnType pawnType, BasePawn* pThePawn);
	//执行指令，即生成Pawn。
	void Executee();

protected:
	//两个辅助方法
	void CreatingPawn(UINT commandIndex);
	void DestoringPawn(UINT commandIndex);

protected:
	//Pawn保存的场景对象，这个场景对象将会提供给Pawn类型具体的去获取网格物体。
	Scence* m_pScence;
	//未执行的命令数量。
	UINT UnExecuteeCommandNum = 0;
	//存储指令缓存，在构造函数中固定一个大小。
	std::vector<PawnCommand> CommandBuffer;

public:
	//命令模板，可以随时添加。
	std::vector<std::unique_ptr<PawnCommandTemplate>> CommandTemplateList;
};

