#include "ArmoredCar.h"

//��ʼ����̬��Ա
//Pawn����
PawnType		ArmoredCar::PawnType = PAWN_TYPE_NONE;
//��������PawnMaster��
PawnMaster *	ArmoredCar::pPawnMaster		= nullptr;
//AIָ��١�
AICommander *	ArmoredCar::pAICommander	= nullptr;
//����ָ��١�
BoneCommander *	ArmoredCar::pBoneCommander	= nullptr;
//Pawn��������
MyStaticAllocator<CarProperty> ArmoredCar::PropertyAllocator(MAX_PAWN_CAR_NUM);
//���Է�������
MyStaticAllocator<ArmoredCar> ArmoredCar::PawnAllocator(MAX_PAWN_CAR_NUM);

ArmoredCar::ArmoredCar()
{
}


ArmoredCar::~ArmoredCar()
{
}

void ArmoredCar::RegisterAll(PawnMaster * pPawnMaster, 
	AICommander * pAICommander, BoneCommander * pBoneCommander)
{
	ASSERT(ArmoredCar::pAICommander == nullptr && "�����ظ�ע��AICommander");
	ASSERT(ArmoredCar::pBoneCommander == nullptr && "�����ظ�ע�����Commander");

	RegisterPawnMaster(pPawnMaster);

	ArmoredCar::pAICommander = pAICommander;
	ArmoredCar::pBoneCommander = pBoneCommander;

	//���AI�������͡�
	ArmoredCar::AIControlType = pAICommander->AddAITemplate(
		std::make_unique<CarAITemplate>());
}

void ArmoredCar::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	ASSERT(ArmoredCar::pPawnMaster == nullptr && "�����ظ�ע��PawnMaster");
	ArmoredCar::pPawnMaster = pPawnMaster;

	ArmoredCar::PawnType = pPawnMaster->AddCommandTemplate(
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

BasePawn * CarPawnTemplate::CreatePawn(PawnUnit * saveUnit, PawnProperty* pProperty, Scence* pScence)
{
	auto pCarProperty = reinterpret_cast<CarProperty*>(pProperty);

	ArmoredCar* newPawn = ArmoredCar::PawnAllocator.Malloc();

	//��¼�洢��λ��
	newPawn->m_pSaveUnit = saveUnit;

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
	pPawn->m_pAIUnit = ArmoredCar::pAICommander->NewAIUnit(ArmoredCar::AIControlType, pPawn);
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
	moveFragment.NextState = STORY_FRAGMENT_NEXT_RANDOM_OTHERS;

	//���״̬����
	StoryFragment shoutFragment;
	shoutFragment.State = STORY_FRAGMENT_CAR_SHOUT;
	shoutFragment.Posibility = 0.1f;
	shoutFragment.ConsistTime = 2.0f;
	moveFragment.NextState = STORY_FRAGMENT_NEXT_RANDOM;
	
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
		move(pCar);
		break;

		//��׼
	case STORY_FRAGMENT_CAR_AIM:
		aim(pCar);
		break;

		//���
	case STORY_FRAGMENT_CAR_SHOUT:
		shout(pCar, consumedTime);
		break;

	default:
		ASSERT(false && "ArmoredCar���ڷǷ�״̬���޷�ִ�в�����");
		break;
	}
}

void CarAITemplate::move(ArmoredCar * pCar)
{
	//Ŀ������
	auto targetType = PlayerPawn::m_pawnType;
	//Ŀ�����ڵ�pawnMaster��
	auto pawnMaster = PlayerPawn::m_pPawnMaster;
	//�洢Ŀ�������
	auto linkedList = &pawnMaster->CommandTemplateList[targetType]->Manager;

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

		//��ʵ�֡���
		//TODO

		pNode = pNode->m_pNext;
	}

}

void CarAITemplate::aim(ArmoredCar * pCar)
{
	//��ʵ�֡���
	//TODO
}

void CarAITemplate::shout(ArmoredCar * pCar, float consumedTime)
{
	//��ʵ�֡���
	//TODO
}
