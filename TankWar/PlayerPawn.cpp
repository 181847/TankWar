#include "PlayerPawn.h"


//��ʼʱ��̬��Ա��
PawnType							PlayerPawn::pawnType				= PAWN_TYPE_NONE;
//Player���������͡�
PlayerControlType					PlayerPawn::m_playerControlType		= PLAYER_CONTROL_TYPE_NONE;
//PawnMaster��
PawnMaster *						PlayerPawn::pPawnMaster				= nullptr;
//���ָ��١�
PlayerCommander *					PlayerPawn::pPlayerCommander		= nullptr;
//����ָ��١�
BoneCommander *						PlayerPawn::pBoneCommander			= nullptr;
//��ײָ��١�
CollideCommander *					PlayerPawn::pCollideCommander		= nullptr;
MyStaticAllocator<PlayerProperty>	PlayerPawn::m_propertyAllocator		(MAX_PLAYER_PAWN_NUM);
MyStaticAllocator<PlayerPawn>		PlayerPawn::m_PlayerPawnAllocator	(MAX_PLAYER_PAWN_NUM);
//�ο�װ�׳����ͱ�ǣ�������ͱ������ʵ�ֵ��������Ȼ������װ�׳���Ч��
PawnType							PlayerPawn::refCarType				= PAWN_TYPE_NONE;

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
	ASSERT(PlayerPawn::pPlayerCommander	== nullptr	&&	"�����ظ�ע��PlayerCommander");
	ASSERT(PlayerPawn::pBoneCommander		== nullptr	&&	"�����ظ�ע��BoneCommander");
	//ASSERT(PlayerPawn::pCollideCommander	== nullptr	&&	"�����ظ�ע��CollideCommander");

	//ע��Master
	RegisterPawnMaster(pPawnMaster);

	//ע��Commander��
	PlayerPawn::pPlayerCommander = pPlayerCommander;
	//���һ���µ���ҿ���ģʽ
	m_playerControlType = pPlayerCommander->AddCommandTemplate(
		std::make_unique<PlayerControlCommandTemplate>());

	PlayerPawn::pBoneCommander = pBoneCommander;
	//pCollideCommander = pCollideCommander;
}

void PlayerPawn::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	ASSERT(PlayerPawn::pPawnMaster == nullptr && "�����ظ�ע��PawnMaster");
	//ע��һ��CommandTempalte�����һ��һ��pawnType��
	PlayerPawn::pawnType = pPawnMaster->AddCommandTemplate(
		std::make_unique<PlayerPawnCommandTemplate>());

	//��¼PawnMaster
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


//***************************************�������ģ��*****************************
PlayerPawnCommandTemplate::PlayerPawnCommandTemplate()
	:PawnCommandTemplate(MAX_PLAYER_PAWN_NUM)
{
}

PlayerPawnCommandTemplate::~PlayerPawnCommandTemplate()
{
}

BasePawn* PlayerPawnCommandTemplate::CreatePawn(PawnUnit * saveUnit, PawnProperty* pProperty, Scence* pScence)
{
	PlayerPawn* newPawn = PlayerPawn::m_PlayerPawnAllocator.Malloc();

	newPawn->m_pawnType = PlayerPawn::pawnType;

	//��¼�洢��λָ�롣
	newPawn->m_pSavedUnit = saveUnit;

	//��¼���ԡ�
	newPawn->m_pProperty = reinterpret_cast<PlayerProperty*>(pProperty);
	//�ӳ����д������������������洢��Player�С�
	newPawn->m_pCamera = pScence->AppendCamera();

	newPawn->m_pCamera->Target->Translation = { 0.0f, 2.0f, 0.0f };

	//�޸�������ľֲ����꣬�����ٺ����ĸ����к��������Ŀ���غϣ������쳣��
	newPawn->m_pCamera->Pos->Translation = { 0.0f, 0.0f, -5.0f };
	//�޸����������������е�ƽ�ƣ���Ϊ������������µ��������۲�����е���������������ƽ�ƣ�
	//�������Ŀ���λ�õ���������ǵ�λ����ƽ����ͬ����һ���������۲����ʱ������һ����
	//������ʱ�޸�һ������ƽ�����꣬��Ϊ�ں�����Ӿֲ������л����Ϣ��
	newPawn->m_pCamera->Pos->World._41 = 1.0f;

	//���ڵ����������һ���ɿ��ƵĶ��������塣
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
		//ָ����ҵ�һ����Ⱦ����ʱshapeGeo�е�box����
		//ָ��MeshGeometry��SubMesh��Material��
		pScence->NewControlItem("shapeGeo", "box", "box");
	//�����������أ�ֻ��������Pawn��λ�ƣ���������ת��
	newPawn->RootControl()->Hide();

	//Ϊ��̨����һ��ControlItem��
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_BATTERY] =
		pScence->NewControlItem("Tank", "Box006", "box");
	//��̨�������ƶ�һ����롣
	newPawn->Battery()->MoveY(1.0f);

	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_MAINBODY] =
		pScence->NewControlItem("Tank", "Box005", "cylinder");
	newPawn->MainBody()->MoveY(1.0f);

	//�����ҿ��ơ�
	AddPlayerControl(newPawn);

	//��ӹ���������
	AddBones(newPawn);

	//�����ײ�¼�����ָ��һ�������¼���
	//TODO PlayerPawn����ײ����ӡ�
	//AddCollideItems(newPawn);
	return newPawn;
}

