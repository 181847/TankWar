#include "PlayerPawn.h"


//初始时静态成员。
PawnType							PlayerPawn::pawnType				= PAWN_TYPE_NONE;
//Player控制器类型。
PlayerControlType					PlayerPawn::m_playerControlType		= PLAYER_CONTROL_TYPE_NONE;
//PawnMaster。
PawnMaster *						PlayerPawn::pPawnMaster				= nullptr;
//玩家指令官。
PlayerCommander *					PlayerPawn::pPlayerCommander		= nullptr;
//骨骼指令官。
BoneCommander *						PlayerPawn::pBoneCommander			= nullptr;
//碰撞指令官。
CollideCommander *					PlayerPawn::pCollideCommander		= nullptr;
MyStaticAllocator<PlayerProperty>	PlayerPawn::m_propertyAllocator		(MAX_PLAYER_PAWN_NUM);
LinkedAllocator<PlayerPawn>			PlayerPawn::m_PlayerPawnAllocator	(MAX_PLAYER_PAWN_NUM);
//参考装甲车类型标记，这个类型标记用来实现点击鼠标左键然后生成装甲车的效果
PawnType							PlayerPawn::refCarType				= PAWN_TYPE_NONE;


//炮管的最大上扬角。
const float Radian_Pitch_Barrel_Max = XM_PIDIV4;
//炮管的最小上扬角。
const float Radian_Pitch_Barrel_Min = - XM_PI / 6;

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
	,	CollideCommander * pCollideCommander
)
{
	ASSERT(PlayerPawn::pPlayerCommander	== nullptr	&&	"不可重复注册PlayerCommander");
	ASSERT(PlayerPawn::pBoneCommander		== nullptr	&&	"不可重复注册BoneCommander");
	ASSERT(PlayerPawn::pCollideCommander	== nullptr	&&	"不可重复注册CollideCommander");

	//注册Master
	RegisterPawnMaster(pPawnMaster);

	//注册Commander。
	PlayerPawn::pPlayerCommander = pPlayerCommander;
	//添加一个新的玩家控制模式
	m_playerControlType = pPlayerCommander->AddCommandTemplate(
		std::make_unique<PlayerControlCommandTemplate>());

	PlayerPawn::pBoneCommander = pBoneCommander;
	PlayerPawn::pCollideCommander = pCollideCommander;
}

void PlayerPawn::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	ASSERT(PlayerPawn::pPawnMaster == nullptr && "不可重复注册PawnMaster");
	//注册一个CommandTempalte，并且获得一个pawnType。
	PlayerPawn::pawnType = pPawnMaster->AddCommandTemplate(
		std::make_unique<PlayerPawnCommandTemplate>());

	//记录PawnMaster
	PlayerPawn::pPawnMaster = pPawnMaster;
}

PlayerProperty * PlayerPawn::NewProperty()
{
	return PlayerPawn::m_propertyAllocator.Malloc();
}

ControlItem * PlayerPawn::RootControl()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT];
}

ControlItem * PlayerPawn::Battery()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_BATTERY];
}

ControlItem * PlayerPawn::MainBody()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_MAINBODY];
}

ControlItem * PlayerPawn::Barrel()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_BARREL];
}


//***************************************玩家生成模板*****************************
PlayerPawnCommandTemplate::PlayerPawnCommandTemplate()
{
}

PlayerPawnCommandTemplate::~PlayerPawnCommandTemplate()
{
}

