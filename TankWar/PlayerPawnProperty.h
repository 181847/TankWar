#pragma once
#include "GCommanders.h"

//前置声明。
class PlayerPawn;

//可生成的PawnPlayer的数量
#define MAX_PLAYER_PAWN_NUM ((unsigned int)5)
#define CONTROLITEM_NUM_PLAYER_PAWN 4

//玩家的可控制ControlItem的根节点序号，注意玩家当中的摄像机和可渲染根节点是分开的。
#define CONTROLITEM_INDEX_PLAYER_PAWN_ROOT 0
//炮台
#define CONTROLITEM_INDEX_PLAYER_PAWN_BATTERY 1
//车身的控制器
#define CONTROLITEM_INDEX_PLAYER_PAWN_MAINBODY 2
//炮管控制器
#define CONTROLITEM_INDEX_PLAYER_PAWN_BARREL 3

//骨骼数组序号
#define BONE_INDEX_PLAYER_PAWN_ROOT 0
//摄像机坐标。
#define BONE_INDEX_PLAYER_PAWN_CAMERA_POS 1
//摄像机目标。
#define BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET 2
//炮台
#define BONE_INDEX_PLAYER_PAWN_BATTERY 3
//车身
#define BONE_INDEX_PLAYER_PAWN_MAINBODY 4
//炮管
#define BONE_INDEX_PLAYER_PAWN_BARREL 5

//默认状态标志。
#define STORY_FRAGMENT_PLAYER_DEFAULT 1

//射线最大长度
#define MAX_RAY_LENGTH 500

//玩家属性定义
struct PlayerProperty : public PawnProperty
{
	//直线速度
	float MoveSpeed;
	//车身旋转速度
	float RotationSpeed;

public:
	PlayerProperty();
	DELETE_COPY_CONSTRUCTOR(PlayerProperty)
};

//在PawnMaster中的类型标识。
extern PawnType gPlayerPawnType;

//在PlayerCommander中的控制标签。
extern PlayerControlType gPlayerControlType;

//AI控制标签。
extern AIControlType gPlayerAIControlType;

//属性分配池
extern MyStaticAllocator<PlayerProperty> gPlayerPropertyAllocator;