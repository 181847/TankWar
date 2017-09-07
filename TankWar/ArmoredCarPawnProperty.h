#pragma once
#include "GCommanders.h"


//最大的装甲车数量
#define MAX_PAWN_CAR_NUM 60

//装甲车单位的可控制网格数量，默认两个，一个根控制器控制位移，另一个控制旋转。
#define CONTROLITEM_NUM_ARMORED_CAR 2
//装甲车的根控制器序号
#define CONTROLITEM_INDEX_CAR_ROOT 0
//装甲车车身控制器
#define CONTROLITEM_INDEX_CAR_MAIN_BODY 1

//根骨骼数组序号
#define BONE_INDEX_PLAYER_CAR_ROOT CONTROLITEM_INDEX_CAR_ROOT
//车身骨骼序号
#define BONE_INDEX_PLAYER_CAR_MAIN_BODY CONTROLITEM_INDEX_CAR_MAIN_BODY

//碰撞体的数量。
#define COLLIDE_RECT_NUM_ARMORED_CAR 1
//根碰撞体序号
#define COLLIDE_RECT_INDEX_CAR_ROOT 0

//车身移动状态标志。
#define STORY_FRAGMENT_CAR_MOVE 1
//装甲车瞄准状态标志。
#define STORY_FRAGMENT_CAR_AIM 2
//装甲车开火状态标志。
#define STORY_FRAGMENT_CAR_SHOUT 3

//默认移动速度
extern const float		gCarDefaultMoveSpeed;
//默认旋转aaaa速度aaa
extern const float		gCarDefaultRotationSpeed;


//在PawnMaster中的类型标识。
extern PawnType			gArmoredCarPawnType;
//玩家的控制模式。
extern AIControlType	gArmoredCarAIControlType;


//装甲车属性定义
struct CarProperty : public PawnProperty
{
	//直线速度
	float MoveSpeed;
	//车身旋转速度
	float RotationSpeed;
};

//属性分配池
extern MyStaticAllocator<CarProperty> gArmoredCarPropertyAllocator;