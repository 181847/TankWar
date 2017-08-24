#include "PlayerPawn.h"


//初始时静态成员。
PawnType			PlayerPawn::m_pawnType = PAWN_TYPE_NONE;
//Player控制器类型。
PlayerControlType	PlayerPawn::m_playerControlType = PLAYER_CONTROL_TYPE_NONE;
//PawnMaster。
PawnMaster *		PlayerPawn::m_pPawnMaster = nullptr;
//玩家指令官。
PlayerCommander *	PlayerPawn::m_pPlayerCommander = nullptr;
//骨骼指令官。
BoneCommander *		PlayerPawn::m_pBoneCommander = nullptr;
//碰撞指令官。
//CollideCommander *	PlayerPawn::m_pCollideCommander		= nullptr;
MyStaticAllocator<PlayerProperty>	PlayerPawn::m_propertyAllocator(MAX_PLAYER_PAWN_NUM);
MyStaticAllocator<PlayerPawn>		PlayerPawn::m_PlayerPawnAllocator(MAX_PLAYER_PAWN_NUM);

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
		std::make_unique<PlayerControlCommandTemplate>());

	m_pBoneCommander = pBoneCommander;
	//m_pCollideCommander = pCollideCommander;
}

void PlayerPawn::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	ASSERT(PlayerPawn::m_pPawnMaster == nullptr && "不可重复注册PawnMaster");
	//注册一个CommandTempalte，并且获得一个pawnType。
	PlayerPawn::m_pawnType = pPawnMaster->AddCommandTemplate(
		std::make_unique<PlayerPawnCommandTemplate>());
}

PlayerProperty * PlayerPawn::NewProperty()
{
	return PlayerPawn::m_propertyAllocator.Malloc();
}

ControlItem * PlayerPawn::RootControl()
{
	//返回控制器数组。
	return m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT];
}

//***************************************玩家生成模板*****************************
BasePawn* PlayerPawnCommandTemplate::CreatePawn(PawnProperty* pProperty, Scence* pScence)
{
	PlayerPawn* newPawn = PlayerPawn::m_PlayerPawnAllocator.Malloc();
	newPawn->m_pProperty = reinterpret_cast<PlayerProperty*>(pProperty);
	//从场景中创建摄像机，把摄像机存储到Player中。
	newPawn->m_pCamera = pScence->AppendCamera();

	//修改摄像机的局部坐标，避免再后来的更新中和摄像机的目标重合，引发异常。
	newPawn->m_pCamera->Pos->Translation = { 3.0f, 3.0f, 3.0f };
	//修改摄像机的世界矩阵中的平移，因为摄像机真正更新到世界至观察矩阵中的数据是世界矩阵的平移，
	//而摄像机目标和位置的世界矩阵都是单位矩阵，平移相同，第一次世界至观察矩阵时会引发一场，
	//这里临时修改一次世界平移坐标，因为在后来会从局部坐标中获得信息。
	newPawn->m_pCamera->Pos->World._41 = 1.0f;

	//根节点控制器分配一个可控制的额网格物体。
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
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
	return newPawn;
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

	//释放这个ControlItem数组上所有指针。
	pScence->DeleteControlItem(toDeletePawn->RootControl());

	//释放摄像机镜头。
	pScence->DeleteCamera(toDeletePawn->m_pCamera);

	//回收Pawn对象。
	PlayerPawn::m_PlayerPawnAllocator.Free(toDeletePawn);
}

void PlayerPawnCommandTemplate::AddPlayerControl(PlayerPawn * pPlayerPawn)
{
	//ASSERT(pPlayerPawn->m_pPlayerControl == nullptr && "不能对PlayerPawn重复添加玩家控制");
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

	//摄像机拍摄目标的骨骼。
	Bone* cameraTarget
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET]
		= PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->m_pCamera->Target);

	//摄像机位置的骨骼。
	Bone* cameraPos = 
		pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_POS] =
		(PlayerPawn::m_pBoneCommander)->NewBone(pPlayerPawn->m_pCamera->Pos);


	//创建骨骼连接。
	cameraTarget->LinkTo(rootBone);
	cameraPos->LinkTo(cameraTarget);
}

void PlayerPawnCommandTemplate::DeletePlayerControl(PlayerPawn * pPlayerPawn)
{
	PlayerPawn* toDeletePawn = (PlayerPawn*)pPlayerPawn;
	PlayerPawn::m_pPlayerCommander->DeletePlayerControl(toDeletePawn->m_pPlayerControl);
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
void PlayerControlCommandTemplate::MouseMove(BasePawn* pPawn, MouseState mouseState, const GameTimer& gt)
{
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	float dt = gt.DeltaTime();
	float dx = 0.01f * static_cast<float>(mouseState.CurrMousePos.x - mouseState.LastMousePos.x);
	float dy = 0.01f * static_cast<float>(mouseState.CurrMousePos.y - mouseState.LastMousePos.y);



	//摄像机水平旋转。
	pPlayerPawn->m_pCamera->Target->RotateYaw(1.0f * dx);

	//摄像机垂直旋转。
	pPlayerPawn->m_pCamera->Target->RotatePitch(1.0f * dy);
	//限制摄像机的镜头俯仰角角度。
	pPlayerPawn->m_pCamera->Target->Rotation.x = MathHelper::Clamp(
	pPlayerPawn->m_pCamera->Target->Rotation.x,
	0.0f,	//限制摄像机的镜头俯仰角不低于1/6 PI，即-30度。
	XM_PIDIV2);	//限制写相机的镜头俯仰角不高于 PI/2 - 0.001，即小于90度。

	//pPlayerPawn->m_pCamera->Target->Rotation.x += 0.01f;
	//pPlayerPawn->m_pCamera->Target->NumFramesDirty = 3;
	
	
}

void PlayerControlCommandTemplate::HitKey_W(BasePawn * pPawn, const GameTimer& gt)
{
	//前进
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->MoveX(1.0f * pPlayerPawn->m_pProperty->MoveSpeed * gt.DeltaTime());
}

void PlayerControlCommandTemplate::HitKey_A(BasePawn * pPawn, const GameTimer& gt)
{
	//左转
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->RotateYaw(-1.0f * pPlayerPawn->m_pProperty->RotationSpeed * gt.DeltaTime());
}

void PlayerControlCommandTemplate::HitKey_S(BasePawn * pPawn, const GameTimer& gt)
{
	//后退
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	float dt = gt.DeltaTime();
	pPlayerPawn->RootControl()->MoveX(-1.0f * pPlayerPawn->m_pProperty->MoveSpeed * dt);
}

void PlayerControlCommandTemplate::HitKey_D(BasePawn * pPawn, const GameTimer& gt)
{
	//右转
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->RotateYaw(1.0f * pPlayerPawn->m_pProperty->RotationSpeed * gt.DeltaTime());
}

void PlayerControlCommandTemplate::PressMouseButton_Left(BasePawn * pPawn, const GameTimer& gt)
{
	//发射子弹。
}

void PlayerControlCommandTemplate::PressMouseButton_Right(BasePawn * pPawn, const GameTimer& gt)
{
	//发射炮弹。
}
