#include "PlayerPawn.h"

LinkedAllocator<PlayerPawn>			PlayerPawn::PawnAllocator(MAX_PLAYER_PAWN_NUM);


//炮管的最大上扬角。
const float Radian_Pitch_Barrel_Max = XM_PIDIV4;
//炮管的最小上扬角。
const float Radian_Pitch_Barrel_Min = - XM_PI / 12;

PlayerPawn::PlayerPawn()
{
}

PlayerPawn::~PlayerPawn()
{
}

void PlayerPawn::Register()
{
	//注册PawnMaster
	gPlayerPawnType = gPawnMaster->AddCommandTemplate(
		std::make_unique<PlayerPawnCommandTemplate>());

	//添加一个新的玩家控制模式
	gPlayerControlType = gPlayerCommander->AddCommandTemplate(
		std::make_unique<PlayerControlCommandTemplate>());

	//添加AI控制模式，这个AI控制主要是用来执行一些每一帧执行的动作，
	//比如不停的瞄准可碰撞物体。
	gPlayerAIControlType = gAICommander->AddAITemplate(
		std::make_unique<PlayerAITemplate>());
}

PlayerProperty * PlayerPawn::NewProperty()
{
	return gPlayerPropertyAllocator.Malloc();
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
	PlayerPawn* newPawn = PlayerPawn::PawnAllocator.Malloc();

	newPawn->m_pawnType = gPlayerPawnType;

	//记录属性。
	newPawn->m_pProperty = reinterpret_cast<PlayerProperty*>(pProperty);
	//从场景中创建摄像机，把摄像机存储到Player中。
	newPawn->m_pCamera = pScence->AppendCamera();

	newPawn->m_pCamera->Target->Translation = { 0.0f, 5.0f, 0.0f };

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
		pScence->NewControlItem("Tank_2", "Barrel", "cylinder");
	newPawn->Barrel()->MoveXYZ(0.0f, 0.26f, .88f);

	//添加玩家控制。
	AddPlayerControl(newPawn);

	//添加骨骼关联。
	AddBones(newPawn);

	//添加AI控制。
	newPawn->m_pAIUnit = gAICommander->NewAIUnit(gPlayerAIControlType, newPawn);
	//初始化AI状态。
	newPawn->m_pAIUnit->CurrState = STORY_FRAGMENT_PLAYER_DEFAULT;

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

	//删除AI控制。
	gAICommander->DeleteAIUnit(toDeletePawn->m_pAIUnit);

	//回收Pawn对象。
	PlayerPawn::PawnAllocator.Free(toDeletePawn);

}

void PlayerPawnCommandTemplate::AddPlayerControl(PlayerPawn * pPlayerPawn)
{
	//ASSERT(pPlayerPawn->m_pPlayerControl == nullptr && "不能对PlayerPawn重复添加玩家控制");
	pPlayerPawn->m_pPlayerControl =
		gPlayerCommander->NewPlayerControl(
			gPlayerControlType, pPlayerPawn);
}

void PlayerPawnCommandTemplate::AddBones(PlayerPawn * pPlayerPawn)
{
	//一个根控制器的骨骼。
	Bone* rootBone = 
		pPlayerPawn->m_arr_Bones[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
		gBoneCommander->NewBone(pPlayerPawn->RootControl());

	//摄像机拍摄目标的骨骼。
	Bone* cameraTarget
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET]
		= gBoneCommander->NewBone(pPlayerPawn->m_pCamera->Target);

	//摄像机位置的骨骼。
	Bone* cameraPos 
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_POS]
		= (gBoneCommander)->NewBone(pPlayerPawn->m_pCamera->Pos);

	//炮台骨骼。
	Bone* battery
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_BATTERY]
		= gBoneCommander->NewBone(pPlayerPawn->Battery());

	//车身骨骼。
	Bone* mainBody
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_MAINBODY]
		= gBoneCommander->NewBone(pPlayerPawn->MainBody());

	Bone* barrel
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_BARREL]
		= gBoneCommander->NewBone(pPlayerPawn->Barrel());

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
	gPlayerCommander->DeletePlayerControl(toDeletePawn->m_pPlayerControl);
	pPlayerPawn->m_pPlayerControl = nullptr;
}

void PlayerPawnCommandTemplate::DeleteBones(PlayerPawn * pPlayerPawn)
{
	//遍历骨骼数组，删除所有骨骼。
	for (int i = 0; i < _countof(pPlayerPawn->m_arr_Bones); ++i) 
	{
		gBoneCommander->DeleteBone(pPlayerPawn->m_arr_Bones[i]);
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
		-XM_PIDIV2,	
		XM_PI / 6 - 0.01f);	

	
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
	ASSERT(gArmoredCarPawnType && "未初始化装甲车的类型标记，无法创建装甲车。");

	//使用装甲车的默认属性进行创建。
	gPawnMaster->CreatePawn(gArmoredCarPawnType, nullptr);
}

