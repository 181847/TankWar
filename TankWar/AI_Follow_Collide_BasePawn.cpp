#include "AI_Follow_Collide_BasePawn.h"



AI_Follow_Collide_BasePawn::AI_Follow_Collide_BasePawn()
{
}


AI_Follow_Collide_BasePawn::~AI_Follow_Collide_BasePawn()
{
}

void AI_Follow_Collide_BasePawn::RegisterAICommander(AICommander * pAICommander)
{
	m_pAICommander = pAICommander;
}

void AI_Follow_Collide_BasePawn::RegisterFollowCommander(FollowCommander * pFollowCommander)
{
	m_pFollowCommander = pFollowCommander;
}

void AI_Follow_Collide_BasePawn::RegisterCollideCommander(CollideCommander * pCollideCommander)
{
	m_pCollideCommander = pCollideCommander;
}
