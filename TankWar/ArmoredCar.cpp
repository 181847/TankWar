#include "ArmoredCar.h"

//��ʼ����̬��Ա
//Pawn����
PawnType						ArmoredCar::pawnType			= PAWN_TYPE_NONE;
//AI������Ԫ����
AIControlType					ArmoredCar::aiControlType		= AI_CONTROL_TYPE_NONE;
//��������PawnMaster��
PawnMaster *					ArmoredCar::pPawnMaster			= nullptr;
//AIָ��١�
AICommander *					ArmoredCar::pAICommander		= nullptr;
//����ָ��١�
BoneCommander *					ArmoredCar::pBoneCommander		= nullptr;
//��ײָ��١�
CollideCommander *				ArmoredCar::pCollideCommander	= nullptr;
//Pawn��������
MyStaticAllocator<CarProperty>	ArmoredCar::PropertyAllocator	(MAX_PAWN_CAR_NUM);
//���Է�������
MyStaticAllocator<ArmoredCar>	ArmoredCar::PawnAllocator		(MAX_PAWN_CAR_NUM);

//��ʼ��Ĭ������
const float CarProperty_default_MoveSpeed		= 2.0f;
const float CarProperty_default_RotationSpeed	= 2.0f;

ArmoredCar::ArmoredCar()
{
}


ArmoredCar::~ArmoredCar()
{
}

void ArmoredCar::RegisterAll(
	PawnMaster * pPawnMaster, 
	AICommander * pAICommander, 
	BoneCommander * pBoneCommander,
	CollideCommander *	pCollideCommander)
{
	ASSERT(ArmoredCar::pAICommander == nullptr && "�����ظ�ע��AICommander");
	ASSERT(ArmoredCar::pBoneCommander == nullptr && "�����ظ�ע�����Commander");

	RegisterPawnMaster(pPawnMaster);

	ArmoredCar::pAICommander = pAICommander;
	ArmoredCar::pBoneCommander = pBoneCommander;
	ArmoredCar::pCollideCommander = pCollideCommander;

	//���AI�������͡�
	ArmoredCar::aiControlType = pAICommander->AddAITemplate(
		std::make_unique<CarAITemplate>());
}

void ArmoredCar::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	ASSERT(ArmoredCar::pPawnMaster == nullptr && "�����ظ�ע��PawnMaster");
	ArmoredCar::pPawnMaster = pPawnMaster;

	ArmoredCar::pawnType = pPawnMaster->AddCommandTemplate(
		std::make_unique<CarPawnTemplate>());
}

