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
	//注意，PawnType为0表示没有类型，不能返回0。
	return CommandTemplateList.size();
}

void PawnMaster::CreatePawn(PawnType pawnType, PawnProperty* pProperty)
{
	//命令类型。
	CommandBuffer[UnExecuteeCommandNum].CommandType = 
		PAWN_COMMAND_TYPE_CREATE;
	//生成Pawn的类型。
	CommandBuffer[UnExecuteeCommandNum].CreateCommand.pawnType = 
		pawnType;
	//Pawn的属性。
	CommandBuffer[UnExecuteeCommandNum].CreateCommand.pProperty = 
		pProperty;

	//未执行命令数量加一。
	++UnExecuteeCommandNum;
}

void PawnMaster::DestroyPawn(BasePawn * pThePawn)
{
	//命令类型。
	CommandBuffer[UnExecuteeCommandNum].CommandType =
		PAWN_COMMAND_TYPE_DESTORY;

	//要删除的Pawn指针。
	CommandBuffer[UnExecuteeCommandNum].DestoryCommand.pPawn_to_destory = 
		pThePawn;

	//未执行命令数量加一。
	++UnExecuteeCommandNum;
}

void PawnMaster::Executee()
{
	for (UINT i = 0; i < UnExecuteeCommandNum; ++i)
	{
		switch (CommandBuffer[i].CommandType)
		{
		case PAWN_COMMAND_TYPE_CREATE:
			CommandTemplateList[CommandBuffer[i].CreateCommand.pawnType]
				->CreatePawn(
					CommandBuffer[i].CreateCommand.pProperty, 
					m_pScence);
			break;

		case PAWN_COMMAND_TYPE_DESTORY:
			CommandTemplateList[CommandBuffer[i].CreateCommand.pawnType]
				->DestoryPawn(
					CommandBuffer[i].DestoryCommand.pPawn_to_destory,
					m_pScence);
			break;

		default:
			ASSERT(false && "PawnMaster执行的命令类型在意料之外");
			break;
		}
	}
}