PawnUnit* PlayerPawnCommandTemplate::DestoryPawn(BasePawn* pPawn, Scence* pScence)
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

	//����Pawn����
	PlayerPawn::m_PlayerPawnAllocator.Free(toDeletePawn);

	//���ش洢��Ԫָ�롣
	return toDeletePawn->m_pSavedUnit;
}

void PlayerPawnCommandTemplate::AddPlayerControl(PlayerPawn * pPlayerPawn)
{
	//ASSERT(pPlayerPawn->m_pPlayerControl == nullptr && "���ܶ�PlayerPawn�ظ������ҿ���");
	pPlayerPawn->m_pPlayerControl =
		PlayerPawn::pPlayerCommander->NewPlayerControl(
			pPlayerPawn->m_playerControlType, pPlayerPawn);
}

void PlayerPawnCommandTemplate::AddBones(PlayerPawn * pPlayerPawn)
{
	//һ�����������Ĺ�����
	Bone* rootBone = 
		pPlayerPawn->m_arr_Bones[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
		PlayerPawn::pBoneCommander->NewBone(pPlayerPawn->RootControl());

	//���������Ŀ��Ĺ�����
	Bone* cameraTarget
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET]
		= PlayerPawn::pBoneCommander->NewBone(pPlayerPawn->m_pCamera->Target);

	//�����λ�õĹ�����
	Bone* cameraPos 
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_POS]
		= (PlayerPawn::pBoneCommander)->NewBone(pPlayerPawn->m_pCamera->Pos);

	//��̨������
	Bone* battery
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_BATTERY]
		= PlayerPawn::pBoneCommander->NewBone(pPlayerPawn->Battery());

	//���������
	Bone* mainBody
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_MAINBODY]
		= PlayerPawn::pBoneCommander->NewBone(pPlayerPawn->MainBody());

	//�����������ӡ�

	//�������Ӹ�������
	mainBody->LinkTo(rootBone);
	//��̨���Ӹ�������ע�⣺��̨���ܳ��������Ӱ�죬������̨һֱָ���������λ�á�
	battery->LinkTo(rootBone);
	//�����Ŀ��������̨��
	cameraTarget->LinkTo(battery);
	//�����λ�����������Ŀ�ꡣ
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
	//�����������飬ɾ�����й�����
	for (int i = 0; i < _countof(pPlayerPawn->m_arr_Bones); ++i) 
	{
		PlayerPawn::pBoneCommander->DeleteBone(pPlayerPawn->m_arr_Bones[i]);
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



	//��̨ˮƽ��ת�������������ˮƽ��ת��
	pPlayerPawn->Battery()->RotateYaw(1.0f * dx);

	//�������ֱ��ת��
	pPlayerPawn->m_pCamera->Target->RotatePitch(1.0f * dy);
	//����������ľ�ͷ�����ǽǶȣ�������90�ȷ�Χ�ڡ�
	pPlayerPawn->m_pCamera->Target->Rotation.x = MathHelper::Clamp(
		pPlayerPawn->m_pCamera->Target->Rotation.x,
		-XM_PI / 6,	
		XM_PIDIV2 - 0.01f);	
	
	
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
	ASSERT(PlayerPawn::refCarType && "δ��ʼ��װ�׳������ͱ�ǣ��޷�����װ�׳���");

	//ʹ��װ�׳���Ĭ�����Խ��д�����
	PlayerPawn::pPawnMaster->CreatePawn(PlayerPawn::refCarType, nullptr);
}

void PlayerControlCommandTemplate::HitMouseButton_Right(BasePawn * pPawn, const GameTimer & gt)
{
	//��������������ߣ����ײ����ĳ�����壬��������

	//ʹ���������Ŀ��λ���Լ�������Ϊ���ߵ�λ�ú���ʼ����
	auto pPlayer = reinterpret_cast<PlayerPawn*>(pPawn);

	//��������һ��������ײ��Ԫ��
	auto rayDetect = PlayerPawn::pCollideCommander->NewRay(
		pPlayer->m_pCamera->Target,	//���ߵ���㡢�Լ�����ʹ�������Ŀ�������任����
		MAX_RAY_LENGTH);			//���ߵĳ��ȶ��塣

	//������1����ײ�������ײ��⡣
	PlayerPawn::pCollideCommander->CollideDetect(rayDetect, COLLIDE_TYPE_BOX_1);

	if (rayDetect->Result.CollideHappended)
	{
		//ɾ����ײ������Pawn��
		PlayerPawn::pPawnMaster->DestroyPawn(
			PAWN_TYPE_NONE, 
			rayDetect-> Result.pCollideBox-> pPawn);
	}

	//����������ײ��Ԫ��
	PlayerPawn::pCollideCommander->DeleteRay(rayDetect);
}

void PlayerControlCommandTemplate::PressMouseButton_Left(BasePawn * pPawn, const GameTimer& gt)
{
	//�����ӵ���
}

void PlayerControlCommandTemplate::PressMouseButton_Right(BasePawn * pPawn, const GameTimer& gt)
{
	//�����ڵ���
}
