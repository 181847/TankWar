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

void PawnMaster::DestroyPawn(PawnType pawnType, BasePawn * pThePawn)
{
	//命令类型。
	CommandBuffer[UnExecuteeCommandNum].CommandType =
		PAWN_COMMAND_TYPE_DESTORY;

	//要删除的Pawn类型。
	CommandBuffer[UnExecuteeCommandNum].DestoryCommand.pawnType =
		pawnType;

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
			CreatingPawn(i);

			//清空当前命令信息。
			CommandBuffer[i].CreateCommand.pawnType = PAWN_TYPE_NONE;
			CommandBuffer[i].CreateCommand.pProperty = nullptr;
			break;

		case PAWN_COMMAND_TYPE_DESTORY:
			DestoringPawn(i);

			//清空当前命令信息。
			CommandBuffer[i].DestoryCommand.pawnType = PAWN_TYPE_NONE;
			CommandBuffer[i].DestoryCommand.pPawn_to_destory = nullptr;
			break;

		default:
			ASSERT(false && "PawnMaster执行的命令类型非法");
			break;
		}// end switch

		//清空当前命令类型。
		CommandBuffer[i].CommandType = PAWN_COMMAND_TYPE_NONE;
	}// end for

	//重置未执行命令数量。
	UnExecuteeCommandNum = 0;
}

void PawnMaster::CreatingPawn(UINT commandIndex)
{
	PawnCommand& command = 
		CommandBuffer[commandIndex];

	PawnCommandTemplate& commandTemplate = 
		*CommandTemplateList[command.CreateCommand.pawnType - 1];

	auto pPawnUnit = commandTemplate.Manager.Malloc();

	BasePawn * newPawn = 
		commandTemplate.CreatePawn(pPawnUnit, command.CreateCommand.pProperty, m_pScence);

	//存储创建的Pawn指针。
	pPawnUnit->pSavedPawn = newPawn;
}

void PawnMaster::DestoringPawn(UINT commandIndex)
{
	PawnCommand& command =
		CommandBuffer[commandIndex];

	PawnCommandTemplate& commandTemplate =
		*CommandTemplateList[command.DestoryCommand.pawnType - 1];
	
	//返回原先存储这个Pawn的单元指针。
	auto noUsedUnit = commandTemplate.DestoryPawn(
		command.DestoryCommand.pPawn_to_destory,
		m_pScence);

	//清空存储的Pawn指针。
	noUsedUnit->pSavedPawn = nullptr;

	//回收内存。
	commandTemplate.Manager.Free(noUsedUnit);
}

PawnCommandTemplate::PawnCommandTemplate(UINT maxPawnNum)
	:Manager(maxPawnNum)
{
}

PawnCommandTemplate::~PawnCommandTemplate()
{
}
