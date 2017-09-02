#include "PerFrameCommander.h"



PerFrameCommander::PerFrameCommander()
{
}


PerFrameCommander::~PerFrameCommander()
{
}

PerFrameUnit * PerFrameCommander::NewUnit(BasePawn * pPawn, TemplateType type)
{
	auto newUnit = UnitAllocator.Malloc();
	newUnit->pPawn = pPawn;
	newUnit->type = type;
	return newUnit;
}

void PerFrameCommander::DeleteUnit(PerFrameUnit * pUnit)
{
	pUnit->pPawn = nullptr;
	pUnit->type = TEMPLATE_TYPE_NONE;

	UnitAllocator.Free(pUnit);
}

void PerFrameCommander::Executee(const GameTimer & gt)
{
	auto pHead = UnitAllocator.GetHead();
	auto pNode = pHead->m_pNext;

	TemplateType type = TEMPLATE_TYPE_NONE;
	BasePawn* pPawn = nullptr;

	while (pNode != pHead)
	{
		type = pNode->element.type;
		pPawn = pNode->element.pPawn;

		ASSERT(type && pPawn);

		TemplateList[TO_ARR_INDEX(type)]->PerFrameEvent(pPawn, gt);

		pNode = pNode->m_pNext;
	}
}
