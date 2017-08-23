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
	ASSERT(false && "BasePawn获取类型的方法没有被覆盖。");
}
