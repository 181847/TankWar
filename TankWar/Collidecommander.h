#pragma once
#include "BasePawn.h"
#include "ControlItem.h"

//�Ƿ���ײ�����ͱ��
#define COLLIDE_RECT_TYPE_NONE 0
//��ײ������1
#define COLLIDE_RECT_TYPE_1 1

struct RayDetect
{
	//�󶨵�ControlItem��
	//���ߵķ��򽫻������ControlItem������任������ƣ�
	//Ĭ��Z�᷽��Ϊ���ߵĳ�ʼ����
	XMFLOAT4X4 RayWorld;
	float RayLength;

	//��ײ�����Ľ��
	struct
	{
		//�Ƿ�����ײ��
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