BasePawn* PlayerPawnCommandTemplate::CreatePawn(PawnProperty* pProperty, Scence* pScence)
{
	PlayerPawn* newPawn = PlayerPawn::m_PlayerPawnAllocator.Malloc();

	newPawn->m_pawnType = PlayerPawn::pawnType;

	//记录属性。
	newPawn->m_pProperty = reinterpret_cast<PlayerProperty*>(pProperty);
	//从场景中创建摄像机，把摄像机存储到Player中。
	newPawn->m_pCamera = pScence->AppendCamera();

	newPawn->m_pCamera->Target->Translation = { 0.0f, 3.0f, 0.0f };

	//修改摄像机的局部坐标，避免再后来的更新中和摄像机的目标重合，引发异常。
	newPawn->m_pCamera->Pos->Translation = { 0.0f, 0.0f, -10.0f };
	//修改摄像机的世界矩阵中的平移，因为摄像机真正更新到世界至观察矩阵中的数据是世界矩阵的平移，
	//而摄像机目标和位置的世界矩阵都是单位矩阵，平移相同，第一次世界至观察矩阵时会引发一场，
	//这里临时修改一次世界平移坐标，因为在后来会从局部坐标中获得信息。
	newPawn->m_pCamera->Pos->World._41 = 1.0f;
	//显示摄像机目标的十字形定位器。
	newPawn->m_pCamera->Target->Show();
	//隐藏摄像机位置的定位器。
	newPawn->m_pCamera->Pos->Hide();

	//根节点控制器分配一个可控制的额网格物体。
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
		//指定玩家的一个渲染网格时shapeGeo中的box网格，
		//指定MeshGeometry、SubMesh、Material。
		pScence->NewControlItem("shapeGeo", "box", "box");
	//根控制器隐藏，只用来控制Pawn的位移，不控制旋转。
	newPawn->RootControl()->Hide();

	//为炮台分配一个ControlItem。
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_BATTERY] =
		pScence->NewControlItem("Tank_2", "Battery", "box");
	//炮台移动一点距离。
	newPawn->Battery()->MoveXYZ(0.0f, 1.032f, -0.036f);

	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_MAINBODY] =
		pScence->NewControlItem("Tank_2", "MainBody", "cylinder");
	//newPawn->MainBody()->MoveY(1.0f);

	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_BARREL] =
		pScence->NewControlItem("Tank_2", "Barrel", "grid");
	newPawn->Barrel()->MoveXYZ(0.0f, 0.26f, .88f);

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
		PlayerPawn::pPlayerCommander->NewPlayerControl(
			pPlayerPawn->m_playerControlType, pPlayerPawn);
}

void PlayerPawnCommandTemplate::AddBones(PlayerPawn * pPlayerPawn)
{
	//一个根控制器的骨骼。
	Bone* rootBone = 
		pPlayerPawn->m_arr_Bones[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
		PlayerPawn::pBoneCommander->NewBone(pPlayerPawn->RootControl());

	//摄像机拍摄目标的骨骼。
	Bone* cameraTarget
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET]
		= PlayerPawn::pBoneCommander->NewBone(pPlayerPawn->m_pCamera->Target);

	//摄像机位置的骨骼。
	Bone* cameraPos 
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_POS]
		= (PlayerPawn::pBoneCommander)->NewBone(pPlayerPawn->m_pCamera->Pos);

	//炮台骨骼。
	Bone* battery
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_BATTERY]
		= PlayerPawn::pBoneCommander->NewBone(pPlayerPawn->Battery());

	//车身骨骼。
	Bone* mainBody
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_MAINBODY]
		= PlayerPawn::pBoneCommander->NewBone(pPlayerPawn->MainBody());

	Bone* barrel
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_BARREL]
		= PlayerPawn::pBoneCommander->NewBone(pPlayerPawn->Barrel());

	//创建骨骼连接。

	//车身连接根骨骼。
	mainBody->LinkTo(rootBone);
	//炮台连接根骨骼，注意：炮台不受车身骨骼的影响，保持炮台一直指向摄像机的位置。
	battery->LinkTo(rootBone);
	//炮管连接炮台。
	barrel->LinkTo(battery);
	//摄像机目标连接炮台。
	cameraTarget->LinkTo(rootBone);
	//摄像机位置连接摄像机目标。
	cameraPos->LinkTo(cameraTarget);
}

void PlayerPawnCommandTemplate::DeletePlayerControl(PlayerPawn * pPlayerPawn)
{
	PlayerPawn* toDeletePawn = (PlayerPawn*)pPlayerPawn;
	PlayerPawn::pPlayerCommander->DeletePlayerControl(toDeletePawn->m_pPlayerControl);
	pPlayerPawn->m_pPlayerControl = nullptr;
}

