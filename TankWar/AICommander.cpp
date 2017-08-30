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
	//��յ�Ԫ����
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
		//��ȡ��λָ�롣
		pUnit = &pNode->element;

		//����Ϊ��ǰ״̬���Ӽ�ʱ��
		pUnit->ConsumedTime += gt.DeltaTime();

		//����״̬��
		Navigate(pUnit, gt);

		//ִ�й��ܡ�
		AITemplateList[TO_ARRAY_INDEX(pUnit->Type) ]->Runing(
			pUnit->pPawn, pUnit->CurrState, pUnit->ConsumedTime, gt);

		//������
		pNode = pNode->m_pNext;
	}
}

//�Զ�����AI��Ԫ��״̬ת����
void AICommander::Navigate(AIUnit * pUnit, const GameTimer & gt)
{

	//��Ӧ��AI����ģ�����ͣ���ţ���
	auto cType = pUnit->Type;
	//��ǰ���ڵ�״̬
	auto currState = pUnit->CurrState;

	ASSERT(cType != AI_CONTROL_TYPE_NONE 
		&& cType <= static_cast<AIControlType>(AITemplateList.size()) 
		&& "AI���Ƶ�Ԫ�ο���AIģ�����ͷǷ�");

	//��ȡ���°�
	auto& storyBoard = AITemplateList[TO_ARRAY_INDEX(cType)]->StoryBoard;
	//��ǰ״̬���ڵĹ���Ƭ��
	auto& storyFragment = storyBoard[TO_ARRAY_INDEX(currState)];
	//��һ��״̬�ı�־��
	auto nextState = storyFragment.NextState;

	//��鵱ǰ״̬�Ƿ��Ѿ��ﵽ�涨ʱ�䡣
	if (pUnit->ConsumedTime > storyFragment.ConsistTime)
	{
		//������һ��״̬��
		switch (nextState)
		{
		case STORY_FRAGMENT_NONE:
			//״̬�Ƿ���
			//ע��STORY_FRAGMENT_NONE��STORY_FRAGMENT_NEXT_NONE�ǲ�ͬ�ģ�
			//ǰ�߱�ʾһ���Ƿ�״̬��
			//���߱�ʾû����һ��״̬�������ֵ�ǰ״̬��
			ASSERT(false && "AIģ�����һ��״̬�Ƿ���״̬ת��ʧ��");
			break;

		case STORY_FRAGMENT_NEXT_RANDOM:
			//���һ��״̬��
			pUnit->Update(ChooseRand(storyBoard));
			break;

		case STORY_FRAGMENT_NEXT_RANDOM_OTHERS:
			//���һ��״̬������һ״̬���ܻ��ǵ�ǰ״̬��
			pUnit->Update(ChooseRandWithOut(storyBoard, currState));
			break;

		case STORY_FRAGMENT_NEXT_NONE:
			//����һ��״̬�����ֵ�ǰ״̬��
			break;

		default:
			//����״̬��
			ASSERT(nextState > 0
				&& nextState <= static_cast<AIStatue>(storyBoard.size())
				&& "�ֶ�ָ�ɵ���һ��״ֵ̬������ѡ��ķ�Χ���뽫״ֵ̬�޶���StoryBoard����֮��");
			//���µ����°���ָ����һ��״̬��
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

	//����ÿһ������Ƭ�Σ�Ϊÿһ������Ƭ������һ��0~9�����ֵ������randDispatch��
	//���randDispatch���Թ���Ƭ�����������Posibility��
	//���س˻������Ǹ�����Ƭ�δ����AI״̬��
	for (auto& fragment : storyBoard)
	{
		randDispatch = 1.0f * ( rand() % 10 ) * fragment.Posibility;

		if (randDispatch > maxPosibility)
		{
			returnedState = fragment.State;
			maxPosibility = randDispatch;
		}
	}

	ASSERT(returnedState && "���״̬�ķ���ֵ�ǷǷ�״̬");
	return returnedState;
}

AIStatue AICommander::ChooseRandWithOut(std::vector<StoryFragment>& storyBoard, AIStatue withOut)
{
	AIStatue returnedState = STORY_FRAGMENT_NONE;
	float randDispatch = 0.1f;
	float maxPosibility = -0.1f;

	//����ÿһ������Ƭ�Σ�Ϊÿһ������Ƭ������һ��0~9�����ֵ������randDispatch��
	//���randDispatch���Թ���Ƭ�����������Posibility��
	//���س˻������Ǹ�����Ƭ�δ����AI״̬��
	for (auto& fragment : storyBoard)
	{
		//���������Ե�״̬��
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

	ASSERT(returnedState && "���״̬�ķ���ֵ�ǷǷ�״̬");
	return returnedState;
}

void AIUnit::Update(AIStatue newState)
{
	ASSERT(newState && "AI����״̬�ǷǷ�״̬������ʧ��");
	this->CurrState = newState;
	//������ʱ����0��Ϊ��ǰ״̬���¼�ʱ��
	this->ConsumedTime = 0.0f;
}
