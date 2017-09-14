#pragma once
#include "PawnMaster.h"
#include "BoneCommander.h"
#include "PlayerCommander.h"
#include "CollideCommander.h"
#include "AICommander.h"

extern PawnMaster *			gPawnMaster;
extern BoneCommander *		gBoneCommander;
extern CollideCommander *	gCollideCommander;
extern AICommander *		gAICommander;
extern PlayerCommander *	gPlayerCommander;

//ע��ȫ�ֵ�����Commander��
void RegisterGlobalCommanders(
	PawnMaster *			gPawnMaster,
	BoneCommander *		gBoneCommander,
	CollideCommander *	gCollideCommander,
	AICommander *		gAICommander,
	PlayerCommander *	gPlayerCommander);