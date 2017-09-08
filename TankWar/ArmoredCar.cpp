#include "ArmoredCar.h"

//������������
MyStaticAllocator<ArmoredCar>	ArmoredCar::PawnAllocator		(MAX_PAWN_CAR_NUM);

ArmoredCar::ArmoredCar()
{
}


ArmoredCar::~ArmoredCar()
{
}

void ArmoredCar::Register()
{
	gArmoredCarPawnType = gPawnMaster->AddCommandTemplate(
		std::make_unique<CarPawnTemplate>());

	//���AI�������͡�
	gArmoredCarAIControlType = gAICommander->AddAITemplate(
		std::make_unique<CarAITemplate>());
}

CarProperty * ArmoredCar::NewProperty()
{
	return gArmoredCarPropertyAllocator.Malloc();
}

ControlItem * ArmoredCar::RootControl()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_CAR_ROOT];
}

ControlItem * ArmoredCar::MainBody()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_CAR_MAIN_BODY];
}

CollideBox * ArmoredCar::RootBox()
{
	return m_arr_CBoxes[COLLIDE_RECT_INDEX_CAR_ROOT];
}


//****************************װ�׳���λ����ģ��***********************************************

CarPawnTemplate::CarPawnTemplate()
{
}

CarPawnTemplate::~CarPawnTemplate()
{
}

BasePawn * CarPawnTemplate::CreatePawn(PawnProperty* pProperty, Scence* pScence)
{
	auto pCarProperty = reinterpret_cast<CarProperty*>(pProperty);

	ArmoredCar* newPawn = ArmoredCar::PawnAllocator.Malloc();

	newPawn->m_pawnType = gArmoredCarPawnType;

	if (pProperty == nullptr)
	{
		//û���������ԣ��Զ�����һ��Ĭ�����ԡ�
		pCarProperty = ArmoredCar::NewProperty();
		
		pCarProperty->MoveSpeed = gCarDefaultMoveSpeed;
		pCarProperty->RotationSpeed = gCarDefaultRotationSpeed;
	}

	//��������
	newPawn->m_pProperty = pCarProperty;

	//����ControlItem
	//����������
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_ROOT] =
		pScence->NewControlItem("shapeGeo", "box", "box");
	//�����������
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_MAIN_BODY] =
		pScence->NewControlItem("shapeGeo", "box", "box");

	//���ظ�������������
	newPawn->RootControl()->Hide();

	//��������ؼ�
	AddAIControl(newPawn);
	AddBones(newPawn);
	AddCollideBoxes(newPawn);

	return newPawn;
}

void CarPawnTemplate::DestoryPawn(BasePawn * pPawn, Scence * pScence)
{
	ArmoredCar* pCar = reinterpret_cast<ArmoredCar*>(pPawn);

	//ɾ����������
	pScence->DeleteControlItem(pCar->RootControl());
	pScence->DeleteControlItem(pCar->MainBody());
	pCar->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_ROOT]			= nullptr;
	pCar->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_MAIN_BODY]	= nullptr;

	//�������Կռ䡣
	gArmoredCarPropertyAllocator.Free(pCar->m_pProperty);
	pCar->m_pProperty = nullptr;

	//ɾ�������ؼ�
	DeleteAIControl(pCar);
	DeleteBones(pCar);
	DeleteCollideBoxes(pCar);

	//����Pawn�ռ�
	ArmoredCar::PawnAllocator.Free(pCar);
}

void CarPawnTemplate::AddAIControl(ArmoredCar * pPawn)
{
	pPawn->m_pAIUnit = gAICommander->NewAIUnit(gArmoredCarAIControlType, pPawn);
	//�趨��ʼ״̬Ϊ�ƶ���
	pPawn->m_pAIUnit->CurrState = STORY_FRAGMENT_CAR_MOVE;
}

void CarPawnTemplate::AddBones(ArmoredCar * pPawn)
{
	Bone* root 
		= pPawn->m_arr_Bones[BONE_INDEX_PLAYER_CAR_ROOT]
		= gBoneCommander->NewBone(pPawn->RootControl());
	Bone* mainBody
		= pPawn->m_arr_Bones[BONE_INDEX_PLAYER_CAR_MAIN_BODY]
		= gBoneCommander->NewBone(pPawn->MainBody());

	mainBody->LinkTo(root);
}

void CarPawnTemplate::AddCollideBoxes(ArmoredCar * pPawn)
{
	//TODO
	PawnType pre = pPawn->m_pawnType;

	//Ϊ�������崴����ײ�С�
	CollideBox* pRootBox 
		=	pPawn->m_arr_CBoxes[ COLLIDE_RECT_INDEX_CAR_ROOT ] 
		=	gCollideCommander->NewCollideBox(
				COLLIDE_TYPE_BOX_1,		//��ײ�����ͱ��
				pPawn->MainBody(),	//ӵ����ײ���ControlItem��
				pPawn);				//ӵ����ײ���Pawn����

	PawnType aft = pPawn->m_pawnType;

	if (pre != aft)
	{
		pPawn->m_pawnType = pre;
	}

	//�޸���ײ�еĴ�С��
	pRootBox->Size.Xmin = -1.0f;	pRootBox->Size.Xmax = 1.0f;
	pRootBox->Size.Ymin = -0.8f;	pRootBox->Size.Ymax = 0.8f;
	pRootBox->Size.Zmin = -1.0f;	pRootBox->Size.Zmax = 1.0f;

	//���¼����Χ�е�����뾶��
	pRootBox->CaculateRadius();
}

