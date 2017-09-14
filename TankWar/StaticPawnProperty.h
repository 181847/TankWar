#pragma once
#include "GCommanders.h"

//����pawn������
#define MAX_STATIC_PAWN_NUM 100
//�����������С
#define CONTROLITEM_NUM_STATIC_PAWN 1

//����������š�
#define CONTROLITEM_INDEX_STATIC_PAWN_ROOT 0

//���������
#define BONE_INDEX_STATIC_PAWN_ROOT 0

//��ײ���������
#define COLLIDE_RECT_NUM_ARMORED_CAR 1
//����ײ����š�
#define COLLIDE_RECT_INDEX_STATIC_PAWN_ROOT 0

struct StaticPawnProperty :
	public PawnProperty
{
	const char * GeometryName;
	const char * ShapeName;
	const char * MaterialName;

	XMFLOAT3 InitRotation;
	XMFLOAT3 InitTranslation;

	//��ײ���С��
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

	
	//��ײ�����ͱ��
	CollideType CBoxType;
};

extern PawnType gStaticPawnType;

extern MyStaticAllocator<StaticPawnProperty> gStaticPawnPropertyAllocator;
