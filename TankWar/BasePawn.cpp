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

void BasePawn::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	m_pPawnMaster = pPawnMaster;
	//����Pawn������ģ��ŵ�PawnMaster��
	//ͨ��PawnMaster.NewPawn(PawnType)�Ϳ����Զ�������Ӧ��Pawn������
	pPawnMaster->RegisterCommandTemplate(this->GeneratePawnCommandTemplate());
}
