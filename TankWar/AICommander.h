#pragma once

//AI�������ͣ���ӦCommandTemplate�����е�һ��Ԫ�أ�ע�����ֵ���Ϊ0��ʾ���ͷǷ���
typedef unsigned int AIControlType;
#define AI_CONTROL_TYPE_NONE 0

//AI״̬��ǩ��ÿ��AI����ģ�嶼���Լ���һ��״̬����Щ״̬��Ӧ�Ź���Ƭ�������е����飬
//�������ֵ����0��ʱ���ʾ�������ţ�
//С��-1��ʱ���ʾ״̬ת���ķ�ʽ��
//ע�⣺-1��ʾ�Ƿ�״̬��
typedef int AIStatue;

//�Ƿ�״̬
#define STORY_FRAGMENT_NONE -1
//���ת��״̬��
#define STORY_FRAGMENT_NEXT_RANDOM -2
//���ת��״̬��������һ��״̬���ܻ��ǵ�ǰ״̬��
#define STORY_FRAGMENT_NEXT_RANDOM_OTHERS -3

struct AIUnit
{
	AIControlType Type;
	AIStatue CurrState;
	BasePawn * pPawn;
};

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
};

//����Ƭ��
struct StoryFragment
{
	//״̬��־�����ֵͬʱ��Ϊ�������ţ�-1��ʾ״̬�Ƿ���
	AIStatue State = STORY_FRAGMENT_NONE;
	//���ת�����״̬�Ŀ����ԡ�
	float Posibility = 0.0f;
	//��״̬��ά��ʱ�䡣
	float ConsistTime = 0.0f;
	//��һ��״̬��
	AIStatue NextState = STORY_FRAGMENT_NONE;
};

class AICommander
{
public:
	//maxControlableUnitCount���ɿ��Ƶ�AI������
	AICommander(UINT maxControlableUnitCount);
	AICommander(const AICommander&) = delete;
	AICommander& operator = (const AICommander&) = delete;
	~AICommander();

	//��ӿ���ģ�壬����ģ�����ͱ�־��ע��������ص����ͱ�־��ʵ��������ģ��֮������ģ��Ĵ�С��
	AIControlType AddAITemplate(std::unique_ptr<AITemplate> pTempalte);

	//�µĿ��Ƶ�λ��
	AIUnit* NewAIUnit(AIControlType type, BasePawn* pPawn);
	void DeleteAIUnit(AIUnit * pUnit);

	//״̬ת����⣬��Ե�ǰ״ִ̬����Ӧ��ָ�
	void AutoNavigate(const GameTimer& gt);

protected:
	//AIģ���б�
	std::vector<std::unique_ptr<AITemplate>> AITemplateList;
	//���Ƶ�Ԫ�б�
	LinkedAllocator<AIUnit> UnitList;
};

