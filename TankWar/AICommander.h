#pragma once

//AI控制类型，对应CommandTemplate数组中的一个元素，注意这个值如果为0表示类型非法。
typedef unsigned int AIControlType;
#define AI_CONTROL_TYPE_NONE 0

//AI状态标签，每种AI控制模板都有自己的一套状态，这些状态对应着故事片段数组中的数组，
//所以这个值大于0的时候表示数组的序号，
//小于-1的时候表示状态转换的方式，
//注意：-1表示非法状态。
typedef int AIStatue;

//非法状态
#define STORY_FRAGMENT_NONE -1
//随机转换状态。
#define STORY_FRAGMENT_NEXT_RANDOM -2
//随机转换状态，但是下一个状态不能还是当前状态。
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
	//根据当前AI的状态，执行需要的控制方法。
	virtual void Runing(
		BasePawn* pPawn, AIStatue state,
		float consumedTime, const GameTimer& gt) = 0;

protected:
	//故事板，存储了所有故事的状态，以及状态转换的方法。
	std::vector<StoryFragment> StoryBoard;
};

//故事片段
struct StoryFragment
{
	//状态标志，这个值同时作为数组的序号，-1表示状态非法。
	AIStatue State = STORY_FRAGMENT_NONE;
	//随机转到这个状态的可能性。
	float Posibility = 0.0f;
	//此状态的维持时间。
	float ConsistTime = 0.0f;
	//下一个状态。
	AIStatue NextState = STORY_FRAGMENT_NONE;
};

class AICommander
{
public:
	//maxControlableUnitCount最多可控制的AI数量。
	AICommander(UINT maxControlableUnitCount);
	AICommander(const AICommander&) = delete;
	AICommander& operator = (const AICommander&) = delete;
	~AICommander();

	//添加控制模板，返回模板类型标志，注意这个返回的类型标志其实就是增加模板之后所有模板的大小。
	AIControlType AddAITemplate(std::unique_ptr<AITemplate> pTempalte);

	//新的控制单位。
	AIUnit* NewAIUnit(AIControlType type, BasePawn* pPawn);
	void DeleteAIUnit(AIUnit * pUnit);

	//状态转换检测，针对当前状态执行相应的指令。
	void AutoNavigate(const GameTimer& gt);

protected:
	//AI模板列表。
	std::vector<std::unique_ptr<AITemplate>> AITemplateList;
	//控制单元列表。
	LinkedAllocator<AIUnit> UnitList;
};

