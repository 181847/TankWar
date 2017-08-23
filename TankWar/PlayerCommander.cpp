#include "PlayerCommander.h"



PlayerCommander::PlayerCommander(UINT maxControlUnitNum)
	:UnitAllocator(maxControlUnitNum)
{
}


PlayerCommander::~PlayerCommander()
{
}

PlayerControlType PlayerCommander::AddCommandTemplate(
	std::unique_ptr<ControlCommandTemplate> pCommandTemplate)
{
	//记录控制模板。
	CommandTemplateList.push_back(std::move(pCommandTemplate));
	//返回这个控制模板的标记，即当前数组的大小，
	//因为这个vector不会删减，只能添加。
	return CommandTemplateList.size();
}

PlayerControlUnit * PlayerCommander::NewPlayerControl(
	PlayerControlType controlType, BasePawn * pControledPawn)
{
	//申请控制单元。
	auto pNewUnit = UnitAllocator.Malloc();
	//填写信息。
	pNewUnit->ControlType = controlType;
	pNewUnit->pControledPawn = pControledPawn;
	return pNewUnit;
}

void PlayerCommander::DeletePlayerControl(PlayerControlUnit * pTheUnit)
{
	//清空控制单元数据。
	pTheUnit->ControlType = PLAYER_CONTROL_TYPE_NONE;
	pTheUnit->pControledPawn = nullptr;

	//收回分配池。
	UnitAllocator.Free(pTheUnit);
}
void PlayerCommander::DetactKeyState()
{
	//更新所有按键的状态。
	for (auto& keyState : keyStates)
	{
		if (GetAsyncKeyState(keyState.keyType) & 0x8000)
		{
			keyState.ChangeState(PRESS);
		}
		else
		{
			keyState.ChangeState(RELEASE);
		}
	}
}

void PlayerCommander::Executee(const GameTimer& gt)
{
	//查看当前APP是否获得鼠标焦点。
	if ( ! mouseState.IsCaptured) 
	{
		//鼠标没有获得焦点，不能执行任何命令。
		return;
	}

	auto pHead = UnitAllocator.GetHead();
	auto pNode = pHead->m_pNext;

	//遍历整个循环链表
	while (pNode != pHead)
	{
		//根据按键状态执行这个控制单元的命令。
		ExecuteeCommandTeplate(&pNode->element, gt);

		//迭代。
		pNode = pNode->m_pNext;
	}
}

void PlayerCommander::ExecuteeCommandTeplate(
	PlayerControlUnit * pUnit, const GameTimer& gt)
{
	//获取控制模板，注意ControlType的0表示非法命令，需要手动减一才是对应数组中的元素。
	auto pCTemplate = CommandTemplateList[pUnit->ControlType - 1].get();

	//执行MouseMove。
	pCTemplate->MouseMove(pUnit->pControledPawn, mouseState, gt);

	//遍历所有按键状态，如果按键处于按下状态，执行相应的函数。
	for (auto& keyState : keyStates)
	{
		//目前只看后四位的情况，如果后四位不全为0，表示当前按键状态为按下，指向对应函数。
		if (keyState.stateChange & 0x0f)
		{
			switch (keyState.keyType)
			{
			case W:
				pCTemplate->HitKey_W(pUnit->pControledPawn, gt);
				break;
			case A:
				pCTemplate->HitKey_A(pUnit->pControledPawn, gt);
				break;
			case S:
				pCTemplate->HitKey_S(pUnit->pControledPawn, gt);
				break;
			case D:
				pCTemplate->HitKey_D(pUnit->pControledPawn, gt);
				break;
			case M_L:
				pCTemplate->PressMouseButton_Left(pUnit->pControledPawn, gt);
				break;
			case M_R:
				pCTemplate->PressMouseButton_Right(pUnit->pControledPawn, gt);
				break;

			default:
				ASSERT(false && "非法的按键类型命令。");
				break;
			}
		}
	}
}