void CarPawnTemplate::DeleteAIControl(ArmoredCar * pPawn)
{
	gAICommander->DeleteAIUnit(pPawn->m_pAIUnit);
	pPawn->m_pAIUnit = nullptr;
}

void CarPawnTemplate::DeleteBones(ArmoredCar * pPawn)
{
	//�����������飬ɾ�����й�����
	for (int i = 0; i < _countof(pPawn->m_arr_Bones); ++i)
	{
		//���չ�����
		gBoneCommander->DeleteBone(pPawn->m_arr_Bones[i]);
		//ָ���ÿա�
		pPawn->m_arr_Bones[i] = nullptr;
	}
}

void CarPawnTemplate::DeleteCollideBoxes(ArmoredCar * pPawn)
{
	gCollideCommander->DeleteCollideBox(pPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_CAR_ROOT]);

	pPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_CAR_ROOT] = nullptr;
}

//**************************װ�׳�AI��Ϊģ��******************************************************************

CarAITemplate::CarAITemplate()
{
	//�ڸ�������ӹ���Ƭ�Σ�AI�������趨�Ĺ������л�״̬��Ȼ����Runing��ִ�о���Ĳ�����

	//�ƶ�״̬����
	StoryFragment moveFragment;
	moveFragment.State = STORY_FRAGMENT_CAR_MOVE;
	moveFragment.Posibility = 0.6f;
	moveFragment.ConsistTime = 5.0f;
	moveFragment.NextState = STORY_FRAGMENT_NEXT_RANDOM;

	//��׼״̬����
	StoryFragment aimFragment;
	aimFragment.State = STORY_FRAGMENT_CAR_AIM;
	aimFragment.Posibility = 0.3f;
	aimFragment.ConsistTime = 1.0f;
	aimFragment.NextState = STORY_FRAGMENT_NEXT_RANDOM_OTHERS;

	//���״̬����
	StoryFragment shoutFragment;
	shoutFragment.State = STORY_FRAGMENT_CAR_SHOUT;
	shoutFragment.Posibility = 0.1f;
	shoutFragment.ConsistTime = 2.0f;
	shoutFragment.NextState = STORY_FRAGMENT_NEXT_RANDOM;

	/*
	//����״̬�������������
	StoryFragment shoutFragment;
	shoutFragment.State = STORY_FRAGMENT_CAR_SHOUT;
	shoutFragment.Posibility = 0.01f;
	shoutFragment.ConsistTime = 0.0f;
	shoutFragment.NextState = STORY_FRAGMENT_NEXT_NONE;
	*/
	
	//���״̬
	StoryBoard.push_back(moveFragment);
	StoryBoard.push_back(aimFragment);
	StoryBoard.push_back(shoutFragment);
}

void CarAITemplate::Runing(BasePawn* pPawn, AIStatue state, float consumedTime, const GameTimer& gt)
{
	ArmoredCar* pCar = reinterpret_cast<ArmoredCar * >(pPawn);
	switch (state)
	{
		//�ƶ�
	case STORY_FRAGMENT_CAR_MOVE:
		move(pCar, gt);
		break;

		//��׼
	case STORY_FRAGMENT_CAR_AIM:
		aim(pCar, gt);
		break;

		//���
	case STORY_FRAGMENT_CAR_SHOUT:
		shout(pCar, consumedTime, gt);
		break;

	default:
		ASSERT(false && "ArmoredCar���ڷǷ�״̬���޷�ִ�в�����");
		break;
	}
}

void CarAITemplate::move(ArmoredCar * pCar, const GameTimer& gt)
{
	auto pHead = PlayerPawn::PawnAllocator.GetHead();
	auto pNode = pHead->m_pNext;
	while (pNode != pHead)
	{
		//��ȡBasePawnָ�롣
		auto pPawn = &pNode->element;

		//ת��Ϊָ�������͡�
		PlayerPawn* pPlayer = reinterpret_cast<PlayerPawn*>(pPawn);
		
		//��ȡ����������
		auto rootControl = pPlayer->RootControl();

		MoveToward(pCar, rootControl, gt);

		//�����ơ���
		//TODO

		pNode = pNode->m_pNext;
	}


}

void CarAITemplate::aim(ArmoredCar * pCar, const GameTimer& gt)
{
	//��ʵ�֡���
	//TODO
}

void CarAITemplate::shout(ArmoredCar * pCar, float consumedTime, const GameTimer& gt)
{
	//��ʵ�֡���
	//TODO
}

void CarAITemplate::MoveToward(ArmoredCar * pCar, ControlItem * pTarget, const GameTimer& gt)
{
	XMFLOAT4 target(pTarget->World.m[3]);

	MoveToward(pCar, target, gt);
}

void CarAITemplate::MoveToward(ArmoredCar * pCar, XMFLOAT4 targetLocation, const GameTimer& gt)
{
	XMFLOAT4 curr(pCar->RootControl()->World.m[3]);

	XMVECTOR targetV	= XMLoadFloat4(&targetLocation);
	XMVECTOR currV		= XMLoadFloat4(&curr);

	float speed = 
		pCar->m_pProperty->MoveSpeed;

	//���ɵ�ָ��λ�õ�λ��������
	targetV = gt.DeltaTime() 
		* speed 
		* XMVector4Normalize(targetV - currV);

	XMStoreFloat4(&curr, targetV);
	pCar
		->RootControl()
		->MoveXYZ(curr.x, curr.y, curr.z);

}
