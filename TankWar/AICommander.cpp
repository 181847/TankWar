#include "AICommander.h"
#include "BasePawn.h"



AICommander::AICommander(UINT maxControlableUnitCount)
	:UnitList(maxControlableUnitCount)
{
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
	return newUnit;
}
