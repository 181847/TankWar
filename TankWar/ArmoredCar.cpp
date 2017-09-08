#include "ArmoredCar.h"

//类对象分配器。
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

	//添加AI控制类型。
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


//****************************装甲车单位生成模板***********************************************

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
		//没有设置属性，自动创建一个默认属性。
		pCarProperty = ArmoredCar::NewProperty();
		
		pCarProperty->MoveSpeed = gCarDefaultMoveSpeed;
		pCarProperty->RotationSpeed = gCarDefaultRotationSpeed;
	}

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
	AddCollideBoxes(newPawn);

	return newPawn;
}

void CarPawnTemplate::DestoryPawn(BasePawn * pPawn, Scence * pScence)
{
	ArmoredCar* pCar = reinterpret_cast<ArmoredCar*>(pPawn);

	//删除控制器。
	pScence->DeleteControlItem(pCar->RootControl());
	pScence->DeleteControlItem(pCar->MainBody());
	pCar->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_ROOT]			= nullptr;
	pCar->m_arr_ControlItem[CONTROLITEM_INDEX_CAR_MAIN_BODY]	= nullptr;

	//回收属性空间。
	gArmoredCarPropertyAllocator.Free(pCar->m_pProperty);
	pCar->m_pProperty = nullptr;

	//删除其他控件
	DeleteAIControl(pCar);
	DeleteBones(pCar);
	DeleteCollideBoxes(pCar);

	//回收Pawn空间
	ArmoredCar::PawnAllocator.Free(pCar);
}

void CarPawnTemplate::AddAIControl(ArmoredCar * pPawn)
{
	pPawn->m_pAIUnit = gAICommander->NewAIUnit(gArmoredCarAIControlType, pPawn);
	//设定初始状态为移动。
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

	//为车身主体创建碰撞盒。
	CollideBox* pRootBox 
		=	pPawn->m_arr_CBoxes[ COLLIDE_RECT_INDEX_CAR_ROOT ] 
		=	gCollideCommander->NewCollideBox(
				COLLIDE_TYPE_BOX_1,		//碰撞体类型标记
				pPawn->MainBody(),	//拥有碰撞体的ControlItem。
				pPawn);				//拥有碰撞体的Pawn对象。

	PawnType aft = pPawn->m_pawnType;

	if (pre != aft)
	{
		pPawn->m_pawnType = pre;
	}

	//修改碰撞盒的大小。
	pRootBox->Size.Xmin = -1.0f;	pRootBox->Size.Xmax = 1.0f;
	pRootBox->Size.Ymin = -0.8f;	pRootBox->Size.Ymax = 0.8f;
	pRootBox->Size.Zmin = -1.0f;	pRootBox->Size.Zmax = 1.0f;

	//重新计算包围盒的球体半径。
	pRootBox->CaculateRadius();
}

void CarPawnTemplate::DeleteAIControl(ArmoredCar * pPawn)
{
	gAICommander->DeleteAIUnit(pPawn->m_pAIUnit);
	pPawn->m_pAIUnit = nullptr;
}

void CarPawnTemplate::DeleteBones(ArmoredCar * pPawn)
{
	//遍历骨骼数组，删除所有骨骼。
	for (int i = 0; i < _countof(pPawn->m_arr_Bones); ++i)
	{
		//回收骨骼。
		gBoneCommander->DeleteBone(pPawn->m_arr_Bones[i]);
		//指针置空。
		pPawn->m_arr_Bones[i] = nullptr;
	}
}

void CarPawnTemplate::DeleteCollideBoxes(ArmoredCar * pPawn)
{
	gCollideCommander->DeleteCollideBox(pPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_CAR_ROOT]);

	pPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_CAR_ROOT] = nullptr;
}

//**************************装甲车AI行为模板******************************************************************

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
	aimFragment.ConsistTime = 1.0f;
	aimFragment.NextState = STORY_FRAGMENT_NEXT_RANDOM_OTHERS;

	//射击状态概览
	StoryFragment shoutFragment;
	shoutFragment.State = STORY_FRAGMENT_CAR_SHOUT;
	shoutFragment.Posibility = 0.1f;
	shoutFragment.ConsistTime = 2.0f;
	shoutFragment.NextState = STORY_FRAGMENT_NEXT_RANDOM;

	/*
	//死亡状态概览，随机死亡
	StoryFragment shoutFragment;
	shoutFragment.State = STORY_FRAGMENT_CAR_SHOUT;
	shoutFragment.Posibility = 0.01f;
	shoutFragment.ConsistTime = 0.0f;
	shoutFragment.NextState = STORY_FRAGMENT_NEXT_NONE;
	*/
	
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
		move(pCar, gt);
		break;

		//瞄准
	case STORY_FRAGMENT_CAR_AIM:
		aim(pCar, gt);
		break;

		//射击
	case STORY_FRAGMENT_CAR_SHOUT:
		shout(pCar, consumedTime, gt);
		break;

	default:
		ASSERT(false && "ArmoredCar处于非法状态，无法执行操作。");
		break;
	}
}

void CarAITemplate::move(ArmoredCar * pCar, const GameTimer& gt)
{
	auto pHead = PlayerPawn::PawnAllocator.GetHead();
	auto pNode = pHead->m_pNext;
	while (pNode != pHead)
	{
		//获取BasePawn指针。
		auto pPawn = &pNode->element;

		//转换为指定的类型。
		PlayerPawn* pPlayer = reinterpret_cast<PlayerPawn*>(pPawn);
		
		//获取根控制器。
		auto rootControl = pPlayer->RootControl();

		MoveToward(pCar, rootControl, gt);

		//待完善……
		//TODO

		pNode = pNode->m_pNext;
	}


}

void CarAITemplate::aim(ArmoredCar * pCar, const GameTimer& gt)
{
	//待实现……
	//TODO
}

void CarAITemplate::shout(ArmoredCar * pCar, float consumedTime, const GameTimer& gt)
{
	//待实现……
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

	//生成到指定位置的位移向量。
	targetV = gt.DeltaTime() 
		* speed 
		* XMVector4Normalize(targetV - currV);

	XMStoreFloat4(&curr, targetV);
	pCar
		->RootControl()
		->MoveXYZ(curr.x, curr.y, curr.z);

}
