#pragma once
#include "BasePawn.h"
#include "ControlItem.h"

typedef int CollideType;
//所有类型的碰撞体
#define COLLIDE_TYPE_ALL -1
//非法碰撞体类型标记
#define COLLIDE_RECT_TYPE_NONE 0
//碰撞体类型1
#define COLLIDE_TYPE_BOX_1 1
#define COLLIDE_TYPE_BOX_2 2
#define COLLIDE_TYPE_BOX_3 3
#define COLLIDE_TYPE_BOX_4 4

#define MAX(X, Y) (X > Y ? X : Y)
#define MIN(X, Y) (X < Y ? X : Y)

//保证前一个数小于后一个数，如果不满足，交换两个数的值。
#define MIN_SWAP(expectLess, expectGreater) \
	if (expectLess > expectGreater)\
	{\
		MySwap(expectLess, expectGreater);\
	}
#define MAX_SWAP(expectGreater, expectLess) \
	if (expectGreater < expectLess)\
	{\
		MySwap(expectLess, expectGreater);\
	}

template<typename T>
void MySwap(T& x, T& y)
{
	T buffer;
	buffer = x;
	x = y;
	y = buffer;
}

struct CollideBox;

struct RayDetect
{
	//射线的起始位置和旋转方向。
	ControlItem* controlPD;
	//射线的长度。
	float RayLength;
	//射线能够检测的负方向距离。
	float MinusLength;

	//碰撞发生的结果
	struct
	{
		//是否发生碰撞。
		bool CollideHappended;
		//碰撞发生的世界坐标。
		XMFLOAT3 CollideLocation;
		//碰撞发生的碰撞体。
		CollideBox* pCollideBox;
	} Result;
};


//长方形碰撞体常用的尺寸结构。
struct BoxSize
{
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
			float Xmin;
			float Ymin;
			float Zmin;

			float Xmax;
			float Ymax;
			float Zmax;
		};
	};

public:
	BoxSize();
};

//长方形碰撞体。
struct CollideBox
{
	//碰撞体类型标记。
	CollideType Type;
	//最大包围球半径的平方
	float SquareSphereRadius;
	//碰撞体尺寸。
	BoxSize Size;
	//对应的ControlItem
	ControlItem *pCItem;
	//对应的Pawn。
	BasePawn* pPawn;

public:
	CollideBox();
	//根据碰撞盒的尺寸，计算包围球的半径平方。
	void CaculateRadius();

	//设置碰撞盒的刀削。
	void Resize(XMFLOAT3 minXYZ, XMFLOAT3 maxXYZ);
};


class CollideCommander
{
public:
	CollideCommander(UINT MaxRayCount, UINT MaxBoxCount);
	CollideCommander(const CollideCommander&) = delete;
	CollideCommander& operator=(const CollideCommander&) = delete;
	~CollideCommander();

public:
	//申请射线
	RayDetect* NewRay(ControlItem * pCItem, float rayLength);
	//删除射线
	void DeleteRay(RayDetect* pDeleteRay);

	//申请长方形碰撞体。
	CollideBox* NewCollideBox(CollideType type, ControlItem* pCItem, BasePawn* pPawn);
	//回收长方形碰撞体。
	void DeleteCollideBox(CollideBox* pBox);

	//射线进行碰撞检查，指定碰撞体的类型，
	//碰撞结果存放在RayDetect参数中。
	void CollideDetect(RayDetect* ray_input_and_output, CollideType detectType);

	//先检查包围球是否碰撞的检测方法。
	void RayCheckWithSphere(
		//input
		RayDetect* ray, CollideBox* pBox, 
		//output
		bool& isCollided, XMFLOAT3& location, float& tl);

	//检查射线是否和碰撞盒碰撞。
	void RayCollideCheck(
		//input
		RayDetect* ray, CollideBox* pBox,
		//output
		bool& isCollided, XMFLOAT3& location, float& tl);

	//对只含有旋转和平移的矩阵取逆。
	void inv(XMFLOAT4X4& matrixWith_R_T);
	void Caculate_MaxT_MinT(
		BoxSize&	boxSize,	//碰撞盒的尺寸
		XMFLOAT4&	rDirection,	//射线方向，只有xyz三个分量有用。
		XMFLOAT4&	rPosition,	//射线坐标，只有xyz三个分量有用。
		float&		maxt,		//交叉点1，如果射线和碰撞盒相交，maxt应该小于mint。
		float&		mint);		//交叉点2
	//为了防止除0带来
	float My_Minus_And_Divide(float dist, float rp, float rd);

protected:
	//射线分配池。
	MyStaticAllocator<RayDetect> RayAllocator;
	//碰撞盒分配池。
	LinkedAllocator<CollideBox> BoxAllocator;
};

