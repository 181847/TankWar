#include "BasePawn.h"

BasePawn::BasePawn()
{

}


BasePawn::~BasePawn()
{
}

//��ȡPawn�����ͣ�PawnTypeΪint�ı�����
//�����ļ���PawnType�궨����BasePawn.h�С�
PawnType BasePawn::GetType()
{
	ThrowIfFailed(false && "BasePawn��ȡ���͵ķ���û�б����ǡ�");
}

void BasePawn::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	ThrowIfFailed(false && "BasePawn��ע�᷽��û�б����ǡ�");
}
