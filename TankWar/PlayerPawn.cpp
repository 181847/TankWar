#include "PlayerPawn.h"



PlayerPawn::PlayerPawn()
{
}


PlayerPawn::~PlayerPawn()
{
}

PawnCommandTemplate* PlayerPawn::GeneratePawnCommandTemplate()
{
	return new PlayerPawnCommandTemplate();
}

BasePawn* PlayerPawnCommandTemplate::CreatePawn(PawnDesc* pDesc, Scence* pScence)
{
	PlayerPawn* newPawn = m_playerAllocator.Malloc();
	//�ӳ����д������������������洢��Player�С�
	newPawn->m_pCamera = pScence->AppendCamera();

	P_Root_Control(newPawn) =
		pScence->GetNewControlItemForRenderItem("���");

	
}

void PlayerPawnCommandTemplate::DestoryPawn(BasePawn* pPawn, Scence* pScence)
{

}
