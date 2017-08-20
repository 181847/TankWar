#include "BasePawn.h"

BasePawn::BasePawn()
{
}


BasePawn::~BasePawn()
{
}

//获取Pawn的类型，PawnType为int的别名，
//基础的几个PawnType宏定义在BasePawn.h中。
PawnType BasePawn::GetType()
{
	return m_type;
}

void BasePawn::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	m_pPawnMaster = pPawnMaster;
	//将本Pawn的生成模板放到PawnMaster，
	//通过PawnMaster.NewPawn(PawnType)就可以自动创建相应的Pawn对象了
	pPawnMaster->RegisterCommandTemplate(this->GeneratePawnCommandTemplate());
}
