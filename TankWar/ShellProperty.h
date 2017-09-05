#pragma once
#include "ShellProperty.h"
#include "MyStaticAllocator.h"
#include "GCommanders.h"

//���ĵ�ҩ������
#define MAX_SHELL_PAWN_NUM 90000

//�ڵ���λ�Ŀɿ�������������ֻ��һ����
#define CONTROLITEM_NUM_SHELL 1
//����������š�
#define CONTROLITEM_INDEX_SHELL 0

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
	Follow
};

struct ShellProperty
{
	float MoveSpeed;
	ShellState CurrState;
};

//��ҩPawn��PawnMaster�з�������ͱ�ǩ��
extern PawnType gShellType;

//��ҩPawn��AI�������ͱ�ǡ�
extern AIControlType gShellAIControlType;

//ȫ�ֵĵ�ҩ���Է���ء�
extern MyStaticAllocator<ShellProperty> gShellPropertyAllocator;