#include "ShellPawn.h"

//��̬�����ʼ��
LinkedAllocator<ShellPawn> ShellPawn::PawnAllocator(MAX_SHELL_PAWN_NUM);

ShellPawn::ShellPawn()
{
}


ShellPawn::~ShellPawn()
{
}

void ShellPawn::Register()
{
	//ע��PawnMaster��
	gShellPawnType =
		gPawnMaster->AddCommandTemplate(
			std::make_unique<ShellPawnTemplate>());

	//ע��AI���Ƶ�λ��
	gShellAIControlType =
		gAICommander->AddAITemplate(
			std::make_unique<ShellAITemplate>());
}

ShellProperty * ShellPawn::NewProperty()
{
	return gShellPropertyAllocator.Malloc();
}

ControlItem * ShellPawn::RootControl()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_SHELL_ROOT];
}


//*************************��ҩ��λ����ģ��*************************************
ShellPawnTemplate::ShellPawnTemplate()
{
}

ShellPawnTemplate::~ShellPawnTemplate()
{
}

BasePawn * ShellPawnTemplate::CreatePawn(PawnProperty* pProperty, Scence* pScence)
{
	auto pShellProperty = reinterpret_cast<ShellProperty * >(pProperty);
	auto pNewPawn = ShellPawn::PawnAllocator.Malloc();

	pNewPawn->m_pawnType = gShellPawnType;

	if (pProperty == nullptr)
	{
		//ʹ��Ĭ�����ԡ�
		pShellProperty = gShellPropertyAllocator.Malloc();

		pShellProperty->	MoveSpeed	= gShellDefaultMoveSpeed;
		pShellProperty->	CurrState	= gShellDefaultState;
		pShellProperty->	StartPos	= gShellDefaultStartPos;
		pShellProperty->	RestDist	= gShellDefaultRestDist;
		pShellProperty->	DeltaDist	= gShellDefaultDeltaDist;
	}

	//�������ԡ�
	pNewPawn->m_pProperty = pShellProperty;

	//����һ������������
	pNewPawn->m_arr_ControlItem[CONTROLITEM_INDEX_SHELL_ROOT] =
		pScence->NewControlItem("Shell", "Shell", "box");
	pNewPawn->RootControl()->Show();
	//�����������趨��λ�ã������ӵ��ķ���
	ReLocateShell(pNewPawn);

	//Ϊ������������һ��������
	pNewPawn->m_arr_Bones[BONE_INDEX_SHELL_ROOT] =
		gBoneCommander->NewBone(pNewPawn->RootControl());

	//����AI���Ƶ�λ��
	pNewPawn->m_pAIUnit =
		gAICommander->NewAIUnit(gShellAIControlType, pNewPawn);
	pNewPawn->m_pAIUnit->CurrState = STORY_FRAGMENT_SHELL_MOVE;

	//�������߼�ⵥλ��
	pNewPawn->m_pRayDetect =
		gCollideCommander->NewRay(pNewPawn->RootControl(), 0.0f);

	return pNewPawn;
}

void ShellPawnTemplate::DestoryPawn(BasePawn * pPawn, Scence * pScence)
{
	auto pShell = reinterpret_cast<ShellPawn * >(pPawn);

	//�������Կռ�
	gShellPropertyAllocator.Free(pShell->m_pProperty);

	//ɾ����������
	pScence->DeleteControlItem(pShell->RootControl());

	//ɾ��������
	gBoneCommander->DeleteBone(pShell->m_arr_Bones[BONE_INDEX_SHELL_ROOT]);

	//ɾ��AI�ؼ���
	gAICommander->DeleteAIUnit(pShell->m_pAIUnit);

	//ɾ�����ߵ�λ
	gCollideCommander->DeleteRay(pShell->m_pRayDetect);

	//ָ���ÿա�
	pShell->m_pProperty = nullptr;
	pShell->m_pAIUnit = nullptr;
	pShell->m_pRayDetect = nullptr;
	for (int i = 0; i < _countof(pShell->m_arr_ControlItem); ++i)
	{
		pShell->m_arr_ControlItem[i] = nullptr;
	}
	for (int i = 0; i < _countof(pShell->m_arr_Bones); ++i)
	{
		pShell->m_arr_Bones[i] = nullptr;
	}


	//����pawn����
	ShellPawn::PawnAllocator.Free(pShell);
}

void ShellPawnTemplate::ReLocateShell(ShellPawn * pPawn)
{
	//���س�ʼ��λ�á�
	XMMATRIX startPos = XMLoadFloat4x4(&pPawn->m_pProperty->StartPos);

	//ʹ��StartPos�������ӵ��ĳ�ʼλ�á�
	//XMStoreFloat4x4(&pPawn->RootControl()->World, startPos);

	//�ٶ�������
	XMVECTOR speedVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//ʹ�ó�ʼλ�þ�������ת�ٶ�����������ȡ�����ٶ�������
	speedVector = XMVector4Transform(speedVector, startPos);

	//�洢�ٶ��������˿�StartPos���ڴ潫�ᱻspeedVector����һ���֣�
	XMStoreFloat4(&pPawn->m_pProperty->MoveDirectionXYZW, speedVector);


	//�洢����
	XMStoreFloat3(&pPawn->RootControl()->Translation, startPos.r[3]);
	//����ֲ���ת����������Z�����ת
	float ry = 0.0f;
	float rx = 0.0f;
	RadianOfRotationMatrix(startPos, rx, ry);
	//��תShellPawn����ShellPawn�ĳ�����ٶȷ���һ�¡�
	pPawn->RootControl()->RotatePitch(rx);
	pPawn->RootControl()->RotateYaw(ry);

	//Ϊ�˷�ֹ��ʼ��ʱ���������껹��λ��ԭ�㣬ʩ�ӵ���ת��λ��û�ܼ�ʱ�ĸ��µ���������У�
	//�����ֶ�����һ���������
	//ע�⣺����ĸ���ֻ����ʱ�ģ�����һ����Ϸѭ����ʱ��������󽫻�ʹ���������ù���
	//�ֲ���ת�;ֲ����ꡣ
	XMStoreFloat4x4(&pPawn->RootControl()->World, startPos);
}

