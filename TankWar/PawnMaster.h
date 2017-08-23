#pragma once
#include "BasePawn.h"

typedef BYTE PawnCommandType;
#define PAWN_COMMAND_TYPE_NONE 0x00
#define PAWN_COMMAND_TYPE_CREATE 0x0f
#define PAWN_COMMAND_TYPE_DESTORY 0xff

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
	void DestroyPawn(BasePawn* pThePawn);
	//执行指令，即生成Pawn。
	void Executee();

protected:
	//Pawn保存的场景对象，这个场景对象将会提供给Pawn类型具体的去获取网格物体。
	Scence* m_pScence;
	//未执行的命令数量。
	UINT UnExecuteeCommandNum = 0;
	//存储指令缓存，在构造函数中固定一个大小。
	std::vector<PawnCommand> CommandBuffer;
	//命令模板，可以随时添加。
	std::vector<std::unique_ptr<PawnCommandTemplate>> CommandTemplateList;
};

