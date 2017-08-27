#include "PlayerPawn.h"


//��ʼʱ��̬��Ա��
PawnType			PlayerPawn::m_pawnType = PAWN_TYPE_NONE;
//Player���������͡�
PlayerControlType	PlayerPawn::m_playerControlType = PLAYER_CONTROL_TYPE_NONE;
//PawnMaster��
PawnMaster *		PlayerPawn::m_pPawnMaster = nullptr;
//���ָ��١�
PlayerCommander *	PlayerPawn::m_pPlayerCommander = nullptr;
//����ָ��١�
BoneCommander *		PlayerPawn::m_pBoneCommander = nullptr;
//��ײָ��١�
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
	ASSERT(PlayerPawn::m_pPlayerCommander	== nullptr	&&	"�����ظ�ע��PlayerCommander");
	ASSERT(PlayerPawn::m_pBoneCommander		== nullptr	&&	"�����ظ�ע��BoneCommander");
	//ASSERT(PlayerPawn::m_pCollideCommander	== nullptr	&&	"�����ظ�ע��CollideCommander");

	//ע��Master
	RegisterPawnMaster(pPawnMaster);

	//ע��Commander��
	m_pPlayerCommander = pPlayerCommander;
	//���һ���µ���ҿ���ģʽ
	m_playerControlType = m_pPlayerCommander->AddCommandTemplate(
		std::make_unique<PlayerControlCommandTemplate>());

	m_pBoneCommander = pBoneCommander;
	//m_pCollideCommander = pCollideCommander;
}

void PlayerPawn::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	ASSERT(PlayerPawn::m_pPawnMaster == nullptr && "�����ظ�ע��PawnMaster");
	//ע��һ��CommandTempalte�����һ��һ��pawnType��
	PlayerPawn::m_pawnType = pPawnMaster->AddCommandTemplate(
		std::make_unique<PlayerPawnCommandTemplate>());
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
BasePawn* PlayerPawnCommandTemplate::CreatePawn(PawnProperty* pProperty, Scence* pScence)
{
	PlayerPawn* newPawn = PlayerPawn::m_PlayerPawnAllocator.Malloc();
	newPawn->m_pProperty = reinterpret_cast<PlayerProperty*>(pProperty);
	//�ӳ����д������������������洢��Player�С�
	newPawn->m_pCamera = pScence->AppendCamera();

	newPawn->m_pCamera->Target->Translation = { 0.0f, 2.0f, 0.0f };

	//�޸�������ľֲ����꣬�����ٺ����ĸ����к��������Ŀ���غϣ������쳣��
	newPawn->m_pCamera->Pos->Translation = { 0.0f, 0.0f, -100.0f };
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

	//����Pawn����
	PlayerPawn::m_PlayerPawnAllocator.Free(toDeletePawn);
}

void PlayerPawnCommandTemplate::AddPlayerControl(PlayerPawn * pPlayerPawn)
{
	//ASSERT(pPlayerPawn->m_pPlayerControl == nullptr && "���ܶ�PlayerPawn�ظ������ҿ���");
	pPlayerPawn->m_pPlayerControl =
		PlayerPawn::m_pPlayerCommander->NewPlayerControl(
			pPlayerPawn->m_playerControlType, pPlayerPawn);
}

void PlayerPawnCommandTemplate::AddBones(PlayerPawn * pPlayerPawn)
{
	//һ�����������Ĺ�����
	Bone* rootBone = 
		pPlayerPawn->m_arr_Bones[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
		PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->RootControl());

	//���������Ŀ��Ĺ�����
	Bone* cameraTarget
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET]
		= PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->m_pCamera->Target);

	//�����λ�õĹ�����
	Bone* cameraPos 
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_POS]
		= (PlayerPawn::m_pBoneCommander)->NewBone(pPlayerPawn->m_pCamera->Pos);

	//��̨������
	Bone* battery
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_BATTERY]
		= PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->Battery());

	//���������
	Bone* mainBody
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_MAINBODY]
		= PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->MainBody());

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
	PlayerPawn::m_pPlayerCommander->DeletePlayerControl(toDeletePawn->m_pPlayerControl);
	pPlayerPawn->m_pPlayerControl = nullptr;
}

void PlayerPawnCommandTemplate::DeleteBones(PlayerPawn * pPlayerPawn)
{
	//�����������飬ɾ�����й�����
	for (int i = 0; i < _countof(pPlayerPawn->m_arr_Bones); ++i) 
	{
		PlayerPawn::m_pBoneCommander->DeleteBone(pPlayerPawn->m_arr_Bones[i]);
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

	pPlayerPawn->RootControl()->MoveX(-1.0f * sinf(rotation.y) * pPlayerPawn->m_pProperty->MoveSpeed * gt.DeltaTime());
	pPlayerPawn->RootControl()->MoveZ(-1.0f * cosf(rotation.y) * pPlayerPawn->m_pProperty->MoveSpeed * gt.DeltaTime());
}

void PlayerControlCommandTemplate::HitKey_D(BasePawn * pPawn, const GameTimer& gt)
{
	//��ת
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->MainBody()->RotateYaw(1.0f * pPlayerPawn->m_pProperty->RotationSpeed * gt.DeltaTime());
}

void PlayerControlCommandTemplate::PressMouseButton_Left(BasePawn * pPawn, const GameTimer& gt)
{
	//�����ӵ���
}

void PlayerControlCommandTemplate::PressMouseButton_Right(BasePawn * pPawn, const GameTimer& gt)
{
	//�����ڵ���
}
