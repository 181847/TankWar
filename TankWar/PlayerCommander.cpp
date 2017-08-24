#include "PlayerCommander.h"



PlayerCommander::PlayerCommander(UINT maxControlUnitNum)
	:UnitAllocator(maxControlUnitNum)
{
	keyStates.push_back(KeyState(KeyType::W));
	keyStates.push_back(KeyState(KeyType::A));
	keyStates.push_back(KeyState(KeyType::S));
	keyStates.push_back(KeyState(KeyType::D));
	keyStates.push_back(KeyState(KeyType::M_L));	//鼠标左键
	keyStates.push_back(KeyState(KeyType::M_R));	//鼠标右键
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
	auto* pCTemplate = CommandTemplateList[pUnit->ControlType - 1].get();

	//检查鼠标是否移动过，只关注当前状态，即后四位为1时表示当前正在移动。
	if (mouseState.moveState & 0x0f)
	{
		//执行MouseMove。
		pCTemplate->MouseMove(pUnit->pControledPawn, mouseState, gt);
		//执行完动作后，将鼠标状态更新为静止，表示鼠标的此次动作完成，
		//现在静止等待下一次动作，这是为了防止：
		//{鼠标没有动时，鼠标状态不变，则下一次仍然会判断鼠标懂了，然而实际上鼠标并没有移动
		//只是上一次的鼠标状态没有改变}
		STATE_CHANGE(mouseState.moveState, STOP);
	}

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

void MouseState::UpdateLocation(LONG newX, LONG newY)
{
	this->LastMousePos = this->CurrMousePos;
	this->CurrMousePos.x = newX;
	this->CurrMousePos.y = newY;

	STATE_CHANGE(moveState, MOVE);
}

KeyState::KeyState(KeyType type)
{
	this->keyType = type;
}

void KeyState::ChangeState(StateChange newState)
{
	ASSERT(newState == RELEASE || newState == PRESS);
	stateChange =
		(stateChange << 4 & 0xf0)	//原状态前移4位，保留移动后的前4位
		| (newState & 0x0f);		//取新状态的后四位，与前面的状态与运算
}
