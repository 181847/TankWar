#include "PlayerPawn.h"

LinkedAllocator<PlayerPawn>			PlayerPawn::PawnAllocator(MAX_PLAYER_PAWN_NUM);


//�ڹܵ��������ǡ�
const float Radian_Pitch_Barrel_Max = XM_PIDIV4;
//�ڹܵ���С����ǡ�
const float Radian_Pitch_Barrel_Min = - XM_PI / 12;

PlayerPawn::PlayerPawn()
{
}

PlayerPawn::~PlayerPawn()
{
}

void PlayerPawn::Register()
{
	//ע��PawnMaster
	gPlayerPawnType = gPawnMaster->AddCommandTemplate(
		std::make_unique<PlayerPawnCommandTemplate>());

	//���һ���µ���ҿ���ģʽ
	gPlayerControlType = gPlayerCommander->AddCommandTemplate(
		std::make_unique<PlayerControlCommandTemplate>());

	//���AI����ģʽ�����AI������Ҫ������ִ��һЩÿһִ֡�еĶ�����
	//���粻ͣ����׼����ײ���塣
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


//***************************************�������ģ��*****************************
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

	//��¼���ԡ�
	newPawn->m_pProperty = reinterpret_cast<PlayerProperty*>(pProperty);
	//�ӳ����д������������������洢��Player�С�
	newPawn->m_pCamera = pScence->AppendCamera();

	newPawn->m_pCamera->Target->Translation = { 0.0f, 5.0f, 0.0f };

	//�޸�������ľֲ����꣬�����ٺ����ĸ����к��������Ŀ���غϣ������쳣��
	newPawn->m_pCamera->Pos->Translation = { 0.0f, 0.0f, -10.0f };
	//�޸����������������е�ƽ�ƣ���Ϊ������������µ��������۲�����е���������������ƽ�ƣ�
	//�������Ŀ���λ�õ���������ǵ�λ����ƽ����ͬ����һ���������۲����ʱ������һ����
	//������ʱ�޸�һ������ƽ�����꣬��Ϊ�ں�����Ӿֲ������л����Ϣ��
	newPawn->m_pCamera->Pos->World._41 = 1.0f;
	//��ʾ�����Ŀ���ʮ���ζ�λ����
	newPawn->m_pCamera->Target->Show();
	//���������λ�õĶ�λ����
	newPawn->m_pCamera->Pos->Hide();

	//���ڵ����������һ���ɿ��ƵĶ��������塣
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
		//ָ����ҵ�һ����Ⱦ����ʱshapeGeo�е�box����
		//ָ��MeshGeometry��SubMesh��Material��
		pScence->NewControlItem("shapeGeo", "box", "box");
	//�����������أ�ֻ��������Pawn��λ�ƣ���������ת��
	newPawn->RootControl()->Hide();

	//Ϊ��̨����һ��ControlItem��
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_BATTERY] =
		pScence->NewControlItem("Tank_2", "Battery", "box");
	//��̨�ƶ�һ����롣
	newPawn->Battery()->MoveXYZ(0.0f, 1.032f, -0.036f);

	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_MAINBODY] =
		pScence->NewControlItem("Tank_2", "MainBody", "cylinder");
	//newPawn->MainBody()->MoveY(1.0f);

	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_BARREL] =
		pScence->NewControlItem("Tank_2", "Barrel", "cylinder");
	newPawn->Barrel()->MoveXYZ(0.0f, 0.26f, .88f);

	//�����ҿ��ơ�
	AddPlayerControl(newPawn);

	//��ӹ���������
	AddBones(newPawn);

	//���AI���ơ�
	newPawn->m_pAIUnit = gAICommander->NewAIUnit(gPlayerAIControlType, newPawn);
	//��ʼ��AI״̬��
	newPawn->m_pAIUnit->CurrState = STORY_FRAGMENT_PLAYER_DEFAULT;

	//�����ײ�¼�����ָ��һ�������¼���
	//TODO PlayerPawn����ײ����ӡ�
	//AddCollideItems(newPawn);
	return newPawn;
}

void PlayerPawnCommandTemplate::DestoryPawn(BasePawn* pPawn, Scence* pScence)
{
	PlayerPawn* toDeletePawn = (PlayerPawn*)pPawn;

	//��PlayerPawn��Player�Ŀ����г�ȥ��
	DeletePlayerControl(toDeletePawn);

	//ɾ������������
	DeleteBones(toDeletePawn);

	//ɾ����ײ��ײ���塣
	//TODO PlayerPawn����ײ��ɾ����
	//DeleteCollideItems();

	//�ͷ����ControlItem����������ָ�롣
	pScence->DeleteControlItem(toDeletePawn->RootControl());

	//�ͷ��������ͷ��
	pScence->DeleteCamera(toDeletePawn->m_pCamera);

	//ɾ��AI���ơ�
	gAICommander->DeleteAIUnit(toDeletePawn->m_pAIUnit);

	//����Pawn����
	PlayerPawn::PawnAllocator.Free(toDeletePawn);

}

