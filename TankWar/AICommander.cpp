#include "AICommander.h"



AICommander::AICommander(UINT maxControlableUnitCount)
	:UnitList(maxControlableUnitCount)
{
	srand(AI_CONTROL_RAND_SEED);
}


AICommander::~AICommander()
{
}

AIControlType AICommander::AddAITemplate(std::unique_ptr<AITemplate> pTempalte)
{
	AITemplateList.push_back(std::move(pTempalte));
	return static_cast<AIControlType>(AITemplateList.size());
}

AIUnit * AICommander::NewAIUnit(AIControlType type, BasePawn * pPawn)
{
	auto newUnit = UnitList.Malloc();
	newUnit->pPawn = pPawn;
	newUnit->Type = type;
	newUnit->ConsumedTime = 0.0f;
	return newUnit;
}

void AICommander::DeleteAIUnit(AIUnit * pUnit)
{
	//清空单元数据
	pUnit->CurrState = STORY_FRAGMENT_NONE;
	pUnit->pPawn = nullptr;
	pUnit->Type = AI_CONTROL_TYPE_NONE;
	pUnit->ConsumedTime = 0.0f;

	UnitList.Free(pUnit);
}

void AICommander::AutoNavigate(const GameTimer & gt)
{
	auto pHead = UnitList.GetHead();
	auto pNode = pHead->m_pNext;
	AIUnit * pUnit;

	while (pNode != pHead)
	{
		//获取单位指针。
		pUnit = &pNode->element;

		//首先为当前状态增加计时。
		pUnit->ConsumedTime += gt.DeltaTime();

		//更新状态。
		Navigate(pUnit, gt);

		//执行功能。
		AITemplateList[TO_ARRAY_INDEX(pUnit->Type) ]->Runing(
			pUnit->pPawn, pUnit->CurrState, pUnit->ConsumedTime, gt);

		//迭代。
		pNode = pNode->m_pNext;
	}
}

//自动控制AI单元的状态转换。
void AICommander::Navigate(AIUnit * pUnit, const GameTimer & gt)
{

	//对应的AI控制模板类型（序号）。
	auto cType = pUnit->Type;
	//当前处于的状态
	auto currState = pUnit->CurrState;

	ASSERT(cType != AI_CONTROL_TYPE_NONE 
		&& cType <= static_cast<AIControlType>(AITemplateList.size()) 
		&& "AI控制单元参考的AI模板类型非法");

	//提取故事板
	auto& storyBoard = AITemplateList[TO_ARRAY_INDEX(cType)]->StoryBoard;
	//当前状态处于的故事片段
	auto& storyFragment = storyBoard[TO_ARRAY_INDEX(currState)];
	//下一个状态的标志。
	auto nextState = storyFragment.NextState;

	//检查当前状态是否已经达到规定时间。
	if (pUnit->ConsumedTime > storyFragment.ConsistTime)
	{
		//搜索下一个状态。
		switch (nextState)
		{
		case STORY_FRAGMENT_NONE:
			//状态非法。
			//注意STORY_FRAGMENT_NONE和STORY_FRAGMENT_NEXT_NONE是不同的，
			//前者表示一个非法状态，
			//后者表示没有下一个状态，即保持当前状态。
			ASSERT(false && "AI模板的下一个状态非法，状态转换失败");
			break;

		case STORY_FRAGMENT_NEXT_RANDOM:
			//随机一个状态。
			pUnit->Update(ChooseRand(storyBoard));
			break;

		case STORY_FRAGMENT_NEXT_RANDOM_OTHERS:
			//随机一个状态，但下一状态不能还是当前状态。
			pUnit->Update(ChooseRandWithOut(storyBoard, currState));
			break;

		case STORY_FRAGMENT_NEXT_NONE:
			//无下一个状态，保持当前状态。
			break;

		default:
			//其他状态。
			ASSERT(nextState > 0
				&& nextState <= static_cast<AIStatue>(storyBoard.size())
				&& "手动指派的下一个状态值超出可选择的范围，请将状态值限定在StoryBoard数组之内");
			//更新到故事板中指定的一个状态。
			pUnit->Update(nextState);
			break;
		}
	}

	
}

AIStatue AICommander::ChooseRand(std::vector<StoryFragment>& storyBoard)
{
	AIStatue returnedState = STORY_FRAGMENT_NONE;
	float randDispatch = 0.1f;
	float maxPosibility = -0.1f;

	//遍历每一个故事片段，为每一个故事片段生成一个0~9的随机值，称作randDispatch，
	//这个randDispatch乘以故事片段自身的属性Posibility，
	//返回乘积最大的那个故事片段代表的AI状态。
	for (auto& fragment : storyBoard)
	{
		randDispatch = 1.0f * ( rand() % 10 ) * fragment.Posibility;

		if (randDispatch > maxPosibility)
		{
			returnedState = fragment.State;
			maxPosibility = randDispatch;
		}
	}

	ASSERT(returnedState && "随机状态的返回值是非法状态");
	return returnedState;
}

AIStatue AICommander::ChooseRandWithOut(std::vector<StoryFragment>& storyBoard, AIStatue withOut)
{
	AIStatue returnedState = STORY_FRAGMENT_NONE;
	float randDispatch = 0.1f;
	float maxPosibility = -0.1f;

	//遍历每一个故事片段，为每一个故事片段生成一个0~9的随机值，称作randDispatch，
	//这个randDispatch乘以故事片段自身的属性Posibility，
	//返回乘积最大的那个故事片段代表的AI状态。
	for (auto& fragment : storyBoard)
	{
		//跳过被忽略的状态。
		if (fragment.State == withOut)
		{
			continue;
		}

		randDispatch = 1.0f * (rand() % 10) * fragment.Posibility;

		if (randDispatch * fragment.Posibility > maxPosibility)
		{
			returnedState = fragment.State;
		}
	}

	ASSERT(returnedState && "随机状态的返回值是非法状态");
	return returnedState;
}

void AIUnit::Update(AIStatue newState)
{
	ASSERT(newState && "AI的新状态是非法状态，更新失败");
	this->CurrState = newState;
	//已消耗时间置0，为当前状态重新计时。
	this->ConsumedTime = 0.0f;
}
