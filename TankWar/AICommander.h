#pragma once
#include "Common/GameTimer.h"
#include <vector>
#include <memory.h>

#define ASSERTIONS_ENABLED 1

#include "MyAssert.h"
#include "BasePawn.h"

//有许多标签属性同时被用作数组的序号，而其中0一般被标记为非法标记，
//因而在使用这些属性作为序号之前，需要减1才能真正对应数组中的序号。
#define TO_ARRAY_INDEX(type_great_than_0) (type_great_than_0 - 1)

//由于在AI的状态转换中存在随机转换的情况，这里用随机种子设置随机序列。
#define AI_CONTROL_RAND_SEED 1

//AI控制类型，对应CommandTemplate数组中的一个元素，注意这个值如果为0表示类型非法。
typedef unsigned int AIControlType;
#define AI_CONTROL_TYPE_NONE 0

//AI状态标签，每种AI控制模板都有自己的一套状态，这些状态对应着故事片段数组中的数组，
//所以这个值大于0的时候表示数组的序号，
//小于-1的时候表示状态转换的方式，
//注意：0表示非法状态，当通过AIStatue作为数组的序号获取StoryBoard中的元素时，需要手动减1.
typedef int AIStatue;
//非法状态
#define STORY_FRAGMENT_NONE 0
//随机转换状态。
#define STORY_FRAGMENT_NEXT_RANDOM -1
//随机转换状态，但是下一个状态不能还是当前状态。
#define STORY_FRAGMENT_NEXT_RANDOM_OTHERS -2
//没有下一个状态，AI的状态将不会被AICommander自动更新，这可以使得程序员手动控制状态的改变。
#define STORY_FRAGMENT_NEXT_NONE -3

struct AIUnit
{
	//AI控制模板类型，这个值将决定AI的行为模式。
	AIControlType Type;
	//当前状态，这个状态由AICommander自动根据控制模板的设置自动更新。
	AIStatue CurrState;
	//被操控的Pawn对象。
	BasePawn * pPawn;
	//当前状态已经消耗的时间，每转换到一个新的状态，这个值都将被清0.
	float ConsumedTime;

public:
	//更新当前状态，并且将已消耗的时间置0。
	void Update(AIStatue newState);
};

//故事片段
struct StoryFragment
{
	//状态标志，这个值同时作为数组的序号，0表示状态非法。
	AIStatue State = STORY_FRAGMENT_NONE;
	//随机转到这个状态的可能性。
	float Posibility = 0.0f;
	//此状态的维持时间。
	float ConsistTime = 0.0f;
	//下一个状态。
	AIStatue NextState = STORY_FRAGMENT_NONE;
};

//前置声明。
class AICommander;
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

protected:
	friend class AICommander;
};

class AICommander
{
public:
	//maxControlableUnitCount最多可控制的AI数量。
	AICommander(UINT maxControlableUnitCount);
	AICommander(const AICommander&) = delete;
	AICommander& operator = (const AICommander&) = delete;
	~AICommander();

	//添加控制模板，返回模板类型标志，注意这个返回的类型标志其实就是增加模板之后所有模板的数量。
	AIControlType AddAITemplate(std::unique_ptr<AITemplate> pTempalte);

	//新的控制单位。
	AIUnit* NewAIUnit(AIControlType type, BasePawn* pPawn);
	void DeleteAIUnit(AIUnit * pUnit);

	//状态转换检测，针对当前状态执行相应的指令。
	void AutoNavigate(const GameTimer& gt);
	//根据对应的AI模板，自动更新指定AI单元的状态，记录AI当前状态已经经过的时间长度。
	void Navigate(AIUnit * unit, const GameTimer& gt);

protected:
	//在指定的故事板中按照概率，随机选择一个状态标志。
	AIStatue ChooseRand(std::vector<StoryFragment>& storyBoard);
	//在指定的故事板中按照概率，随机选择一个状态标志，但是返回的状态不会是参数指定的“withOut”。
	AIStatue ChooseRandWithOut(std::vector<StoryFragment>& storyBoard, AIStatue withOut);

protected:
	//AI模板列表。
	std::vector<std::unique_ptr<AITemplate>> AITemplateList;
	//控制单元列表。
	LinkedAllocator<AIUnit> UnitList;

};