void PlayerPawnCommandTemplate::AddPlayerControl(PlayerPawn * pPlayerPawn)
{
	//ASSERT(pPlayerPawn->m_pPlayerControl == nullptr && "���ܶ�PlayerPawn�ظ������ҿ���");
	pPlayerPawn->m_pPlayerControl =
		gPlayerCommander->NewPlayerControl(
			gPlayerControlType, pPlayerPawn);
}

void PlayerPawnCommandTemplate::AddBones(PlayerPawn * pPlayerPawn)
{
	//һ�����������Ĺ�����
	Bone* rootBone = 
		pPlayerPawn->m_arr_Bones[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
		gBoneCommander->NewBone(pPlayerPawn->RootControl());

	//���������Ŀ��Ĺ�����
	Bone* cameraTarget
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET]
		= gBoneCommander->NewBone(pPlayerPawn->m_pCamera->Target);

	//�����λ�õĹ�����
	Bone* cameraPos 
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_POS]
		= (gBoneCommander)->NewBone(pPlayerPawn->m_pCamera->Pos);

	//��̨������
	Bone* battery
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_BATTERY]
		= gBoneCommander->NewBone(pPlayerPawn->Battery());

	//���������
	Bone* mainBody
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_MAINBODY]
		= gBoneCommander->NewBone(pPlayerPawn->MainBody());

	Bone* barrel
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_BARREL]
		= gBoneCommander->NewBone(pPlayerPawn->Barrel());

	//�����������ӡ�

	//�������Ӹ�������
	mainBody->LinkTo(rootBone);
	//��̨���Ӹ�������ע�⣺��̨���ܳ��������Ӱ�죬������̨һֱָ���������λ�á�
	battery->LinkTo(rootBone);
	//�ڹ�������̨��
	barrel->LinkTo(battery);
	//�����Ŀ��������̨��
	cameraTarget->LinkTo(rootBone);
	//�����λ�����������Ŀ�ꡣ
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
	//�����������飬ɾ�����й�����
	for (int i = 0; i < _countof(pPlayerPawn->m_arr_Bones); ++i) 
	{
		gBoneCommander->DeleteBone(pPlayerPawn->m_arr_Bones[i]);
		pPlayerPawn->m_arr_Bones[i] = nullptr;
	}
}


//******************��ҿ���ģ��*****************************************
void PlayerControlCommandTemplate::MouseMove(BasePawn* pPawn, MouseState mouseState, const GameTimer& gt)
{
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	float dt = gt.DeltaTime();
	float dx = 0.01f * static_cast<float>(mouseState.CurrMousePos.x - mouseState.LastMousePos.x);
	float dy = 0.01f * static_cast<float>(mouseState.CurrMousePos.y - mouseState.LastMousePos.y);



	//��̨ˮƽ��ת
	//pPlayerPawn->Battery()->RotateYaw(1.0f * dx);

	//�����ˮƽ��ת��
	pPlayerPawn->m_pCamera->Target->RotateYaw(1.0f * dx);
	//�������ֱ��ת��
	pPlayerPawn->m_pCamera->Target->RotatePitch(1.0f * dy);
	//����������ľ�ͷ�����ǽǶȣ�������90�ȷ�Χ�ڡ�
	pPlayerPawn->m_pCamera->Target->Rotation.x = MathHelper::Clamp(
		pPlayerPawn->m_pCamera->Target->Rotation.x,
		-XM_PIDIV2,	
		XM_PI / 6 - 0.01f);	

	
}

void PlayerControlCommandTemplate::HitKey_W(BasePawn * pPawn, const GameTimer& gt)
{
	//ǰ��
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	//ʹ�ó������ת��
	XMFLOAT3 rotation = pPlayerPawn->MainBody()->Rotation;

	pPlayerPawn->RootControl()->MoveX(sinf(rotation.y) * pPlayerPawn->m_pProperty->MoveSpeed * gt.DeltaTime());
	pPlayerPawn->RootControl()->MoveZ(cosf(rotation.y) * pPlayerPawn->m_pProperty->MoveSpeed * gt.DeltaTime());
}

void PlayerControlCommandTemplate::HitKey_A(BasePawn * pPawn, const GameTimer& gt)
{
	//��ת
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->MainBody()->RotateYaw(-1.0f * pPlayerPawn->m_pProperty->RotationSpeed * gt.DeltaTime());
}

void PlayerControlCommandTemplate::HitKey_S(BasePawn * pPawn, const GameTimer& gt)
{
	//����
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	//ʹ�ó������ת��
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
	//��ת
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->MainBody()->RotateYaw(
		1.0f 
		* pPlayerPawn->		m_pProperty->	RotationSpeed 
		* gt.DeltaTime());
}

void PlayerControlCommandTemplate::HitMouseButton_Left(BasePawn * pPawn, const GameTimer & gt)
{
	ASSERT(gArmoredCarPawnType && "δ��ʼ��װ�׳������ͱ�ǣ��޷�����װ�׳���");

	//ʹ��װ�׳���Ĭ�����Խ��д�����
	gPawnMaster->CreatePawn(gArmoredCarPawnType, nullptr);
}

