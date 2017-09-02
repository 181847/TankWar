#pragma once
#include "BasePawn.h"
#include "ControlItem.h"

//非法碰撞体类型标记
#define COLLIDE_RECT_TYPE_NONE 0
//碰撞体类型1
#define COLLIDE_RECT_TYPE_1 1

struct RayDetect
{
	//绑定的ControlItem，
	//射线的方向将会由这个ControlItem的世界变换矩阵控制，
	//默认Z轴方向为射线的初始方向。
	XMFLOAT4X4 RayWorld;
	float RayLength;

	//碰撞发生的结果
	struct
	{
		//是否发生碰撞。
		bool CollideHappended;
		BasePawn * pPanwHolder;
		ControlItem * pCollideControlItem;
	} Result;
};

class Collidecommander
{
public:
	Collidecommander();
	~Collidecommander();
};

