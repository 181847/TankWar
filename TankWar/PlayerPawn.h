#pragma once
#include "BasePawn.h"
#include "LinkedAllocator.h"
#define MAX_PLAYER_PAWN_NUM 5
#define NextNext(pointer) (pointer->Next->Next)
//从玩家Pawn指针找到对应的摄像机指针。
#define P_PlayerCameraPositionControl(pPlayerPawn) (pPlayerPawn->m_pCamera->m_cameraPos)
//从玩家Pawn指针找到对应的摄像机目标指针。
#define P_PlayerCameraTargetControl(pPlayerPawn) (pPlayerPawn->m_pCamera->m_cameraPos->Next)
class PlayerPawn :
	public BasePawn
{
private:
	PlayerPawn();
public:
	~PlayerPawn();
	virtual PawnCommandTemplate* GeneratePawnCommandTemplate();
	friend class PlayerPawnCommandTemplate;

protected:
	//static PlayerCommander* m_pPlayerCommander;
	Camera* m_pCamera;
};

class PlayerPawnCommandTemplate : public PawnCommandTemplate
{
protected:
	static LinkedAllocator<PlayerPawn> m_playerAllocator;
public:
	//pDesc是一个动态分配的内存，CreatePawn需要自行释放这个内存。
	virtual BasePawn* CreatePawn(PawnDesc* pDesc, Scence* pScence);
	//pPawn是一个动态分配的内存，在销毁对应的Pawn之后，DestoryPawn需要自行释放这个内存。
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence);
};

LinkedAllocator<PlayerPawn> PlayerPawnCommandTemplate::m_playerAllocator(MAX_PLAYER_PAWN_NUM);
