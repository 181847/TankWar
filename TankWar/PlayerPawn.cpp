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
	//从场景中创建摄像机，把摄像机存储到Player中。
	newPawn->m_pCamera = pScence->AppendCamera();

	P_Root_Control(newPawn) =
		//指定玩家的一个渲染网格时shapeGeo中的box网格，
		//指定MeshGeometry、SubMesh、Material。
		pScence->GetNewControlItemForRenderItem("shapeGeo", "box", "box");
	//目前玩家只需要一个渲染网格，所以玩家的根控制器只有一个节点，
	//所以根节点的后继设为null。
	P_Root_Control(newPawn)->Next = nullptr;

	//将玩家添加到PlayerCommander的控制中。
	AddControlTo_PlayerCommander(newPawn);

	//添加控制到FollowCommander，让摄像机追随box的移动。
	AddControlTo_FollowCommander(newPawn);
}

void PlayerPawnCommandTemplate::DestoryPawn(BasePawn* pPawn, Scence* pScence)
{
	PlayerPawn* toDeletePawn = (PlayerPawn*)pPawn;

	//将PlayerPawn从Player的控制中除去。
	DetachControlFrom_PlayerCommander(toDeletePawn);

	//将Player中所有跟随的效果从FollowCommander中删除。
	DetachControlFrom_FollowCommander(toDeletePawn);

	//释放这个ControlItem链表上所有的ControlItem。
	pScence->FreeControlItemLink(P_Root_Control(toDeletePawn));

	//释放摄像机镜头。
	pScence->FreeCamera(P_PlayerCameraPositionControl(toDeletePawn));

	//回收Pawn对象。
	m_playerAllocator.Free(toDeletePawn);
}

void PlayerPawnCommandTemplate::AddControlTo_PlayerCommander(PlayerPawn * pPlayerPawn)
{
	//申请在PlayerCommander中增加一个新的控制槽，
	STATIC_P_PLAYER_COMMANDER(PlayerPawn)->
		ApplyForNewControlSlot(CommandId_Player_Control, pPlayerPawn);
}

void PlayerPawnCommandTemplate::AddControlTo_FollowCommander(PlayerPawn * pPlayerPawn)
{
	//让摄像机目标的世界平移跟随box，并且向上偏移5个单位（Y轴）。
	STATIC_P_FOLLOW_COMMANDER(PlayerPawn)->
		ApplyForNewControlSlot(
			P_PlayerCameraPositionControl(pPlayerPawn),
			COMMAND_FOLLOW_COMMANDER_TRANSLATION_FOLLOW,
			P_Root_Control(pPlayerPawn),
			0, 5, 0);
}
