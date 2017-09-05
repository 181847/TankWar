#pragma once
#include "ShellProperty.h"
#include "MyStaticAllocator.h"
#include "GCommanders.h"

//最大的弹药数量。
#define MAX_SHELL_PAWN_NUM 90000

//炮弹单位的可控制网格数量，只有一个。
#define CONTROLITEM_NUM_SHELL 1
//根控制器序号。
#define CONTROLITEM_INDEX_SHELL 0

//根骨骼数组序号
#define BONE_INDEX_SHELL_ROOT 0

//移动状态标志。
#define STORY_FRAGMENT_SHELL_MOVE 1
//跟随状态。
#define STORY_FRAGMENT_SHELL_FOLLOW 2

enum ShellState :unsigned int
{
	//自我移动状态。
	Move,
	//跟随状态，表示被冰冻平面捕获。
	Follow
};

struct ShellProperty
{
	float MoveSpeed;
	ShellState CurrState;
};

//弹药Pawn在PawnMaster中分配的类型标签。
extern PawnType gShellType;

//弹药Pawn的AI控制类型标记。
extern AIControlType gShellAIControlType;

//全局的弹药属性分配池。
extern MyStaticAllocator<ShellProperty> gShellPropertyAllocator;