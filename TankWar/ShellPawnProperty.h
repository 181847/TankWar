#pragma once
#include "ShellPawnProperty.h"
#include "MyStaticAllocator.h"
#include "GCommanders.h"

//最大的弹药数量。
#define MAX_SHELL_PAWN_NUM 900

//炮弹单位的可控制网格数量，只有一个。
#define CONTROLITEM_NUM_SHELL 1
//根控制器序号。
#define CONTROLITEM_INDEX_SHELL_ROOT 0

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
	Follow,
	//死亡状态
	Dead
};

struct ShellProperty:
	public PawnProperty
{
	//移动速度。
	float MoveSpeed;
	//能够移动的剩余距离，一旦这个距离被耗尽，弹药单位将会被删除。
	float RestDist;
	//最近一次运动的距离。
	float DeltaDist;
	ShellState CurrState;

	union
	{
		//子弹自由移动时的移动方向，这个属性和StartPos共用同一段内存，
		//StartPos专门存储初始的姿态，当第一次初始化Shellpawn之后，
		//StartPos就没有用了，我们将这一部分专门生成一个代表方向的Vector，
		//方便后续的计算。
		float MoveDirection[4];
		XMFLOAT4 MoveDirectionXYZW;
		//子弹的初始位置，和旋转方向。
		XMFLOAT4X4 StartPos;
	};


public:
	ShellProperty();
	DELETE_COPY_CONSTRUCTOR(ShellProperty)
};

//弹药Pawn在PawnMaster中分配的类型标签。
extern PawnType gShellPawnType;

//弹药Pawn的AI控制类型标记。
extern AIControlType gShellAIControlType;

//全局的弹药属性分配池。
extern MyStaticAllocator<ShellProperty> gShellPropertyAllocator;

//弹药单位的默认移动速度。
extern float gShellDefaultMoveSpeed;

//弹药单位默认的移动距离。
extern float gShellDefaultRestDist;

//弹药单位默认的单帧移动距离。
extern float gShellDefaultDeltaDist;

//弹药单位的默认状态。
extern ShellState gShellDefaultState;

//弹药的初始坐标和旋转方向。
extern XMFLOAT4X4 gShellDefaultStartPos;