void PlayerControlCommandTemplate::HitMouseButton_Right(BasePawn * pPawn, const GameTimer & gt)
{
	//从摄像机发出射线，如果撞击到某个物体，销毁它。

	
	//使用摄像机的目标位置以及方向作为射线的位置和起始方向。
	auto pPlayer = reinterpret_cast<PlayerPawn*>(pPawn);
	/*
	//首先申请一个射线碰撞单元。
	auto rayDetect = gCollideCommander->NewRay(
		pPlayer->m_pCamera->Target,	//射线的起点、以及方向使用摄像机目标的世界变换矩阵。
		MAX_RAY_LENGTH);			//射线的长度定义。

	//对类型1的碰撞体进行碰撞检测。
	gCollideCommander->CollideDetect(rayDetect, COLLIDE_TYPE_BOX_1);

	if (rayDetect->Result.CollideHappended)
	{
		//删除碰撞发生的Pawn。
		PlayerPawn::pPawnMaster->DestroyPawn(
			PAWN_TYPE_NONE, 
			rayDetect-> Result.pCollideBox-> pPawn);
	}

	//回收射线碰撞单元。
	gCollideCommander->DeleteRay(rayDetect);
	*/


	auto pShellProperty = gShellPropertyAllocator.Malloc();
	pShellProperty->CurrState = ShellState::Move;
	pShellProperty->DeltaDist = 0.0f;	//这个初始的帧运动距离只是暂时的，之后每一帧都会被Shell自动更新。
	pShellProperty->StartPos = pPlayer->Barrel()->World;
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

PlayerAITemplate::PlayerAITemplate()
{
	StoryFragment singleFragment;
	singleFragment.State = STORY_FRAGMENT_PLAYER_DEFAULT;
	singleFragment.Posibility = 1.0f;
	singleFragment.ConsistTime = 200.0f;
	singleFragment.NextState = STORY_FRAGMENT_NEXT_NONE;

	StoryBoard.push_back(singleFragment);
}

void PlayerAITemplate::Runing(
	BasePawn * pPawn,	AIStatue state, 
	float consumedTime, const GameTimer & gt)
{
	auto pPlayer = reinterpret_cast<PlayerPawn * >(pPawn);

	switch (state)
	{
	case STORY_FRAGMENT_PLAYER_DEFAULT:
		aim(pPlayer, gt);
		

		break;

	default:
		ASSERT(false && "Player的AI状态非法，无法执行操作。");
	}
}

void PlayerAITemplate::aim(PlayerPawn * pPlayer, const GameTimer& gt)
{
	//从摄像机的方向检查射线碰撞，
	//将炮管barrel的方向对准发生碰撞的方向。
	auto ray = gCollideCommander->NewRay(pPlayer->m_pCamera->Target, 10e7f);
	gCollideCommander->CollideDetect(ray, COLLIDE_TYPE_ALL);

	XMFLOAT3 dummyLocation = { 0.0f, 0.0f, 0.0f };

	if (ray->Result.CollideHappended)
	{
		//旋转定位炮台的方向，使得炮台的水平方向尝试对准发生碰撞的位置。
		//注意不是直接指向，而是慢慢的旋转炮台到指定的位置。
		rotateBattery(pPlayer, ray->Result.CollideLocation, gt);
		//垂直旋转炮管，使得炮管对阵发生碰撞的位置。
		//注意不是直接指向，而是慢慢的旋转炮管到指定的位置。
		rotateBarrel(pPlayer, ray->Result.CollideLocation, gt);
	}
	else
	{
		//没有发生碰撞，将物体的位置强制归零。
		//pPlayer->Barrel()->ClearRotation();
		//pPlayer->Battery()->ClearRotation();
	}

	gCollideCommander->DeleteRay(ray);
}



void PlayerAITemplate::rotateBattery(
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

	if (FloatEqual(ry, 0.0f, 0.02f))
	{
		//什么也不做。
	}
	else if (ry > 0.0f)
	{
		//左转。
		pPlayerPawn->Battery()->RotateYaw(
			rx
		);
	}
	else
	{
		//右转。
		pPlayerPawn->Battery()->RotateYaw(
			(-rx)
		);
	}
}

void PlayerAITemplate::rotateBarrel(PlayerPawn * pPlayerPawn, XMFLOAT3 targetFloat3, const GameTimer & gt)
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

	if (FloatEqual(rx, 0.0f, 0.02f))
	{
		//什么也不做。
	}
	else if (rx > 0.0f)
	{
		//向下旋转
		pPlayerPawn->Barrel()->RotatePitch(ry);
	}
	else
	{
		//向上旋转
		pPlayerPawn->Barrel()->RotatePitch(-ry);
	}

	//限制炮管的俯仰角角度
	pPlayerPawn->Barrel()->Rotation.x = MathHelper::Clamp(
		pPlayerPawn->Barrel()->Rotation.x,
		Radian_Pitch_Barrel_Min,
		Radian_Pitch_Barrel_Max);
}
