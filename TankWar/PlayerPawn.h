#pragma once
#include "BasePawn.h"
#include "AI_Follow_Collide_BasePawn.h"
#include "LinkedAllocator.h"


//�����ɵ�PawnPlayer������
#define MAX_PLAYER_PAWN_NUM 5
#define NextNext(pointer) ((pointer)->Next->Next)
//�����Pawnָ���ҵ���Ӧ�������ָ�롣
#define P_PlayerCameraPositionControl(pPlayerPawn) ((pPlayerPawn)->m_pCamera->m_cameraPos)
//�����Pawnָ���ҵ���Ӧ�������Ŀ��ָ�롣
#define P_PlayerCameraTargetControl(pPlayerPawn) ((pPlayerPawn)->m_pCamera->m_cameraPos->Next)
#define STATIC_P_PLAYER_COMMANDER(Class_Base_On_PlayerPawn) (Class_Base_On_PlayerPawn.m_pPlayerCommander)

class PlayerPawn :
	public AI_Follow_Collide_BasePawn
{
protected:
	PlayerPawn();
public:
	~PlayerPawn();
	virtual PawnCommandTemplate* GeneratePawnCommandTemplate();
	void RegisterPlayerCommander(PlayerCommander* pPlayerCommander);
	friend class PlayerPawnCommandTemplate;

protected:
	//static PlayerCommander* m_pPlayerCommander;
	MyCamera* m_pCamera;
	//PlayerCommander����Pawn�����������Ϊ������ҿ��ƽ�ɫ��
	static PlayerCommander* m_pPlayerCommander;
	//PlayerCommander����������ţ�
	//ͨ����������ΪPlayer����ָ���Ĳ���������
	static int CommandId_Player_Control;
};

//����PawnMaster���Զ�������Pawn������ģ�壬��ֱֹ������Pawn����
class PlayerPawnCommandTemplate : public PawnCommandTemplate
{
protected:
	static LinkedAllocator<PlayerPawn> m_playerAllocator;
public:
	PawnType PawnType =	PAWN_TYPE_PLAYER;
	//pDesc��һ����̬������ڴ棬CreatePawn��Ҫ�����ͷ�����ڴ档
	virtual BasePawn* CreatePawn(PawnDesc* pDesc, Scence* pScence);
	//pPawn��һ����̬������ڴ棬�����ٶ�Ӧ��Pawn֮��DestoryPawn��Ҫ�����ͷ�����ڴ浽m_playerAllocator��
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence);

protected:
	void AddControlTo_PlayerCommander(PlayerPawn* pPlayerPawn);
	void AddControlTo_FollowCommander(PlayerPawn* pPlayerPawn);

	void DetachControlFrom_PlayerCommander(PlayerPawn* pPlayerPawn);
	void DetachControlFrom_FollowCommander(PlayerPawn* pPlayerPawn);
};

//��ʼ��PlayerPawn�������е��ڴ����ش�С��
LinkedAllocator<PlayerPawn> PlayerPawnCommandTemplate::m_playerAllocator(MAX_PLAYER_PAWN_NUM);

