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

//注册全局的所有Commander。
void RegisterGlobalCommanders(
	PawnMaster *			gPawnMaster,
	BoneCommander *		gBoneCommander,
	CollideCommander *	gCollideCommander,
	AICommander *		gAICommander,
	PlayerCommander *	gPlayerCommander);