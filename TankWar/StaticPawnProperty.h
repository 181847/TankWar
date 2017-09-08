#pragma once
#include "GCommanders.h"

//最大的pawn数量。
#define MAX_STATIC_PAWN_NUM 100
//控制器数组大小
#define CONTROLITEM_NUM_STATIC_PAWN 1

//根控制器序号。
#define CONTROLITEM_INDEX_STATIC_PAWN_ROOT 0

//根骨骼序号
#define BONE_INDEX_STATIC_PAWN_ROOT 0

//碰撞体的数量。
#define COLLIDE_RECT_NUM_ARMORED_CAR 1
//根碰撞体序号。
#define COLLIDE_RECT_INDEX_STATIC_PAWN_ROOT 0

struct StaticPawnProperty :
	public PawnProperty
{
	const char * GeometryName;
	const char * ShapeName;
	const char * MaterialName;

	XMFLOAT3 InitRotation;
	XMFLOAT3 InitTranslation;

	//碰撞体大小。
	union
	{
		struct
		{
			XMFLOAT3 XM_MinXYZ;
			XMFLOAT3 XM_MaxXYZ;
		};
		struct
		{
			float MinXYZ[3];
			float MaxXYZ[3];
		};

		struct
		{
			float	Xmin, Ymin, Zmin,
					Xmax, Ymax, Zmax;
		};
	} CBoxSize;

	
	//碰撞体类型标记
	CollideType CBoxType;
};

extern PawnType gStaticPawnType;

extern MyStaticAllocator<StaticPawnProperty> gStaticPawnPropertyAllocator;
