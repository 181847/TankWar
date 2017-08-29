#include "ArmoredCar.h"

//初始化静态成员
//Pawn类型
PawnType		ArmoredCar::PawnType = PAWN_TYPE_NONE;
//负责生成PawnMaster。
PawnMaster *	ArmoredCar::pPawnMaster		= nullptr;
//AI指令官。
AICommander *	ArmoredCar::pAICommander	= nullptr;
//骨骼指令官。
BoneCommander *	ArmoredCar::pBoneCommander	= nullptr;
//Pawn分配器。
MyStaticAllocator<CarProperty> ArmoredCar::PropertyAllocator(MAX_PAWN_CAR_NUM);
//属性分配器。
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
	ASSERT(ArmoredCar::pAICommander == nullptr && "不可重复注册AICommander");
	ASSERT(ArmoredCar::pBoneCommander == nullptr && "不可重复注册骨骼Commander");

	RegisterPawnMaster(pPawnMaster);

	ArmoredCar::pAICommander = pAICommander;
	ArmoredCar::pBoneCommander = pBoneCommander;

	//添加AI控制类型。
	ArmoredCar::AIControlType = pAICommander->AddAITemplate(
		std::make_unique<CarAITemplate>());
}

void ArmoredCar::RegisterPawnMaster(PawnMaster * pPawnMaster)
{
	ASSERT(ArmoredCar::pPawnMaster == nullptr && "不可重复注册PawnMaster");
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

	//记录存储单位。
	newPawn->m_pSaveUnit = saveUnit;

	//设置属性
	newPawn->m_pProperty = pCarProperty;

	//申请ControlItem
	//根控制器。
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_ROOT] =
		pScence->NewControlItem("shapeGeo", "box", "box");
	//车身控制器。
	newPawn->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_MAIN_BODY] =
		pScence->NewControlItem("shapeGeo", "box", "box");

	//隐藏根控制器的网格。
	newPawn->RootControl()->Hide();

	//添加其他控件
	AddAIControl(newPawn);
	AddBones(newPawn);

	return newPawn;
}

PawnUnit* CarPawnTemplate::DestoryPawn(BasePawn * pPawn, Scence * pScence)
{
	ArmoredCar* pCar = reinterpret_cast<ArmoredCar*>(pPawn);

	//删除控制器。
	pScence->DeleteControlItem(pCar->RootControl());
	pScence->DeleteControlItem(pCar->MainBody());
	pCar->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_ROOT]			= nullptr;
	pCar->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_MAIN_BODY]	= nullptr;

	//回收属性空间。
	ArmoredCar::PropertyAllocator.Free(pCar->m_pProperty);
	pCar->m_pProperty = nullptr;

	//删除其他控件
	DeleteAIControl(pCar);
	DeleteBones(pCar);

	//回收Pawn空间
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
	//遍历骨骼数组，删除所有骨骼。
	for (int i = 0; i < _countof(pPawn->m_arr_Bones); ++i)
	{
		//回收骨骼。
		ArmoredCar::pBoneCommander->DeleteBone(pPawn->m_arr_Bones[i]);
		//指针置空。
		pPawn->m_arr_Bones[i] = nullptr;
	}
}

CarAITemplate::CarAITemplate()
{
	//在父类中添加故事片段，AI将沿着设定的故事线切换状态，然后在Runing中执行具体的操作。

	//移动状态概览
	StoryFragment moveFragment;
	moveFragment.State = STORY_FRAGMENT_CAR_MOVE;
	moveFragment.Posibility = 0.6f;
	moveFragment.ConsistTime = 5.0f;
	moveFragment.NextState = STORY_FRAGMENT_NEXT_RANDOM;

	//瞄准状态概览
	StoryFragment aimFragment;
	aimFragment.State = STORY_FRAGMENT_CAR_AIM;
	aimFragment.Posibility = 0.3f;
	aimFragment.ConsistTime = 5.0f;
	moveFragment.NextState = STORY_FRAGMENT_NEXT_RANDOM_OTHERS;

	//射击状态概览
	StoryFragment shoutFragment;
	shoutFragment.State = STORY_FRAGMENT_CAR_SHOUT;
	shoutFragment.Posibility = 0.1f;
	shoutFragment.ConsistTime = 2.0f;
	moveFragment.NextState = STORY_FRAGMENT_NEXT_RANDOM;
	
	//添加状态
	StoryBoard.push_back(moveFragment);
	StoryBoard.push_back(aimFragment);
	StoryBoard.push_back(shoutFragment);
}

void CarAITemplate::Runing(BasePawn* pPawn, AIStatue state, float consumedTime, const GameTimer& gt)
{
	ArmoredCar* pCar = reinterpret_cast<ArmoredCar * >(pPawn);
	switch (state)
	{
		//移动
	case STORY_FRAGMENT_CAR_MOVE:
		move(pCar);
		break;

		//瞄准
	case STORY_FRAGMENT_CAR_AIM:
		aim(pCar);
		break;

		//射击
	case STORY_FRAGMENT_CAR_SHOUT:
		shout(pCar, consumedTime);
		break;

	default:
		ASSERT(false && "ArmoredCar处于非法状态，无法执行操作。");
		break;
	}
}

void CarAITemplate::move(ArmoredCar * pCar)
{
	//目标类型
	auto targetType = PlayerPawn::m_pawnType;
	//目标所在的pawnMaster。
	auto pawnMaster = PlayerPawn::m_pPawnMaster;
	//存储目标的链表。
	auto linkedList = &pawnMaster->CommandTemplateList[targetType]->Manager;

	auto pHead = linkedList->GetHead();
	auto pNode = pHead->m_pNext;
	while (pNode != pHead)
	{
		//获取BasePawn指针。
		auto pPawn = pNode->element.pSavedPawn;

		//转换为指定的类型。
		PlayerPawn* pPlayer = reinterpret_cast<PlayerPawn*>(pPawn);
		
		//获取根控制器。
		auto rootControl = pPlayer->RootControl();

		//待实现……
		//TODO

		pNode = pNode->m_pNext;
	}

}

void CarAITemplate::aim(ArmoredCar * pCar)
{
	//待实现……
	//TODO
}

void CarAITemplate::shout(ArmoredCar * pCar, float consumedTime)
{
	//待实现……
	//TODO
}
