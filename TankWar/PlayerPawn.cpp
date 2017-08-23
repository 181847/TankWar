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

ControlItem *& PlayerPawn::RootControl()
{
	//���ؿ��������顣
	return m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT];
}

//***************************************�������ģ��*****************************
BasePawn* PlayerPawnCommandTemplate::CreatePawn(PawnProperty* pProperty, Scence* pScence)
{
	PlayerPawn* newPawn = PlayerPawn::m_PlayerPawnAllocator.Malloc();
	//�ӳ����д������������������洢��Player�С�
	newPawn->m_pCamera = pScence->AppendCamera();

	//���ڵ����������һ���ɿ��ƵĶ��������塣
	newPawn->RootControl() =
		//ָ����ҵ�һ����Ⱦ����ʱshapeGeo�е�box����
		//ָ��MeshGeometry��SubMesh��Material��
		pScence->NewControlItem("shapeGeo", "box", "box");

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
	ASSERT(pPlayerPawn->m_pPlayerControl == nullptr && "���ܶ�PlayerPawn�ظ������ҿ���");
	pPlayerPawn->m_pPlayerControl =
		PlayerPawn::m_pPlayerCommander->NewPlayerControl(
			pPlayerPawn->m_playerControlType, pPlayerPawn);
}

void PlayerPawnCommandTemplate::AddBones(PlayerPawn * pPlayerPawn)
{
	//һ����Ⱦ����Ĺ�����
	Bone* rootBone = 
		pPlayerPawn->m_arr_Bones[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT] =
		PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT]);

	//�����λ�õĹ�����
	Bone* cameraPos = 
		pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_POS] =
		(PlayerPawn::m_pBoneCommander)->NewBone(pPlayerPawn->m_pCamera->Pos);

	//���������Ŀ��Ĺ�����
	Bone* cameraTarget 
		= pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET] 
		= PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->m_pCamera->Target);

	//�����������ӡ�
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
	float dx = static_cast<float>(mouseState.CurrMousePos.x - mouseState.LastMousePos.x);
	float dy = static_cast<float>(mouseState.CurrMousePos.y - mouseState.LastMousePos.y);

	//�����ˮƽ��ת��
	pPlayerPawn->m_pCamera->Target->RotateYaw(dx);
	//�������ֱ��ת��
	pPlayerPawn->m_pCamera->Target->RotatePitch(dy);

	//����������ľ�ͷ�����ǽǶȡ�
	pPlayerPawn->m_pCamera->Target->Rotation.y = 
		MathHelper::Clamp(
			pPlayerPawn->m_pCamera->Target->Rotation.y, 
			-0.333f * XM_PIDIV2,	//����������ľ�ͷ�����ǲ�����1/6 PI����-30�ȡ�
			XM_PIDIV2 - 0.001f);	//����д����ľ�ͷ�����ǲ����� PI/2 - 0.001����С��90�ȡ�
	
}

void PlayerControlCommandTemplate::HitKey_W(BasePawn * pPawn, const GameTimer& gt)
{
	//ǰ��
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->MoveX(0.1f * pPlayerPawn->m_pProperty->MoveSpeed);
}

void PlayerControlCommandTemplate::HitKey_A(BasePawn * pPawn, const GameTimer& gt)
{
	//��ת
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->RotateYaw(-0.1f * pPlayerPawn->m_pProperty->RotationSpeed);
}

void PlayerControlCommandTemplate::HitKey_S(BasePawn * pPawn, const GameTimer& gt)
{
	//����
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->MoveX(-0.1f * pPlayerPawn->m_pProperty->MoveSpeed);
}

void PlayerControlCommandTemplate::HitKey_D(BasePawn * pPawn, const GameTimer& gt)
{
	//��ת
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->RotateYaw(0.1f * pPlayerPawn->m_pProperty->RotationSpeed);
}

void PlayerControlCommandTemplate::PressMouseButton_Left(BasePawn * pPawn, const GameTimer& gt)
{
	//�����ӵ���
}

void PlayerControlCommandTemplate::PressMouseButton_Right(BasePawn * pPawn, const GameTimer& gt)
{
	//�����ڵ���
}
