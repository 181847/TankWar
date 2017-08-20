#pragma once
#include "BasePawn.h"
#include "AI_Follow_Collide_BasePawn.h"
#include "LinkedAllocator.h"


//可生成的PawnPlayer的数量
#define MAX_PLAYER_PAWN_NUM 5
#define NextNext(pointer) ((pointer)->Next->Next)
//从玩家Pawn指针找到对应的摄像机指针。
#define P_PlayerCameraPositionControl(pPlayerPawn) ((pPlayerPawn)->m_pCamera->m_cameraPos)
//从玩家Pawn指针找到对应的摄像机目标指针。
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
	//PlayerCommander负责本Pawn的输入控制行为，让玩家控制角色。
	static PlayerCommander* m_pPlayerCommander;
	//PlayerCommander控制命令序号，
	//通过这个序号来为Player配置指定的操作方法。
	static int CommandId_Player_Control;
};

//用于PawnMaster中自动化生成Pawn的命令模板，禁止直接生成Pawn对象。
class PlayerPawnCommandTemplate : public PawnCommandTemplate
{
protected:
	static LinkedAllocator<PlayerPawn> m_playerAllocator;
public:
	PawnType PawnType =	PAWN_TYPE_PLAYER;
	//pDesc是一个动态分配的内存，CreatePawn需要自行释放这个内存。
	virtual BasePawn* CreatePawn(PawnDesc* pDesc, Scence* pScence);
	//pPawn是一个动态分配的内存，在销毁对应的Pawn之后，DestoryPawn需要自行释放这个内存到m_playerAllocator。
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence);

protected:
	void AddControlTo_PlayerCommander(PlayerPawn* pPlayerPawn);
	void AddControlTo_FollowCommander(PlayerPawn* pPlayerPawn);

	void DetachControlFrom_PlayerCommander(PlayerPawn* pPlayerPawn);
	void DetachControlFrom_FollowCommander(PlayerPawn* pPlayerPawn);
};

//初始化PlayerPawn代理人中的内存分配池大小。
LinkedAllocator<PlayerPawn> PlayerPawnCommandTemplate::m_playerAllocator(MAX_PLAYER_PAWN_NUM);

