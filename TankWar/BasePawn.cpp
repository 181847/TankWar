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
	return m_type;
}