void PlayerPawnCommandTemplate::DeleteBones(PlayerPawn * pPlayerPawn)
{
	//遍历骨骼数组，删除所有骨骼。
	for (int i = 0; i < _countof(pPlayerPawn->m_arr_Bones); ++i) 
	{
		PlayerPawn::pBoneCommander->DeleteBone(pPlayerPawn->m_arr_Bones[i]);
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



	//炮台水平旋转
	//pPlayerPawn->Battery()->RotateYaw(1.0f * dx);

	//摄像机水平旋转。
	pPlayerPawn->m_pCamera->Target->RotateYaw(1.0f * dx);
	//摄像机垂直旋转。
	pPlayerPawn->m_pCamera->Target->RotatePitch(1.0f * dy);
	//限制摄像机的镜头俯仰角角度，上至下90度范围内。
	pPlayerPawn->m_pCamera->Target->Rotation.x = MathHelper::Clamp(
		pPlayerPawn->m_pCamera->Target->Rotation.x,
		-XM_PI / 6,	
		XM_PIDIV2 - 0.01f);	

	//从摄像机的方向检查射线碰撞，
	//将炮管barrel的方向对准发生碰撞的方向。
	auto ray = PlayerPawn::pCollideCommander->NewRay(pPlayerPawn->m_pCamera->Target, 200.0f);
	PlayerPawn::pCollideCommander->CollideDetect(ray, COLLIDE_TYPE_ALL);
	if (ray->Result.CollideHappended)
	{
		//旋转定位炮台的方向，使得炮台的水平方向尝试对准发生碰撞的位置。
		//注意不是直接指向，而是慢慢的旋转炮台到指定的位置。
		rotateBattery(pPlayerPawn, ray->Result.CollideLocation, gt);
		//垂直旋转炮管，使得炮管对阵发生碰撞的位置。
		//注意不是直接指向，而是慢慢的旋转炮管到指定的位置。
		rotateBarrel(pPlayerPawn, ray->Result.CollideLocation, gt);
	}

	PlayerPawn::pCollideCommander->DeleteRay(ray);
	
}

void PlayerControlCommandTemplate::HitKey_W(BasePawn * pPawn, const GameTimer& gt)
{
	//前进
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	//使用车身的旋转。
	XMFLOAT3 rotation = pPlayerPawn->MainBody()->Rotation;

	pPlayerPawn->RootControl()->MoveX(sinf(rotation.y) * pPlayerPawn->m_pProperty->MoveSpeed * gt.DeltaTime());
	pPlayerPawn->RootControl()->MoveZ(cosf(rotation.y) * pPlayerPawn->m_pProperty->MoveSpeed * gt.DeltaTime());
}

void PlayerControlCommandTemplate::HitKey_A(BasePawn * pPawn, const GameTimer& gt)
{
	//左转
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->MainBody()->RotateYaw(-1.0f * pPlayerPawn->m_pProperty->RotationSpeed * gt.DeltaTime());
}

void PlayerControlCommandTemplate::HitKey_S(BasePawn * pPawn, const GameTimer& gt)
{
	//后退
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	//使用车身的旋转。
	XMFLOAT3 rotation = pPlayerPawn->MainBody()->Rotation;

	pPlayerPawn->RootControl()->MoveX(
		-1.0f 
		* sinf(rotation.y) 
		* pPlayerPawn->		m_pProperty->	MoveSpeed 
		* gt.DeltaTime());

	pPlayerPawn->RootControl()->MoveZ(
		-1.0f 
		* cosf(rotation.y) 
		* pPlayerPawn ->	m_pProperty->	MoveSpeed 
		* gt.DeltaTime());
}

void PlayerControlCommandTemplate::HitKey_D(BasePawn * pPawn, const GameTimer& gt)
{
	//右转
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->MainBody()->RotateYaw(
		1.0f 
		* pPlayerPawn->		m_pProperty->	RotationSpeed 
		* gt.DeltaTime());
}

void PlayerControlCommandTemplate::HitMouseButton_Left(BasePawn * pPawn, const GameTimer & gt)
{
	ASSERT(PlayerPawn::refCarType && "未初始化装甲车的类型标记，无法创建装甲车。");

	//使用装甲车的默认属性进行创建。
	PlayerPawn::pPawnMaster->CreatePawn(PlayerPawn::refCarType, nullptr);
}

void PlayerControlCommandTemplate::HitMouseButton_Right(BasePawn * pPawn, const GameTimer & gt)
{
	//从摄像机发出射线，如果撞击到某个物体，销毁它。

	
	//使用摄像机的目标位置以及方向作为射线的位置和起始方向。
	auto pPlayer = reinterpret_cast<PlayerPawn*>(pPawn);
	/*
	//首先申请一个射线碰撞单元。
	auto rayDetect = PlayerPawn::pCollideCommander->NewRay(
		pPlayer->m_pCamera->Target,	//射线的起点、以及方向使用摄像机目标的世界变换矩阵。
		MAX_RAY_LENGTH);			//射线的长度定义。

	//对类型1的碰撞体进行碰撞检测。
	PlayerPawn::pCollideCommander->CollideDetect(rayDetect, COLLIDE_TYPE_BOX_1);

	if (rayDetect->Result.CollideHappended)
	{
		//删除碰撞发生的Pawn。
		PlayerPawn::pPawnMaster->DestroyPawn(
			PAWN_TYPE_NONE, 
			rayDetect-> Result.pCollideBox-> pPawn);
	}

	//回收射线碰撞单元。
	PlayerPawn::pCollideCommander->DeleteRay(rayDetect);
	*/


	auto pShellProperty = gShellPropertyAllocator.Malloc();
	pShellProperty->CurrState = ShellState::Move;
	pShellProperty->DeltaDist = 0.0f;	//这个初始的帧运动距离只是暂时的，之后每一帧都会被Shell自动更新。
	pShellProperty->StartPos = pPlayer->m_pCamera->Target->World;
	pShellProperty->RestDist = 200.0f;
	pShellProperty->MoveSpeed = 200.0f;

	//记录一个创建弹药Pawn的指令。
	gPawnMaster->CreatePawn(gShellPawnType, pShellProperty);
}

void PlayerControlCommandTemplate::PressMouseButton_Left(BasePawn * pPawn, const GameTimer& gt)
{
	//发射子弹。
}

void PlayerControlCommandTemplate::PressMouseButton_Right(BasePawn * pPawn, const GameTimer& gt)
{
	//发射炮弹。
}

void PlayerControlCommandTemplate::rotateBattery(
	PlayerPawn * pPlayerPawn,
	XMFLOAT3 targetFloat3,
	const GameTimer& gt)
{
	float rx, ry;
	auto world = XMLoadFloat4x4(&pPlayerPawn->Battery()->World);
	auto target = XMVectorSet(
		targetFloat3.x,
		targetFloat3.y, 
		targetFloat3.z,
		1.0f);

	//找到需要旋转的额水平角度，注意只需要绕局部y轴（水平）的旋转，
	//这里将needRx设为false，表示不需要rx，这能够节省一点开根号的计算量。
	OffsetInLocal(world, target, false, rx, ry);
	
	//将没有用的rx用来存储旋转弧度。
	rx = pPlayerPawn->m_pProperty->RotationSpeed * gt.DeltaTime();

	if (FloatEqual(ry, 0.0f, FLT_EPSILON))
	{
		//什么也不做。
	}
	else if (ry > XM_PIDIV2)
	{
		//大于于pi/2，左转最快。
		pPlayerPawn->Battery()->RotateYaw(
			( - rx)
		);
	}
	else
	{
		//小于于pi/2，右转最快。
		pPlayerPawn->Battery()->RotateYaw(
			rx
		);
	}
}

void PlayerControlCommandTemplate::rotateBarrel(PlayerPawn * pPlayerPawn, XMFLOAT3 targetFloat3, const GameTimer & gt)
{
	float rx, ry;
	auto world = XMLoadFloat4x4(&pPlayerPawn->Barrel()->World);
	auto target = XMVectorSet(
		targetFloat3.x,
		targetFloat3.y,
		targetFloat3.z,
		1.0f);

	OffsetInLocal(world, target, true, rx, ry);

	//将没有用的ry用来存储旋转弧度。
	ry = pPlayerPawn->m_pProperty->RotationSpeed * gt.DeltaTime();

	if (FloatEqual(rx, 0.0f, FLT_EPSILON))
	{
		//什么也不做。
	}
	else
	{
		//直接旋转。
		pPlayerPawn->Barrel()->RotatePitch(ry);


		//限制炮管的俯仰角角度，上至下90度范围内。
		pPlayerPawn->Barrel()->Rotation.x = MathHelper::Clamp(
			pPlayerPawn->Barrel()->Rotation.x,
			Radian_Pitch_Barrel_Min,
			Radian_Pitch_Barrel_Max);
	}
}
