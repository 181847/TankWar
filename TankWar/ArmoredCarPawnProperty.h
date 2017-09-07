#pragma once
#include "GCommanders.h"


//����װ�׳�����
#define MAX_PAWN_CAR_NUM 60

//װ�׳���λ�Ŀɿ�������������Ĭ��������һ��������������λ�ƣ���һ��������ת��
#define CONTROLITEM_NUM_ARMORED_CAR 2
//װ�׳��ĸ����������
#define CONTROLITEM_INDEX_CAR_ROOT 0
//װ�׳����������
#define CONTROLITEM_INDEX_CAR_MAIN_BODY 1

//�������������
#define BONE_INDEX_PLAYER_CAR_ROOT CONTROLITEM_INDEX_CAR_ROOT
//����������
#define BONE_INDEX_PLAYER_CAR_MAIN_BODY CONTROLITEM_INDEX_CAR_MAIN_BODY

//��ײ���������
#define COLLIDE_RECT_NUM_ARMORED_CAR 1
//����ײ�����
#define COLLIDE_RECT_INDEX_CAR_ROOT 0

//�����ƶ�״̬��־��
#define STORY_FRAGMENT_CAR_MOVE 1
//װ�׳���׼״̬��־��
#define STORY_FRAGMENT_CAR_AIM 2
//װ�׳�����״̬��־��
#define STORY_FRAGMENT_CAR_SHOUT 3

//Ĭ���ƶ��ٶ�
extern const float		gCarDefaultMoveSpeed;
//Ĭ����תaaaa�ٶ�aaa
extern const float		gCarDefaultRotationSpeed;


//��PawnMaster�е����ͱ�ʶ��
extern PawnType			gArmoredCarPawnType;
//��ҵĿ���ģʽ��
extern AIControlType	gArmoredCarAIControlType;


//װ�׳����Զ���
struct CarProperty : public PawnProperty
{
	//ֱ���ٶ�
	float MoveSpeed;
	//������ת�ٶ�
	float RotationSpeed;
};

//���Է����
extern MyStaticAllocator<CarProperty> gArmoredCarPropertyAllocator;