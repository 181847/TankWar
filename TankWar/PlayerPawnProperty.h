#pragma once
#include "GCommanders.h"

//ǰ��������
class PlayerPawn;

//�����ɵ�PawnPlayer������
#define MAX_PLAYER_PAWN_NUM ((unsigned int)5)
#define CONTROLITEM_NUM_PLAYER_PAWN 4

//��ҵĿɿ���ControlItem�ĸ��ڵ���ţ�ע����ҵ��е�������Ϳ���Ⱦ���ڵ��Ƿֿ��ġ�
#define CONTROLITEM_INDEX_PLAYER_PAWN_ROOT 0
//��̨
#define CONTROLITEM_INDEX_PLAYER_PAWN_BATTERY 1
//����Ŀ�����
#define CONTROLITEM_INDEX_PLAYER_PAWN_MAINBODY 2
//�ڹܿ�����
#define CONTROLITEM_INDEX_PLAYER_PAWN_BARREL 3

//�����������
#define BONE_INDEX_PLAYER_PAWN_ROOT 0
//��������ꡣ
#define BONE_INDEX_PLAYER_PAWN_CAMERA_POS 1
//�����Ŀ�ꡣ
#define BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET 2
//��̨
#define BONE_INDEX_PLAYER_PAWN_BATTERY 3
//����
#define BONE_INDEX_PLAYER_PAWN_MAINBODY 4
//�ڹ�
#define BONE_INDEX_PLAYER_PAWN_BARREL 5

//Ĭ��״̬��־��
#define STORY_FRAGMENT_PLAYER_DEFAULT 1

//������󳤶�
#define MAX_RAY_LENGTH 500

//������Զ���
struct PlayerProperty : public PawnProperty
{
	//ֱ���ٶ�
	float MoveSpeed;
	//������ת�ٶ�
	float RotationSpeed;

public:
	PlayerProperty();
	DELETE_COPY_CONSTRUCTOR(PlayerProperty)
};

//��PawnMaster�е����ͱ�ʶ��
extern PawnType gPlayerPawnType;

//��PlayerCommander�еĿ��Ʊ�ǩ��
extern PlayerControlType gPlayerControlType;

//AI���Ʊ�ǩ��
extern AIControlType gPlayerAIControlType;

//���Է����
extern MyStaticAllocator<PlayerProperty> gPlayerPropertyAllocator;