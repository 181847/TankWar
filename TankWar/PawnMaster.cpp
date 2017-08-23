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
	//�������͡�
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

void PawnMaster::DestroyPawn(BasePawn * pThePawn)
{
	//�������͡�
	CommandBuffer[UnExecuteeCommandNum].CommandType =
		PAWN_COMMAND_TYPE_DESTORY;

	//Ҫɾ����Pawnָ�롣
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
			CommandTemplateList[CommandBuffer[i].CreateCommand.pawnType - 1]
				->CreatePawn(
					CommandBuffer[i].CreateCommand.pProperty, 
					m_pScence);

			//��յ�ǰ������Ϣ��
			CommandBuffer[i].CreateCommand.pawnType = PAWN_TYPE_NONE;
			CommandBuffer[i].CreateCommand.pProperty = nullptr;
			break;

		case PAWN_COMMAND_TYPE_DESTORY:
			CommandTemplateList[CommandBuffer[i].CreateCommand.pawnType - 1]
				->DestoryPawn(
					CommandBuffer[i].DestoryCommand.pPawn_to_destory,
					m_pScence);

			//��յ�ǰ������Ϣ��
			CommandBuffer[i].DestoryCommand.pPawn_to_destory = nullptr;
			break;

		default:
			ASSERT(false && "PawnMasterִ�е��������ͷǷ�");
			break;
		}

		//��յ�ǰ�������͡�
		CommandBuffer[i].CommandType = PAWN_COMMAND_TYPE_NONE;
	}

	//����δִ������������
	UnExecuteeCommandNum = 0;
}
