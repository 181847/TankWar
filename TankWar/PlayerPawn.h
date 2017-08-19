#pragma once
#include "BasePawn.h"
#include "LinkedAllocator.h"
#define MAX_PLAYER_PAWN_NUM 5
#define NextNext(pointer) (pointer->Next->Next)
//�����Pawnָ���ҵ���Ӧ�������ָ�롣
#define P_PlayerCameraPositionControl(pPlayerPawn) (pPlayerPawn->m_pCamera->m_cameraPos)
//�����Pawnָ���ҵ���Ӧ�������Ŀ��ָ�롣
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
	//pDesc��һ����̬������ڴ棬CreatePawn��Ҫ�����ͷ�����ڴ档
	virtual BasePawn* CreatePawn(PawnDesc* pDesc, Scence* pScence);
	//pPawn��һ����̬������ڴ棬�����ٶ�Ӧ��Pawn֮��DestoryPawn��Ҫ�����ͷ�����ڴ档
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence);
};

LinkedAllocator<PlayerPawn> PlayerPawnCommandTemplate::m_playerAllocator(MAX_PLAYER_PAWN_NUM);
