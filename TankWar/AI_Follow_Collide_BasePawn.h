#pragma once
#include "BasePawn.h"
//获取Pawn中针对的FollowCommander
#define STATIC_P_AI_COMMANDER(Class_Base_On_AI_Follow_Collide_BasePawn) (Class_Base_On_AI_Follow_Collide_BasePawn.m_pAICommander)
//获取Pawn中针对的FollowCommander
#define STATIC_P_FOLLOW_COMMANDER(Class_Base_On_AI_Follow_Collide_BasePawn) (Class_Base_On_AI_Follow_Collide_BasePawn.m_pFollowCommander)
//获取Pawn中针对的FollowCommander
#define STATIC_P_COLLIDE_COMMANDER(Class_Base_On_AI_Follow_Collide_BasePawn) (Class_Base_On_AI_Follow_Collide_BasePawn.m_pCollideCommander)
class AI_Follow_Collide_BasePawn :
	public BasePawn
{
public:
	AI_Follow_Collide_BasePawn();
	~AI_Follow_Collide_BasePawn();
	//生成Pawn的生成代理结构，这个结构用于PawnMaster中，使得PawnMaster能够控制生成Pawn。
	virtual PawnCommandTemplate* GeneratePawnCommandTemplate() = 0;
	//注册AICommander。
	virtual void RegisterAICommander		(AICommander* pAICommander);
	//注册跟随Commander
	virtual void RegisterFollowCommander	(FollowCommander* pFollowCommander);
	//注册碰撞Commander
	virtual void RegisterCollideCommander	(CollideCommander* pCollideCommander);

protected:
	//负责这个Pawn自动控制的AICommander。
	static AICommander *		m_pAICommander;
	//负责这个Pawn的ControlItem跟随效果的Commander。
	static FollowCommander *	m_pFollowCommander;
	//负责这个Pawn碰撞效果的Commander。
	static CollideCommander *	m_pCollideCommander;
};

