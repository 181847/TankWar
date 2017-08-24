#include "PlayerCommander.h"



PlayerCommander::PlayerCommander(UINT maxControlUnitNum)
	:UnitAllocator(maxControlUnitNum)
{
	keyStates.push_back(KeyState(KeyType::W));
	keyStates.push_back(KeyState(KeyType::A));
	keyStates.push_back(KeyState(KeyType::S));
	keyStates.push_back(KeyState(KeyType::D));
	keyStates.push_back(KeyState(KeyType::M_L));	//������
	keyStates.push_back(KeyState(KeyType::M_R));	//����Ҽ�
}


PlayerCommander::~PlayerCommander()
{
}

PlayerControlType PlayerCommander::AddCommandTemplate(
	std::unique_ptr<ControlCommandTemplate> pCommandTemplate)
{
	//��¼����ģ�塣
	CommandTemplateList.push_back(std::move(pCommandTemplate));
	//�����������ģ��ı�ǣ�����ǰ����Ĵ�С��
	//��Ϊ���vector����ɾ����ֻ����ӡ�
	return CommandTemplateList.size();
}

PlayerControlUnit * PlayerCommander::NewPlayerControl(
	PlayerControlType controlType, BasePawn * pControledPawn)
{
	//������Ƶ�Ԫ��
	auto pNewUnit = UnitAllocator.Malloc();
	//��д��Ϣ��
	pNewUnit->ControlType = controlType;
	pNewUnit->pControledPawn = pControledPawn;
	return pNewUnit;
}

void PlayerCommander::DeletePlayerControl(PlayerControlUnit * pTheUnit)
{
	//��տ��Ƶ�Ԫ���ݡ�
	pTheUnit->ControlType = PLAYER_CONTROL_TYPE_NONE;
	pTheUnit->pControledPawn = nullptr;

	//�ջط���ء�
	UnitAllocator.Free(pTheUnit);
}
void PlayerCommander::DetactKeyState()
{
	//�������а�����״̬��
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

	//��������ѭ������
	while (pNode != pHead)
	{
		//���ݰ���״ִ̬��������Ƶ�Ԫ�����
		ExecuteeCommandTeplate(&pNode->element, gt);

		//������
		pNode = pNode->m_pNext;
	}
}

void PlayerCommander::ExecuteeCommandTeplate(
	PlayerControlUnit * pUnit, const GameTimer& gt)
{
	//��ȡ����ģ�壬ע��ControlType��0��ʾ�Ƿ������Ҫ�ֶ���һ���Ƕ�Ӧ�����е�Ԫ�ء�
	auto* pCTemplate = CommandTemplateList[pUnit->ControlType - 1].get();

	//�������Ƿ��ƶ�����ֻ��ע��ǰ״̬��������λΪ1ʱ��ʾ��ǰ�����ƶ���
	if (mouseState.moveState & 0x0f)
	{
		//ִ��MouseMove��
		pCTemplate->MouseMove(pUnit->pControledPawn, mouseState, gt);
		//ִ���궯���󣬽����״̬����Ϊ��ֹ����ʾ���Ĵ˴ζ�����ɣ�
		//���ھ�ֹ�ȴ���һ�ζ���������Ϊ�˷�ֹ��
		//{���û�ж�ʱ�����״̬���䣬����һ����Ȼ���ж���궮�ˣ�Ȼ��ʵ������겢û���ƶ�
		//ֻ����һ�ε����״̬û�иı�}
		STATE_CHANGE(mouseState.moveState, STOP);
	}

	//�������а���״̬������������ڰ���״̬��ִ����Ӧ�ĺ�����
	for (auto& keyState : keyStates)
	{
		//Ŀǰֻ������λ��������������λ��ȫΪ0����ʾ��ǰ����״̬Ϊ���£�ָ���Ӧ������
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
				ASSERT(false && "�Ƿ��İ����������");
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
		(stateChange << 4 & 0xf0)	//ԭ״̬ǰ��4λ�������ƶ����ǰ4λ
		| (newState & 0x0f);		//ȡ��״̬�ĺ���λ����ǰ���״̬������
}
