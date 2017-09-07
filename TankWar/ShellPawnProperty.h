#pragma once
#include "ShellPawnProperty.h"
#include "MyStaticAllocator.h"
#include "GCommanders.h"

//���ĵ�ҩ������
#define MAX_SHELL_PAWN_NUM 900

//�ڵ���λ�Ŀɿ�������������ֻ��һ����
#define CONTROLITEM_NUM_SHELL 1
//����������š�
#define CONTROLITEM_INDEX_SHELL_ROOT 0

//�������������
#define BONE_INDEX_SHELL_ROOT 0

//�ƶ�״̬��־��
#define STORY_FRAGMENT_SHELL_MOVE 1
//����״̬��
#define STORY_FRAGMENT_SHELL_FOLLOW 2

enum ShellState :unsigned int
{
	//�����ƶ�״̬��
	Move,
	//����״̬����ʾ������ƽ�沶��
	Follow,
	//����״̬
	Dead
};

struct ShellProperty:
	public PawnProperty
{
	//�ƶ��ٶȡ�
	float MoveSpeed;
	//�ܹ��ƶ���ʣ����룬һ��������뱻�ľ�����ҩ��λ���ᱻɾ����
	float RestDist;
	//���һ���˶��ľ��롣
	float DeltaDist;
	ShellState CurrState;

	union
	{
		//�ӵ������ƶ�ʱ���ƶ�����������Ժ�StartPos����ͬһ���ڴ棬
		//StartPosר�Ŵ洢��ʼ����̬������һ�γ�ʼ��Shellpawn֮��
		//StartPos��û�����ˣ����ǽ���һ����ר������һ���������Vector��
		//��������ļ��㡣
		float MoveDirection[4];
		XMFLOAT4 MoveDirectionXYZW;
		//�ӵ��ĳ�ʼλ�ã�����ת����
		XMFLOAT4X4 StartPos;
	};


public:
	ShellProperty();
	DELETE_COPY_CONSTRUCTOR(ShellProperty)
};

//��ҩPawn��PawnMaster�з�������ͱ�ǩ��
extern PawnType gShellPawnType;

//��ҩPawn��AI�������ͱ�ǡ�
extern AIControlType gShellAIControlType;

//ȫ�ֵĵ�ҩ���Է���ء�
extern MyStaticAllocator<ShellProperty> gShellPropertyAllocator;

//��ҩ��λ��Ĭ���ƶ��ٶȡ�
extern float gShellDefaultMoveSpeed;

//��ҩ��λĬ�ϵ��ƶ����롣
extern float gShellDefaultRestDist;

//��ҩ��λĬ�ϵĵ�֡�ƶ����롣
extern float gShellDefaultDeltaDist;

//��ҩ��λ��Ĭ��״̬��
extern ShellState gShellDefaultState;

//��ҩ�ĳ�ʼ�������ת����
extern XMFLOAT4X4 gShellDefaultStartPos;