ShellAITemplate::ShellAITemplate()
{
	//��ͨ�ƶ�״̬��
	StoryFragment moveFragment;
	moveFragment.State = STORY_FRAGMENT_SHELL_MOVE;
	moveFragment.Posibility = 0.5f;
	moveFragment.ConsistTime = 500.0f;
	//û����һ��״̬��״̬�ֶ�ת����
	moveFragment.NextState = STORY_FRAGMENT_NEXT_NONE;

	//����״̬��
	StoryFragment followFragment;
	moveFragment.State = STORY_FRAGMENT_SHELL_FOLLOW;
	moveFragment.Posibility = 0.5f;
	moveFragment.ConsistTime = 500.0f;
	//û����һ��״̬��״̬�ֶ�ת����
	moveFragment.NextState = STORY_FRAGMENT_NEXT_NONE;

	StoryBoard.push_back(moveFragment);
	StoryBoard.push_back(followFragment);
}

ShellAITemplate::~ShellAITemplate()
{
}

void ShellAITemplate::Runing(
	BasePawn * pPawn, AIStatue statue,
	float consumedTime, const GameTimer& gt)
{
	auto pShell = reinterpret_cast<ShellPawn * >(pPawn);

	switch (statue)
	{
	case STORY_FRAGMENT_SHELL_MOVE:
		//���㵱ǰ֡���ƶ����룬�Լ�ʣ����ƶ����룬
		caculateDeltaDist(pShell, gt);
		//�ƶ�״̬������ָ����Ŀ������ƶ���
		move(pShell, gt);
		//���������ײ��ɾ����ײ����һ��Pawn��
		collideDetect(pShell, gt);
		break;

	case STORY_FRAGMENT_SHELL_FOLLOW:
		//����״̬���������ƶ������Ǽ��������ײ��ɾ����ײ����һ��Pawn��
		collideDetect(pShell, gt);
		break;

	default:
		ASSERT(false && "ShellPawn���ڷǷ�״̬���޷�ִ�в�����");
		break;
	}
}

void ShellAITemplate::move(ShellPawn * pShell, const GameTimer & gt)
{
	//��ȡ�ٶ�������ע����λ������
	XMVECTOR speedVector = XMLoadFloat4(
		&pShell->m_pProperty->MoveDirectionXYZW);

	XMFLOAT4 speedVectorFloat4;

	//�����ٶȺ�ʱ�䡣
	speedVector *= pShell->m_pProperty->DeltaDist;
	XMStoreFloat4(&speedVectorFloat4, speedVector);

	//�ƶ���
	pShell->RootControl()->MoveXYZ(
		speedVectorFloat4.x,
		speedVectorFloat4.y,
		speedVectorFloat4.z);
}

void ShellAITemplate::collideDetect(ShellPawn * pShell, const GameTimer & gt)
{
	if (pShell->m_pProperty->CurrState == ShellState::Dead)
	{
		//��ҩ������ȡ����ײ��顣
		return;
	}

	auto rayDetect = pShell->m_pRayDetect;
	gCollideCommander->CollideDetect(rayDetect, COLLIDE_TYPE_BOX_1);

	if (rayDetect->Result.CollideHappended)
	{
		//������ײ��ɾ������ײ����Pawn����
		gPawnMaster->DestroyPawn(PAWN_TYPE_NONE, rayDetect->Result.pCollideBox->pPawn);
		//ɾ����ҩ����
		gPawnMaster->DestroyPawn(PAWN_TYPE_NONE, pShell);
		//�������״̬ΪDead��
		pShell->m_pProperty->CurrState = ShellState::Dead;
	}
}

void ShellAITemplate::caculateDeltaDist(ShellPawn * pShell, const GameTimer & gt)
{
	float deltaDist =
		pShell->m_pProperty->DeltaDist = 
		pShell->m_pProperty->MoveSpeed * gt.DeltaTime();

	pShell->m_pProperty->RestDist -= deltaDist;

	if (pShell->m_pProperty->RestDist < 0.0f)
	{
		//�����ƶ����룬ɾ����ҩ����
		gPawnMaster->DestroyPawn(PAWN_TYPE_NONE, pShell);
		//�������״̬ΪDead��
		pShell->m_pProperty->CurrState = ShellState::Dead;
		return;
	}

	pShell->m_pRayDetect->RayLength = deltaDist;
	pShell->m_pRayDetect->MinusLength = - deltaDist / 2;
}
