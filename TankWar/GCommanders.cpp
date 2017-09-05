#include "GCommanders.h"

PawnMaster *		gPawnMaster			= nullptr;
BoneCommander *		gBoneCommander		= nullptr;
CollideCommander *	gCollideCommander	= nullptr;
AICommander *		gAICommander		= nullptr;
PlayerCommander *	gPlayerCommander	= nullptr;

void RegisterGlobalCommanders(
	PawnMaster * pPM,
	BoneCommander * pBC, 
	CollideCommander * pCC, 
	AICommander * pAIC, 
	PlayerCommander * pPC)
{
	gPawnMaster			= pPM;
	gBoneCommander		= pBC;
	gCollideCommander	= pCC;
	gAICommander		= pAIC;
	gPlayerCommander	= pPC;
}
