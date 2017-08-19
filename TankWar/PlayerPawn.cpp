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
	//从场景中创建摄像机，把摄像机存储到Player中。
	newPawn->m_pCamera = pScence->AppendCamera();

	P_Root_Control(newPawn) =
		pScence->GetNewControlItemForRenderItem("玩家");

	
}

void PlayerPawnCommandTemplate::DestoryPawn(BasePawn* pPawn, Scence* pScence)
{

}