void PlayerControlCommandTemplate::HitMouseButton_Right(BasePawn * pPawn, const GameTimer & gt)
{
	//��������������ߣ����ײ����ĳ�����壬��������

	
	//ʹ���������Ŀ��λ���Լ�������Ϊ���ߵ�λ�ú���ʼ����
	auto pPlayer = reinterpret_cast<PlayerPawn*>(pPawn);
	/*
	//��������һ��������ײ��Ԫ��
	auto rayDetect = gCollideCommander->NewRay(
		pPlayer->m_pCamera->Target,	//���ߵ���㡢�Լ�����ʹ�������Ŀ�������任����
		MAX_RAY_LENGTH);			//���ߵĳ��ȶ��塣

	//������1����ײ�������ײ��⡣
	gCollideCommander->CollideDetect(rayDetect, COLLIDE_TYPE_BOX_1);

	if (rayDetect->Result.CollideHappended)
	{
		//ɾ����ײ������Pawn��
		PlayerPawn::pPawnMaster->DestroyPawn(
			PAWN_TYPE_NONE, 
			rayDetect-> Result.pCollideBox-> pPawn);
	}

	//����������ײ��Ԫ��
	gCollideCommander->DeleteRay(rayDetect);
	*/


	auto pShellProperty = gShellPropertyAllocator.Malloc();
	pShellProperty->CurrState = ShellState::Move;
	pShellProperty->DeltaDist = 0.0f;	//�����ʼ��֡�˶�����ֻ����ʱ�ģ�֮��ÿһ֡���ᱻShell�Զ����¡�
	pShellProperty->StartPos = pPlayer->Barrel()->World;
	pShellProperty->RestDist = 200.0f;
	pShellProperty->MoveSpeed = 200.0f;

	//��¼һ��������ҩPawn��ָ�
	gPawnMaster->CreatePawn(gShellPawnType, pShellProperty);
}

void PlayerControlCommandTemplate::PressMouseButton_Left(BasePawn * pPawn, const GameTimer& gt)
{
	//�����ӵ���
}

void PlayerControlCommandTemplate::PressMouseButton_Right(BasePawn * pPawn, const GameTimer& gt)
{
	//�����ڵ���
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
		ASSERT(false && "Player��AI״̬�Ƿ����޷�ִ�в�����");
	}
}

void PlayerAITemplate::aim(PlayerPawn * pPlayer, const GameTimer& gt)
{
	//��������ķ�����������ײ��
	//���ڹ�barrel�ķ����׼������ײ�ķ���
	auto ray = gCollideCommander->NewRay(pPlayer->m_pCamera->Target, 10e7f);
	gCollideCommander->CollideDetect(ray, COLLIDE_TYPE_ALL);

	XMFLOAT3 dummyLocation = { 0.0f, 0.0f, 0.0f };

	if (ray->Result.CollideHappended)
	{
		//��ת��λ��̨�ķ���ʹ����̨��ˮƽ�����Զ�׼������ײ��λ�á�
		//ע�ⲻ��ֱ��ָ�򣬶�����������ת��̨��ָ����λ�á�
		rotateBattery(pPlayer, ray->Result.CollideLocation, gt);
		//��ֱ��ת�ڹܣ�ʹ���ڹܶ�������ײ��λ�á�
		//ע�ⲻ��ֱ��ָ�򣬶�����������ת�ڹܵ�ָ����λ�á�
		rotateBarrel(pPlayer, ray->Result.CollideLocation, gt);
	}
	else
	{
		//û�з�����ײ���������λ��ǿ�ƹ��㡣
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

	//�ҵ���Ҫ��ת�Ķ�ˮƽ�Ƕȣ�ע��ֻ��Ҫ�ƾֲ�y�ᣨˮƽ������ת��
	//���ｫneedRx��Ϊfalse����ʾ����Ҫrx�����ܹ���ʡһ�㿪���ŵļ�������
	OffsetInLocal(world, target, false, rx, ry);

	//��û���õ�rx�����洢��ת���ȡ�
	rx = pPlayerPawn->m_pProperty->RotationSpeed * gt.DeltaTime();

	if (FloatEqual(ry, 0.0f, 0.02f))
	{
		//ʲôҲ������
	}
	else if (ry > 0.0f)
	{
		//��ת��
		pPlayerPawn->Battery()->RotateYaw(
			rx
		);
	}
	else
	{
		//��ת��
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

	//��û���õ�ry�����洢��ת���ȡ�
	ry = pPlayerPawn->m_pProperty->RotationSpeed * gt.DeltaTime();

	if (FloatEqual(rx, 0.0f, 0.02f))
	{
		//ʲôҲ������
	}
	else if (rx > 0.0f)
	{
		//������ת
		pPlayerPawn->Barrel()->RotatePitch(ry);
	}
	else
	{
		//������ת
		pPlayerPawn->Barrel()->RotatePitch(-ry);
	}

	//�����ڹܵĸ����ǽǶ�
	pPlayerPawn->Barrel()->Rotation.x = MathHelper::Clamp(
		pPlayerPawn->Barrel()->Rotation.x,
		Radian_Pitch_Barrel_Min,
		Radian_Pitch_Barrel_Max);
}
