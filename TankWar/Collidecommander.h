#pragma once
#include "BasePawn.h"
#include "ControlItem.h"

typedef int CollideType;
//�������͵���ײ��
#define COLLIDE_TYPE_ALL -1
//�Ƿ���ײ�����ͱ��
#define COLLIDE_RECT_TYPE_NONE 0
//��ײ������1
#define COLLIDE_TYPE_BOX_1 1
#define COLLIDE_TYPE_BOX_2 2
#define COLLIDE_TYPE_BOX_3 3
#define COLLIDE_TYPE_BOX_4 4

#define MAX(X, Y) (X > Y ? X : Y)
#define MIN(X, Y) (X < Y ? X : Y)

//��֤ǰһ����С�ں�һ��������������㣬������������ֵ��
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
	//���ߵ���ʼλ�ú���ת����
	ControlItem* controlPD;
	//���ߵĳ��ȡ�
	float RayLength;
	//�����ܹ����ĸ�������롣
	float MinusLength;

	//��ײ�����Ľ��
	struct
	{
		//�Ƿ�����ײ��
		bool CollideHappended;
		//��ײ�������������ꡣ
		XMFLOAT3 CollideLocation;
		//��ײ��������ײ�塣
		CollideBox* pCollideBox;
	} Result;
};


//��������ײ�峣�õĳߴ�ṹ��
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

//��������ײ�塣
struct CollideBox
{
	//��ײ�����ͱ�ǡ�
	CollideType Type;
	//����Χ��뾶��ƽ��
	float SquareSphereRadius;
	//��ײ��ߴ硣
	BoxSize Size;
	//��Ӧ��ControlItem
	ControlItem *pCItem;
	//��Ӧ��Pawn��
	BasePawn* pPawn;

public:
	CollideBox();
	//������ײ�еĳߴ磬�����Χ��İ뾶ƽ����
	void CaculateRadius();

	//������ײ�еĵ�����
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
	//��������
	RayDetect* NewRay(ControlItem * pCItem, float rayLength);
	//ɾ������
	void DeleteRay(RayDetect* pDeleteRay);

	//���볤������ײ�塣
	CollideBox* NewCollideBox(CollideType type, ControlItem* pCItem, BasePawn* pPawn);
	//���ճ�������ײ�塣
	void DeleteCollideBox(CollideBox* pBox);

	//���߽�����ײ��飬ָ����ײ������ͣ�
	//��ײ��������RayDetect�����С�
	void CollideDetect(RayDetect* ray_input_and_output, CollideType detectType);

	//�ȼ���Χ���Ƿ���ײ�ļ�ⷽ����
	void RayCheckWithSphere(
		//input
		RayDetect* ray, CollideBox* pBox, 
		//output
		bool& isCollided, XMFLOAT3& location, float& tl);

	//��������Ƿ����ײ����ײ��
	void RayCollideCheck(
		//input
		RayDetect* ray, CollideBox* pBox,
		//output
		bool& isCollided, XMFLOAT3& location, float& tl);

	//��ֻ������ת��ƽ�Ƶľ���ȡ�档
	void inv(XMFLOAT4X4& matrixWith_R_T);
	void Caculate_MaxT_MinT(
		BoxSize&	boxSize,	//��ײ�еĳߴ�
		XMFLOAT4&	rDirection,	//���߷���ֻ��xyz�����������á�
		XMFLOAT4&	rPosition,	//�������ֻ꣬��xyz�����������á�
		float&		maxt,		//�����1��������ߺ���ײ���ཻ��maxtӦ��С��mint��
		float&		mint);		//�����2
	//Ϊ�˷�ֹ��0����
	float My_Minus_And_Divide(float dist, float rp, float rd);

protected:
	//���߷���ء�
	MyStaticAllocator<RayDetect> RayAllocator;
	//��ײ�з���ء�
	LinkedAllocator<CollideBox> BoxAllocator;
};