CarProperty * ArmoredCar::NewProperty()
{
	return PropertyAllocator.Malloc();
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
	:PawnCommandTemplate(MAX_PAWN_CAR_NUM)
{
}

CarPawnTemplate::~CarPawnTemplate()
{
}

BasePawn * CarPawnTemplate::CreatePawn(PawnUnit * saveUnit, PawnProperty* pProperty, Scence* pScence)
{
	auto pCarProperty = reinterpret_cast<CarProperty*>(pProperty);

	ArmoredCar* newPawn = ArmoredCar::PawnAllocator.Malloc();

	newPawn->m_pawnType = ArmoredCar::pawnType;

	//��¼�洢��λ��
	newPawn->m_pSaveUnit = saveUnit;

	if (pProperty == nullptr)
	{
		//û���������ԣ��Զ�����һ��Ĭ�����ԡ�
		pCarProperty = ArmoredCar::NewProperty();
		
		pCarProperty->MoveSpeed = CarProperty_default_MoveSpeed;
		pCarProperty->RotationSpeed = CarProperty_default_RotationSpeed;
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

PawnUnit* CarPawnTemplate::DestoryPawn(BasePawn * pPawn, Scence * pScence)
{
	ArmoredCar* pCar = reinterpret_cast<ArmoredCar*>(pPawn);

	//ɾ����������
	pScence->DeleteControlItem(pCar->RootControl());
	pScence->DeleteControlItem(pCar->MainBody());
	pCar->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_ROOT]			= nullptr;
	pCar->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_MAIN_BODY]	= nullptr;

	//�������Կռ䡣
	ArmoredCar::PropertyAllocator.Free(pCar->m_pProperty);
	pCar->m_pProperty = nullptr;

	//ɾ�������ؼ�
	DeleteAIControl(pCar);
	DeleteBones(pCar);

	//����Pawn�ռ�
	ArmoredCar::PawnAllocator.Free(pCar);

	return pCar->m_pSaveUnit;
}

void CarPawnTemplate::AddAIControl(ArmoredCar * pPawn)
{
	pPawn->m_pAIUnit = ArmoredCar::pAICommander->NewAIUnit(ArmoredCar::aiControlType, pPawn);
	//�趨��ʼ״̬Ϊ�ƶ���
	pPawn->m_pAIUnit->CurrState = STORY_FRAGMENT_CAR_MOVE;
}

void CarPawnTemplate::AddBones(ArmoredCar * pPawn)
{
	Bone* root 
		= pPawn->m_arr_Bones[BONE_INDEX_PLAYER_CAR_ROOT]
		= ArmoredCar::pBoneCommander->NewBone(pPawn->RootControl());
	Bone* mainBody
		= pPawn->m_arr_Bones[BONE_INDEX_PLAYER_CAR_MAIN_BODY]
		= ArmoredCar::pBoneCommander->NewBone(pPawn->MainBody());

	mainBody->LinkTo(root);
}

void CarPawnTemplate::AddCollideBoxes(ArmoredCar * pPawn)
{
	//Ϊ�������崴����ײ�С�
	CollideBox* pRootBox 
		=	pPawn->m_arr_CBoxes[ COLLIDE_RECT_INDEX_CAR_ROOT ] 
		=	ArmoredCar::pCollideCommander->NewCollideBox(
				COLLIDE_TYPE_BOX_1,		//��ײ�����ͱ��
				pPawn->MainBody(),	//ӵ����ײ���ControlItem��
				pPawn);				//ӵ����ײ���Pawn����

	//�޸���ײ�еĴ�С��
	pRootBox->Size.Xmin = -5;	pRootBox->Size.Xmax = 5;
	pRootBox->Size.Ymin = -1;	pRootBox->Size.Ymax = 1;
	pRootBox->Size.Zmin = -5;	pRootBox->Size.Zmax = 5;

	//���¼����Χ�е�����뾶��
	pRootBox->CaculateRadius();
}

void CarPawnTemplate::DeleteAIControl(ArmoredCar * pPawn)
{
	ArmoredCar::pAICommander->DeleteAIUnit(pPawn->m_pAIUnit);
	pPawn->m_pAIUnit = nullptr;
}

void CarPawnTemplate::DeleteBones(ArmoredCar * pPawn)
{
	//�����������飬ɾ�����й�����
	for (int i = 0; i < _countof(pPawn->m_arr_Bones); ++i)
	{
		//���չ�����
		ArmoredCar::pBoneCommander->DeleteBone(pPawn->m_arr_Bones[i]);
		//ָ���ÿա�
		pPawn->m_arr_Bones[i] = nullptr;
	}
}

void CarPawnTemplate::DeleteCollideBoxes(ArmoredCar * pPawn)
{
	ArmoredCar::pCollideCommander->DeleteCollideBox(pPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_CAR_ROOT]);

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
	aimFragment.ConsistTime = 5.0f;
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
	//Ŀ������
	auto targetType = PlayerPawn::pawnType;
	//Ŀ�����ڵ�pawnMaster��
	auto pawnMaster = PlayerPawn::pPawnMaster;
	//�洢Ŀ�������
	auto linkedList = 
		&(pawnMaster->CommandTemplateList[TO_ARRAY_INDEX(targetType)]
			->Manager);

	auto pHead = linkedList->GetHead();
	auto pNode = pHead->m_pNext;
	while (pNode != pHead)
	{
		//��ȡBasePawnָ�롣
		auto pPawn = pNode->element.pSavedPawn;

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
