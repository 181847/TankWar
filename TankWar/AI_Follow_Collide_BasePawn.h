#pragma once
#include "BasePawn.h"
//��ȡPawn����Ե�FollowCommander
#define STATIC_P_AI_COMMANDER(Class_Base_On_AI_Follow_Collide_BasePawn) (Class_Base_On_AI_Follow_Collide_BasePawn.m_pAICommander)
//��ȡPawn����Ե�FollowCommander
#define STATIC_P_FOLLOW_COMMANDER(Class_Base_On_AI_Follow_Collide_BasePawn) (Class_Base_On_AI_Follow_Collide_BasePawn.m_pFollowCommander)
//��ȡPawn����Ե�FollowCommander
#define STATIC_P_COLLIDE_COMMANDER(Class_Base_On_AI_Follow_Collide_BasePawn) (Class_Base_On_AI_Follow_Collide_BasePawn.m_pCollideCommander)
class AI_Follow_Collide_BasePawn :
	public BasePawn
{
public:
	AI_Follow_Collide_BasePawn();
	~AI_Follow_Collide_BasePawn();
	//����Pawn�����ɴ���ṹ������ṹ����PawnMaster�У�ʹ��PawnMaster�ܹ���������Pawn��
	virtual PawnCommandTemplate* GeneratePawnCommandTemplate() = 0;
	//ע��AICommander��
	virtual void RegisterAICommander		(AICommander* pAICommander);
	//ע�����Commander
	virtual void RegisterFollowCommander	(FollowCommander* pFollowCommander);
	//ע����ײCommander
	virtual void RegisterCollideCommander	(CollideCommander* pCollideCommander);

protected:
	//�������Pawn�Զ����Ƶ�AICommander��
	static AICommander *		m_pAICommander;
	//�������Pawn��ControlItem����Ч����Commander��
	static FollowCommander *	m_pFollowCommander;
	//�������Pawn��ײЧ����Commander��
	static CollideCommander *	m_pCollideCommander;
};

