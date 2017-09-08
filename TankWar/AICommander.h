#pragma once
#include "Common/GameTimer.h"
#include <vector>
#include <memory.h>

#define ASSERTIONS_ENABLED 1

#include "MyAssert.h"
#include "BasePawn.h"

//������ǩ����ͬʱ�������������ţ�������0һ�㱻���Ϊ�Ƿ���ǣ�
//�����ʹ����Щ������Ϊ���֮ǰ����Ҫ��1����������Ӧ�����е���š�
#define TO_ARRAY_INDEX(type_great_than_0) (type_great_than_0 - 1)

//������AI��״̬ת���д������ת��������������������������������С�
#define AI_CONTROL_RAND_SEED 1

//AI�������ͣ���ӦCommandTemplate�����е�һ��Ԫ�أ�ע�����ֵ���Ϊ0��ʾ���ͷǷ���
typedef unsigned int AIControlType;
#define AI_CONTROL_TYPE_NONE 0

//AI״̬��ǩ��ÿ��AI����ģ�嶼���Լ���һ��״̬����Щ״̬��Ӧ�Ź���Ƭ�������е����飬
//�������ֵ����0��ʱ���ʾ�������ţ�
//С��-1��ʱ���ʾ״̬ת���ķ�ʽ��
//ע�⣺0��ʾ�Ƿ�״̬����ͨ��AIStatue��Ϊ�������Ż�ȡStoryBoard�е�Ԫ��ʱ����Ҫ�ֶ���1.
typedef int AIStatue;
//�Ƿ�״̬
#define STORY_FRAGMENT_NONE 0
//���ת��״̬��
#define STORY_FRAGMENT_NEXT_RANDOM -1
//���ת��״̬��������һ��״̬���ܻ��ǵ�ǰ״̬��
#define STORY_FRAGMENT_NEXT_RANDOM_OTHERS -2
//û����һ��״̬��AI��״̬�����ᱻAICommander�Զ����£������ʹ�ó���Ա�ֶ�����״̬�ĸı䡣
#define STORY_FRAGMENT_NEXT_NONE -3

struct AIUnit
{
	//AI����ģ�����ͣ����ֵ������AI����Ϊģʽ��
	AIControlType Type;
	//��ǰ״̬�����״̬��AICommander�Զ����ݿ���ģ��������Զ����¡�
	AIStatue CurrState;
	//���ٿص�Pawn����
	BasePawn * pPawn;
	//��ǰ״̬�Ѿ����ĵ�ʱ�䣬ÿת����һ���µ�״̬�����ֵ��������0.
	float ConsumedTime;

public:
	//���µ�ǰ״̬�����ҽ������ĵ�ʱ����0��
	void Update(AIStatue newState);
};

//����Ƭ��
struct StoryFragment
{
	//״̬��־�����ֵͬʱ��Ϊ�������ţ�0��ʾ״̬�Ƿ���
	AIStatue State = STORY_FRAGMENT_NONE;
	//���ת�����״̬�Ŀ����ԡ�
	float Posibility = 0.0f;
	//��״̬��ά��ʱ�䡣
	float ConsistTime = 0.0f;
	//��һ��״̬��
	AIStatue NextState = STORY_FRAGMENT_NONE;
};

//ǰ��������
class AICommander;
class AITemplate
{
public:
	//���ݵ�ǰAI��״̬��ִ����Ҫ�Ŀ��Ʒ�����
	virtual void Runing(
		BasePawn* pPawn, AIStatue state,
		float consumedTime, const GameTimer& gt) = 0;

protected:
	//���°壬�洢�����й��µ�״̬���Լ�״̬ת���ķ�����
	std::vector<StoryFragment> StoryBoard;

protected:
	friend class AICommander;
};

class AICommander
{
public:
	//maxControlableUnitCount���ɿ��Ƶ�AI������
	AICommander(UINT maxControlableUnitCount);
	AICommander(const AICommander&) = delete;
	AICommander& operator = (const AICommander&) = delete;
	~AICommander();

	//��ӿ���ģ�壬����ģ�����ͱ�־��ע��������ص����ͱ�־��ʵ��������ģ��֮������ģ���������
	AIControlType AddAITemplate(std::unique_ptr<AITemplate> pTempalte);

	//�µĿ��Ƶ�λ��
	AIUnit* NewAIUnit(AIControlType type, BasePawn* pPawn);
	void DeleteAIUnit(AIUnit * pUnit);

	//״̬ת����⣬��Ե�ǰ״ִ̬����Ӧ��ָ�
	void AutoNavigate(const GameTimer& gt);
	//���ݶ�Ӧ��AIģ�壬�Զ�����ָ��AI��Ԫ��״̬����¼AI��ǰ״̬�Ѿ�������ʱ�䳤�ȡ�
	void Navigate(AIUnit * unit, const GameTimer& gt);

protected:
	//��ָ���Ĺ��°��а��ո��ʣ����ѡ��һ��״̬��־��
	AIStatue ChooseRand(std::vector<StoryFragment>& storyBoard);
	//��ָ���Ĺ��°��а��ո��ʣ����ѡ��һ��״̬��־�����Ƿ��ص�״̬�����ǲ���ָ���ġ�withOut����
	AIStatue ChooseRandWithOut(std::vector<StoryFragment>& storyBoard, AIStatue withOut);

protected:
	//AIģ���б�
	std::vector<std::unique_ptr<AITemplate>> AITemplateList;
	//���Ƶ�Ԫ�б�
	LinkedAllocator<AIUnit> UnitList;

};


