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

void PlayerPawn::RegisterPlayerCommander(PlayerCommander * pPlayerCommander)
{
	m_pPlayerCommander = pPlayerCommander;
	CommandId_Player_Control = 
		m_pPlayerCommander->RegisterNewCommandTemplate(new CommmandTemplate_Player_Control());
}

BasePawn* PlayerPawnCommandTemplate::CreatePawn(PawnDesc* pDesc, Scence* pScence)
{
	PlayerPawn* newPawn = m_playerAllocator.Malloc();
	//�ӳ����д������������������洢��Player�С�
	newPawn->m_pCamera = pScence->AppendCamera();

	P_Root_Control(newPawn) =
		//ָ����ҵ�һ����Ⱦ����ʱshapeGeo�е�box����
		//ָ��MeshGeometry��SubMesh��Material��
		pScence->GetNewControlItemForRenderItem("shapeGeo", "box", "box");
	//Ŀǰ���ֻ��Ҫһ����Ⱦ����������ҵĸ�������ֻ��һ���ڵ㣬
	//���Ը��ڵ�ĺ����Ϊnull��
	P_Root_Control(newPawn)->Next = nullptr;

	//�������ӵ�PlayerCommander�Ŀ����С�
	AddControlTo_PlayerCommander(newPawn);

	//��ӿ��Ƶ�FollowCommander���������׷��box���ƶ���
	AddControlTo_FollowCommander(newPawn);
}

void PlayerPawnCommandTemplate::DestoryPawn(BasePawn* pPawn, Scence* pScence)
{
	PlayerPawn* toDeletePawn = (PlayerPawn*)pPawn;

	//��PlayerPawn��Player�Ŀ����г�ȥ��
	DetachControlFrom_PlayerCommander(toDeletePawn);

	//��Player�����и����Ч����FollowCommander��ɾ����
	DetachControlFrom_FollowCommander(toDeletePawn);

	//�ͷ����ControlItem���������е�ControlItem��
	pScence->FreeControlItemLink(P_Root_Control(toDeletePawn));

	//�ͷ��������ͷ��
	pScence->FreeCamera(P_PlayerCameraPositionControl(toDeletePawn));

	//����Pawn����
	m_playerAllocator.Free(toDeletePawn);
}

void PlayerPawnCommandTemplate::AddControlTo_PlayerCommander(PlayerPawn * pPlayerPawn)
{
	//������PlayerCommander������һ���µĿ��Ʋۣ�
	STATIC_P_PLAYER_COMMANDER(PlayerPawn)->
		ApplyForNewControlSlot(CommandId_Player_Control, pPlayerPawn);
}

void PlayerPawnCommandTemplate::AddControlTo_FollowCommander(PlayerPawn * pPlayerPawn)
{
	//�������Ŀ�������ƽ�Ƹ���box����������ƫ��5����λ��Y�ᣩ��
	STATIC_P_FOLLOW_COMMANDER(PlayerPawn)->
		ApplyForNewControlSlot(
			P_PlayerCameraPositionControl(pPlayerPawn),
			COMMAND_FOLLOW_COMMANDER_TRANSLATION_FOLLOW,
			P_Root_Control(pPlayerPawn),
			0, 5, 0);
}
