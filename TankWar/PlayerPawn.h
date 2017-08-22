#pragma once
#include "BasePawn.h"
#include "LinkedAllocator.h"
#include "PawnMaster.h"
#include "PlayerCommander.h"


//�����ɵ�PawnPlayer������
#define MAX_PLAYER_PAWN_NUM ((unsigned int)5)
#define CONTROLITEM_NUM_PLAYER_PAWN 1

//��ҵĿɿ���ControlItem�ĸ��ڵ���ţ�ע����ҵ��е�������Ϳ���Ⱦ���ڵ��Ƿֿ��ġ�
#define CONTROLITEM_INDEX_PLAYER_PAWN_ROOT 0
#define BONE_INDEX_PLAYER_PAWN_ROOT 0
#define BONE_INDEX_PLAYER_PAWN_CAMERA_POS 1
#define BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET 2

class PlayerPawn
{
protected:
	PlayerPawn();
public:
	~PlayerPawn();

//��һ���ָ������PlayerPawn��ľ�̬����
public :
	//��PawnMaster�е����ͱ�ʶ��
	static PawnType m_pawnType;
	//��ҵĿ���ģʽ��
	static PlayerControlType m_playerControlType;

	//���������ɺ�����ָ���PawnMaster��
	static PawnMaster *			m_pPawnMaster;
	static PlayerCommander *	m_pPlayerCommander;
	static BoneCommander *		m_pBoneCommander;
	//static CollideCommander *	m_pCollideCommander;

	//���Է����
	static MyStaticAllocator<PlayerProperty> m_propertyAllocator;
	//���������
	static MyStaticAllocator<PlayerPawn> m_PlayerPawnAllocator;

public:
	//ִ������ע�᷽�������������ע��PawnMaster��
	static void RegisterAll(
		PawnMaster *		pPawnMaster, 
		PlayerCommander *	m_pPlayerCommander, 
		BoneCommander *		pBoneCommander
	//	, CollideCommander *	pCollideCommander
	);
	//ע��PawnMaster������PawnMaster�л�ȡһ��PawnType��
	static void RegisterPawnMaster(PawnMaster * pPawnMaster);

	//�������ģ��
	friend class PlayerPawnCommandTemplate;
	//��ҿ���ģ��
	friend class PlayerControlCommandTemplate;


//��һ�����ǹ���Player�ķǾ�̬���ԡ�
protected:
	//���������;
	MyCamera* m_pCamera;
	//��PlayerCommander�еĿ��Ƶ�Ԫָ�롣
	PlayerControl* m_pPlayerControl;
	//PlayerPawn�ɿ��Ƶ�ControlItem������ע�Ᵽ��Ķ���ָ�롣
	ControlItem* m_arr_ControlItem[CONTROLITEM_NUM_PLAYER_PAWN];
	//���й�����������ӦControlItem��ע�������������������������������Ĺ�����
	Bone* m_arr_Bones[CONTROLITEM_NUM_PLAYER_PAWN + 2];

public:
	//ָ�����PlayerPawn����������Ϸ���ԣ�����ǰ���ٶȣ�������ת�ٶȣ��ڵ���ȴʱ�䡭��
	PlayerProperty* m_pProperty;

public:
	//PlayerPawn�ĸ���������
	ControlItem *& RootControl();
};

//��ʼʱ��̬��Ա��
PawnType			PlayerPawn::m_pawnType			= PAWN_TYPE_NONE;
//Player���������͡�
PlayerControlType	PlayerPawn::m_playerControlType = PLAYER_CONTROL_TYPE_NONE;
//PawnMaster��
PawnMaster *		PlayerPawn::m_pPawnMaster		= nullptr;
//���ָ��١�
PlayerCommander *	PlayerPawn::m_pPlayerCommander	= nullptr;
//����ָ��١�
BoneCommander *		PlayerPawn::m_pBoneCommander	= nullptr;
//��ײָ��١�
//CollideCommander *	PlayerPawn::m_pCollideCommander		= nullptr;
MyStaticAllocator<PlayerProperty>	PlayerPawn::m_propertyAllocator(	MAX_PLAYER_PAWN_NUM);
MyStaticAllocator<PlayerPawn>		PlayerPawn::m_PlayerPawnAllocator(	MAX_PLAYER_PAWN_NUM);

//������Զ���
struct PlayerProperty : public PawnProperty
{
	//ֱ���ٶ�
	float MoveSpeed;
	//������ת�ٶ�
	float rotationSpeed;
};

//����PawnMaster���Զ�������Pawn������ģ�壬��ֱֹ������Pawn����
class PlayerPawnCommandTemplate : public PawnCommandTemplate
{
public:
	virtual BasePawn* CreatePawn(PawnProperty* pProperty, Scence* pScence);
	//pPawn��һ����̬������ڴ棬�����ٶ�Ӧ��Pawn֮��DestoryPawn��Ҫ�����ͷ�����ڴ浽m_playerAllocator��
	//ͬʱPawn�е�PawnPropertyҲ��Ҫ���Żض�Ӧ���ڴ���ڡ�
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence);

protected:
	//��PlayerCommander���һ����ҵĿ�������
	void AddPlayerControl(PlayerPawn* pPlayerPawn);
	//��BoneCommander����ӹ����������PlayerPawn�����������Լ�����ͷ�γ�������
	void AddBones(PlayerPawn* pPlayerPawn);

	void DeletePlayerControl(PlayerPawn* pPlayerPawn);
	void DeleteBones(PlayerPawn* pPlayerPawn);
};

//���Ƶ�Ԫ����Ӧ�û����롣
class PlayerControlCommandTemplate : public ControlCommandTemplate
{
public:
	//����ƶ�
	virtual void MouseMove(BasePawn* pPawn, float lastX, float lastY, float currX, float currY, WPARAM btnState);
	//����W��
	virtual void HitKey_W(BasePawn* pPawn);
	//����A��
	virtual void HitKey_A(BasePawn* pPawn);
	//����S��
	virtual void HitKey_S(BasePawn* pPawn);
	//����D��
	virtual void HitKey_D(BasePawn* pPawn);
	//����������
	virtual void PressMouseButton_Left(BasePawn* pPawn);
	//��������Ҽ�
	virtual void PressMouseButton_Right(BasePawn* pPawn);
};
