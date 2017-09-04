#include "PawnMaster.h"



PawnMaster::PawnMaster(UINT MaxCommandNum, Scence* pScence)
	:CommandBuffer(MaxCommandNum)
{
	m_pScence = pScence;
}


PawnMaster::~PawnMaster()
{
}

PawnType PawnMaster::AddCommandTemplate(std::unique_ptr<PawnCommandTemplate> pCommandTemplate)
{
	CommandTemplateList.push_back(std::move(pCommandTemplate));
	//ע�⣬PawnTypeΪ0��ʾû�����ͣ����ܷ���0��
	return CommandTemplateList.size();
}

void PawnMaster::CreatePawn(PawnType pawnType, PawnProperty* pProperty)
{
	//��¼�������͡�
	CommandBuffer[UnExecuteeCommandNum].CommandType = 
		PAWN_COMMAND_TYPE_CREATE;
	//����Pawn�����͡�
	CommandBuffer[UnExecuteeCommandNum].CreateCommand.pawnType = 
		pawnType;
	//Pawn�����ԡ�
	CommandBuffer[UnExecuteeCommandNum].CreateCommand.pProperty = 
		pProperty;

	//δִ������������һ��
	++UnExecuteeCommandNum;
}

void PawnMaster::DestroyPawn(PawnType pawnType, BasePawn * pThePawn)
{
	//��¼�������͡�
	CommandBuffer[UnExecuteeCommandNum].CommandType =
		PAWN_COMMAND_TYPE_DESTORY;

	if (pThePawn->m_pawnType > 50)
	{
		CommandBuffer[UnExecuteeCommandNum].DestoryCommand.pawnType =
			pThePawn->m_pawnType;
	}

	//��¼Ҫɾ����Pawn���͡�
	CommandBuffer[UnExecuteeCommandNum].DestoryCommand.pawnType =
		pThePawn->m_pawnType;

	//��¼Ҫɾ����Pawnָ�롣
	CommandBuffer[UnExecuteeCommandNum].DestoryCommand.pPawn_to_destory = 
		pThePawn;

	//δִ������������һ��
	++UnExecuteeCommandNum;
}

void PawnMaster::Executee()
{
	for (UINT i = 0; i < UnExecuteeCommandNum; ++i)
	{
		switch (CommandBuffer[i].CommandType)
		{
		case PAWN_COMMAND_TYPE_CREATE:
			ASSERT(CommandBuffer[i].CreateCommand.pawnType);
			CreatingPawn(i);

			//��յ�ǰ������Ϣ��
			CommandBuffer[i].CreateCommand.pawnType = PAWN_TYPE_NONE;
			CommandBuffer[i].CreateCommand.pProperty = nullptr;
			break;

		case PAWN_COMMAND_TYPE_DESTORY:

			ASSERT(CommandBuffer[i].DestoryCommand.pPawn_to_destory);

			if (CommandBuffer[i].DestoryCommand.pawnType > 50)
			{
				CommandBuffer[i].DestoryCommand.pPawn_to_destory;
			}

			DestoringPawn(i);

			//��յ�ǰ������Ϣ��
			CommandBuffer[i].DestoryCommand.pawnType = PAWN_TYPE_NONE;
			CommandBuffer[i].DestoryCommand.pPawn_to_destory = nullptr;
			break;

		default:
			ASSERT(false && "PawnMasterִ�е��������ͷǷ�");
			break;
		}// end switch

		//��յ�ǰ�������͡�
		CommandBuffer[i].CommandType = PAWN_COMMAND_TYPE_NONE;
	}// end for

	//����δִ������������
	UnExecuteeCommandNum = 0;
}

void PawnMaster::CreatingPawn(UINT commandIndex)
{
	PawnCommand& command = 
		CommandBuffer[commandIndex];

	PawnCommandTemplate& commandTemplate = 
		*CommandTemplateList[command.CreateCommand.pawnType - 1];

	BasePawn * newPawn = 
		commandTemplate.CreatePawn(command.CreateCommand.pProperty, m_pScence);
}

void PawnMaster::DestoringPawn(UINT commandIndex)
{
	PawnCommand& command =
		CommandBuffer[commandIndex];

	PawnCommandTemplate& commandTemplate =
		*CommandTemplateList[command.DestoryCommand.pawnType - 1];
	
	commandTemplate.DestoryPawn(
		command.DestoryCommand.pPawn_to_destory,
		m_pScence);
}

PawnCommandTemplate::PawnCommandTemplate(UINT maxPawnNum)
{
}

PawnCommandTemplate::~PawnCommandTemplate()
{
}
