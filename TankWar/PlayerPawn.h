#pragma once
#include "BasePawn.h"
#include "PawnMaster.h"
#include "PlayerCommander.h"
#include "BoneCommander.h"
#include "CollideCommander.h"
#include "ShellProperty.h"

//�����ɵ�PawnPlayer������
#define MAX_PLAYER_PAWN_NUM ((unsigned int)5)
#define CONTROLITEM_NUM_PLAYER_PAWN 4

//��ҵĿɿ���ControlItem�ĸ��ڵ���ţ�ע����ҵ��е�������Ϳ���Ⱦ���ڵ��Ƿֿ��ġ�
#define CONTROLITEM_INDEX_PLAYER_PAWN_ROOT 0
//��̨
#define CONTROLITEM_INDEX_PLAYER_PAWN_BATTERY 1
//����Ŀ�����
#define CONTROLITEM_INDEX_PLAYER_PAWN_MAINBODY 2
//�ڹܿ�����
#define CONTROLITEM_INDEX_PLAYER_PAWN_BARREL 3

//�����������
#define BONE_INDEX_PLAYER_PAWN_ROOT 0
//��������ꡣ
#define BONE_INDEX_PLAYER_PAWN_CAMERA_POS 1
//�����Ŀ�ꡣ
#define BONE_INDEX_PLAYER_PAWN_CAMERA_TARGET 2
//��̨
#define BONE_INDEX_PLAYER_PAWN_BATTERY 3
//����
#define BONE_INDEX_PLAYER_PAWN_MAINBODY 4
//�ڹ�
#define BONE_INDEX_PLAYER_PAWN_BARREL 5

//������󳤶�
#define MAX_RAY_LENGTH 500


//������Զ���
struct PlayerProperty : public PawnProperty
{
	//ֱ���ٶ�
	float MoveSpeed;
	//������ת�ٶ�
	float RotationSpeed;
};

class PlayerPawn: public BasePawn
{
public:
	PlayerPawn();
	~PlayerPawn();

//��һ���ָ������PlayerPawn��ľ�̬����
public :
	//��PawnMaster�е����ͱ�ʶ��
	static PawnType pawnType;
	//��ҵĿ���ģʽ��
	static PlayerControlType m_playerControlType;

	//���������ɺ�����ָ���PawnMaster��
	static PawnMaster *			pPawnMaster;
	static PlayerCommander *	pPlayerCommander;
	static BoneCommander *		pBoneCommander;
	static CollideCommander *	pCollideCommander;

	//���Է����
	static MyStaticAllocator<PlayerProperty> m_propertyAllocator;
	//���������
	static LinkedAllocator<PlayerPawn> m_PlayerPawnAllocator;

	//����ľ�̬���ԣ����һ��װ�׳��Ĵ������ͣ����û�������������ʱ��Ϳ��Դ���һ��ArmoredCarPawn��
	static PawnType refCarType;

	//�ڹܵ��������ǡ�
	static const float Radian_Pitch_Barrel_Max;
	//�ڹܵ���С����ǡ�
	static const float Radian_Pitch_Barrel_Min;

public:
	//ִ������ע�᷽�������������ע��PawnMaster��
	static void RegisterAll(
		PawnMaster *		pPawnMaster, 
		PlayerCommander *	pPlayerCommander, 
		BoneCommander *		pBoneCommander
		, CollideCommander *	pCollideCommander
	);
	//ע��PawnMaster������PawnMaster�л�ȡһ��PawnType��
	static void RegisterPawnMaster(PawnMaster * pPawnMaster);
	static PlayerProperty* NewProperty();

	//�������ģ��
	friend class PlayerPawnCommandTemplate;
	//��ҿ���ģ��
	friend class PlayerControlCommandTemplate;


//��һ�����ǹ���Player�ķǾ�̬���ԡ�
protected:
	//��PawnMaster�д洢��ǰPawn����ĵ�λָ�롣
	PawnUnit* m_pSavedUnit;
	//���������;
	MyCamera* m_pCamera;
	//��PlayerCommander�еĿ��Ƶ�Ԫָ�롣
	PlayerControlUnit * m_pPlayerControl;
	//PlayerPawn�ɿ��Ƶ�ControlItem������ע�Ᵽ��Ķ���ָ�롣
	ControlItem* m_arr_ControlItem[CONTROLITEM_NUM_PLAYER_PAWN];
	//���й�����������ӦControlItem��ע�������������������������������Ĺ�����
	Bone* m_arr_Bones[CONTROLITEM_NUM_PLAYER_PAWN + 2];

public:
	//ָ�����PlayerPawn����������Ϸ���ԣ�����ǰ���ٶȣ�������ת�ٶȣ��ڵ���ȴʱ�䡭��
	PlayerProperty* m_pProperty;

public:
	//PlayerPawn�ĸ������������������λ�ƣ��������ʾ��
	ControlItem * RootControl();
	//��̨��������
	ControlItem * Battery();
	//��������������Ƴ�����
	ControlItem * MainBody();
	//�ڹܿ�������
	ControlItem * Barrel();
};



//����PawnMaster���Զ�������Pawn������ģ�壬��ֱֹ������Pawn����
class PlayerPawnCommandTemplate : public PawnCommandTemplate
{
public:
	PlayerPawnCommandTemplate();
	~PlayerPawnCommandTemplate();

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
	virtual void MouseMove(BasePawn* pPawn, MouseState mouseState, const GameTimer& gt);
	//����W��
	virtual void HitKey_W(BasePawn* pPawn, const GameTimer& gt);
	//����A��
	virtual void HitKey_A(BasePawn* pPawn, const GameTimer& gt);
	//����S��
	virtual void HitKey_S(BasePawn* pPawn, const GameTimer& gt);
	//����D��
	virtual void HitKey_D(BasePawn* pPawn, const GameTimer& gt);
	//���һ��������
	virtual void HitMouseButton_Left(BasePawn * pPawn, const GameTimer& gt);
	//���һ������Ҽ�
	virtual void HitMouseButton_Right(BasePawn * pPawn, const GameTimer& gt);
	//����������
	virtual void PressMouseButton_Left(BasePawn* pPawn, const GameTimer& gt);
	//��������Ҽ�
	virtual void PressMouseButton_Right(BasePawn* pPawn, const GameTimer& gt);

public:
	//��ݷ�����

	//��ת��λ��̨�ķ���ʹ����̨��ˮƽ�����Զ�׼������ײ��λ�á�
	//ע�ⲻ��ֱ��ָ�򣬶�����������ת��̨��ָ����λ�á�
	static void rotateBattery(
		PlayerPawn * pPlayerPawn, 
		XMFLOAT3 targetFloat3,
		const GameTimer& gt);

	//��ֱ��ת�ڹܣ�ʹ���ڹܶ�������ײ��λ�á�
	//ע�ⲻ��ֱ��ָ�򣬶�����������ת�ڹܵ�ָ����λ�á�
	static void rotateBarrel(
		PlayerPawn * pPlayerPawn,
		XMFLOAT3 targetFloat3, 
		const GameTimer& gt);
};
