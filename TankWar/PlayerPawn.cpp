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
		new PlayerControlCommandTemplate());

	m_pBoneCommander = pBoneCommander;
	//m_pCollideCommander = pCollideCommander;
}

void PlayerPawn::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	ASSERT(PlayerPawn::m_pPawnMaster == nullptr && "�����ظ�ע��PawnMaster");
	//ע��һ��CommandTempalte�����һ��һ��pawnType��
	PlayerPawn::m_pawnType = pPawnMaster->AddCommandTemplate(
		new PlayerPawnCommandTemplate());
}

ControlItem *& PlayerPawn::RootControl()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_PLAYER_PAWN_ROOT];
}

//***************************************�������ģ��*****************************
BasePawn* PlayerPawnCommandTemplate::CreatePawn(PawnProperty* pProperty, Scence* pScence)
{
	PlayerPawn* newPawn = PlayerPawn::m_PlayerPawnAllocator.Malloc();
	//�ӳ����д������������������洢��Player�С�
	newPawn->m_pCamera = pScence->AppendCamera();

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

	//�ͷ����ControlItem���������е�ControlItem��
	pScence->DeleteControlItem(toDeletePawn->RootControl());

	//�ͷ��������ͷ��
	pScence->DeleteCamera(toDeletePawn->m_pCamera);

	//����Pawn����
	m_playerAllocator.Free(toDeletePawn);
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
		PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->m_pCamera->Pos());

	//���������Ŀ��Ĺ�����
	Bone* cameraTarget = 
		pPlayerPawn->m_arr_Bones[BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET] =
		PlayerPawn::m_pBoneCommander->NewBone(pPlayerPawn->m_pCamera->Target());

	//�����������ӡ�
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
	//�����������飬ɾ�����й�����
	for (int i = 0; i < _countof(pPlayerPawn->m_arr_Bones); ++i) 
	{
		PlayerPawn::m_pBoneCommander->DeleteBone(pPlayerPawn->m_arr_Bones[i]);
		pPlayerPawn->m_arr_Bones[i] = nullptr;
	}
}


//******************��ҿ���ģ��*****************************************
void PlayerControlCommandTemplate::MouseMove(BasePawn * pPawn, float lastX, float lastY, float currX, float currY, WPARAM btnState)
{
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	float dx = currX - lastX;
	float dy = currY - lastY;

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

void PlayerControlCommandTemplate::HitKey_W(BasePawn * pPawn)
{
	//ǰ��
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->MoveX(0.1f * m_pProperty->MoveSpeed);
}

void PlayerControlCommandTemplate::HitKey_A(BasePawn * pPawn)
{
	//��ת
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->RotateYaw(-0.1f * m_pProperty->RotateSpeed);
}

void PlayerControlCommandTemplate::HitKey_S(BasePawn * pPawn)
{
	//����
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->MoveX(-0.1f * m_pProperty->MoveSpeed);
}

void PlayerControlCommandTemplate::HitKey_D(BasePawn * pPawn)
{
	//��ת
	PlayerPawn* pPlayerPawn = reinterpret_cast<PlayerPawn*>(pPawn);
	pPlayerPawn->RootControl()->RotateYaw(0.1f * m_pProperty->RotateSpeed);
}

void PlayerControlCommandTemplate::PressMouseButton_Left(BasePawn * pPawn)
{
	//�����ӵ���
}

void PlayerControlCommandTemplate::PressMouseButton_Right(BasePawn * pPawn)
{
	//�����ڵ���
}
