#include "PlayerPawn.h"



PlayerPawn::PlayerPawn()
{
}


PlayerPawn::~PlayerPawn()
{
}

void PlayerPawn::RegisterAll(
	PawnMaster * pPawnMaster, 
	PlayerCommander * pPlayerCommander, 
	BoneCommander * pBoneCommander
	//,	CollideCommander * pCollideCommander
)
{
	ASSERT(PlayerPawn::m_pPlayerCommander	== nullptr	&&	"不可重复注册PlayerCommander");
	ASSERT(PlayerPawn::m_pBoneCommander		== nullptr	&&	"不可重复注册BoneCommander");
	//ASSERT(PlayerPawn::m_pCollideCommander	== nullptr	&&	"不可重复注册CollideCommander");

	//注册Master
	RegisterPawnMaster(pPawnMaster);

	//注册Commander。
	m_pPlayerCommander = pPlayerCommander;
	//添加一个新的玩家控制模式
	m_playerControlType = m_pPlayerCommander->AddCommandTemplate(
		new PlayerControlCommandTemplate());

	m_pBoneCommander = pBoneCommander;
	//m_pCollideCommander = pCollideCommander;
}

void PlayerPawn::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	ASSERT(PlayerPawn::m_pPawnMaster == nullptr && "不可重复注册PawnMaster");
	//注册一个CommandTempalte，并且获得一个pawnType。
	PlayerPawn::m_pawnType = pPawnMaster->AddCommandTemplate(
		new PlayerPawnCommandTemplate());
}

ControlItem *& PlayerPawn::RootControl()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT];
}

//***************************************玩家生成模板*****************************
BasePawn* PlayerPawnCommandTemplate::CreatePawn(PawnProperty* pProperty, Scence* pScence)
{
	PlayerPawn* newPawn = PlayerPawn::m_PlayerPawnAllocator.Malloc();
	//从场景中创建摄像机，把摄像机存储到Player中。
	newPawn->m_pCamera = pScence->AppendCamera();

	newPawn->RootControl() =
		//指定玩家的一个渲染网格时shapeGeo中的box网格，
		//指定MeshGeometry、SubMesh、Material。
		pScence->NewControlItem("shapeGeo", "box", "box");

	//添加玩家控制。
	AddPlayerControl(newPawn);

	//添加骨骼关联。
	AddBones(newPawn);

	//添加碰撞事件，并指派一个触发事件。
	//TODO PlayerPawn的碰撞体添加。
	//AddCollideItems(newPawn);
}

void PlayerPawnCommandTemplate::DestoryPawn(BasePawn* pPawn, Scence* pScence)
{
	PlayerPawn* toDeletePawn = (PlayerPawn*)pPawn;

	//将PlayerPawn从Player的控制中除去。
	DeletePlayerControl(toDeletePawn);

	//删除骨骼关联。
	DeleteBones(toDeletePawn);

	//删除碰撞碰撞形体。
	//TODO PlayerPawn的碰撞体删除。
	//DeleteCollideItems();

	//释放这个ControlItem链表上所有的ControlItem。
	pScence->DeleteControlItem(toDeletePawn->RootControl());

	//释放摄像机镜头。
	pScence->DeleteCamera(toDeletePawn->m_pCamera);

	//回收Pawn对象。
	m_playerAllocator.Free(toDeletePawn);
}

void PlayerPawnCommandTemplate::AddPlayerControl(PlayerPawn * pPlayerPawn)
{
	ASSERT(pPlayerPawn->m_pPlayerControl == nullptr && "不能对PlayerPawn重复添加玩家控制");
	pPlayerPawn->m_pPlayerControl =
		PlayerPawn::m_pPlayerCommander->NewPlayerControl(
			pPlayerPawn->m_playerControlType, pPlayerPawn);
}

void PlayerPawnCommandTemplate::AddBones(PlayerPawn * pPlayerPawn)
{
	//一个渲染网格的骨骼。
	Bone* rootBone = 
		pPlayerPawn->m_arr_Bones[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
		PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT]);

	//摄像机位置的骨骼。
	Bone* cameraPos = 
		pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_POS] =
		PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->m_pCamera->Pos());

	//摄像机拍摄目标的骨骼。
	Bone* cameraTarget = 
		pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET] =
		PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->m_pCamera->Target());

	//创建骨骼连接。
	cameraTarget->LinkTo(rootBone);
	cameraPos->LinkTo(cameraTarget);
}

void PlayerPawnCommandTemplate::DeletePlayerControl(PlayerPawn * pPlayerPawn)
{
	PlayerPawn::m_pPlayerCommander->DeletePlayerControl(pPlayerPawn->m_pPlayerControl);
	pPlayerPawn->m_pPlayerControl = nullptr;
}

void PlayerPawnCommandTemplate::DeleteBones(PlayerPawn * pPlayerPawn)
{
	//遍历骨骼数组，删除所有骨骼。
	for (int i = 0; i < _countof(pPlayerPawn->m_arr_Bones); ++i) 
	{
		PlayerPawn::m_pBoneCommander->DeleteBone(pPlayerPawn->m_arr_Bones[i]);
		pPlayerPawn->m_arr_Bones[i] = nullptr;
	}
}


//******************玩家控制模板*****************************************
void PlayerControlCommandTemplate::MouseMove(BasePawn * pPawn, float lastX, float lastY, float currX, float currY, WPARAM btnState)
{
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	float dx = currX - lastX;
	float dy = currY - lastY;

	//摄像机水平旋转。
	pPlayerPawn->m_pCamera->Target->RotateYaw(dx);
	//摄像机垂直旋转。
	pPlayerPawn->m_pCamera->Target->RotatePitch(dy);

	//限制摄像机的镜头俯仰角角度。
	pPlayerPawn->m_pCamera->Target->Rotation.y = 
		MathHelper::Clamp(
			pPlayerPawn->m_pCamera->Target->Rotation.y, 
			-0.333f * XM_PIDIV2,	//限制摄像机的镜头俯仰角不低于1/6 PI，即-30度。
			XM_PIDIV2 - 0.001f);	//限制写相机的镜头俯仰角不高于 PI/2 - 0.001，即小于90度。
	
}

void PlayerControlCommandTemplate::HitKey_W(BasePawn * pPawn)
{
	//前进
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->MoveX(0.1f * m_pProperty->MoveSpeed);
}

void PlayerControlCommandTemplate::HitKey_A(BasePawn * pPawn)
{
	//左转
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->RotateYaw(-0.1f * m_pProperty->RotateSpeed);
}

void PlayerControlCommandTemplate::HitKey_S(BasePawn * pPawn)
{
	//后退
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->MoveX(-0.1f * m_pProperty->MoveSpeed);
}

void PlayerControlCommandTemplate::HitKey_D(BasePawn * pPawn)
{
	//右转
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->RotateYaw(0.1f * m_pProperty->RotateSpeed);
}

void PlayerControlCommandTemplate::PressMouseButton_Left(BasePawn * pPawn)
{
	//发射子弹。
}

void PlayerControlCommandTemplate::PressMouseButton_Right(BasePawn * pPawn)
{
	//发射炮弹。
}
