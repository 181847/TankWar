#pragma once
#include "BasePawn.h"
#include "../../../../Common/GameTimer.h"

//控制模板类型
typedef unsigned int TemplateType;
#define TEMPLATE_TYPE_NONE 0

#define TO_ARR_INDEX(templateType) (templateType - 1)

struct PerFrameUnit
{
	TemplateType type;
	BasePawn* pPawn;
};

class PerFrameTemplate
{
public:
	virtual void PerFrameEvent(BasePawn* pPawn, const GameTimer& gt) = 0;
};

//每一帧执行功能。
class PerFrameCommander
{
public:
	PerFrameCommander();
	PerFrameCommander(const PerFrameCommander&) = delete;
	PerFrameCommander& operator = (const PerFrameCommander&) = delete;
	~PerFrameCommander();

	PerFrameUnit* NewUnit(BasePawn* pPawn, TemplateType type);
	void DeleteUnit(PerFrameUnit* pUnit);

	void Executee(const GameTimer& gt);

	//控制单元。
	LinkedAllocator<PerFrameUnit> UnitAllocator;
	//模板列表。
	std::vector<std::unique_ptr<PerFrameTemplate>> TemplateList;
};

