#pragma once
#include "BasePawn.h"
#include "../../../../Common/GameTimer.h"

//����ģ������
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

//ÿһִ֡�й��ܡ�
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

	//���Ƶ�Ԫ��
	LinkedAllocator<PerFrameUnit> UnitAllocator;
	//ģ���б�
	std::vector<std::unique_ptr<PerFrameTemplate>> TemplateList;
};

