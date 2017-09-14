#pragma once
#include "BasePawn.h"
#include "PlayerPawnProperty.h"
#include "ShellPawnProperty.h"
#include "ArmoredCarPawnProperty.h"

class PlayerPawn: public BasePawn
{
public:
	PlayerPawn();
	~PlayerPawn();

//��һ���ָ������PlayerPawn��ľ�̬����
public :
	//���������
	static LinkedAllocator<PlayerPawn> PawnAllocator;

	//�ڹܵ��������ǡ�
	static const float Radian_Pitch_Barrel_Max;
	//�ڹܵ���С����ǡ�
	static const float Radian_Pitch_Barrel_Min;

public:
	//ִ������ע�᷽�������������ע��PawnMaster��
	static void Register();
	static PlayerProperty* NewProperty();

	//�������ģ��
	friend class PlayerPawnCommandTemplate;
	//��ҿ���ģ��
	friend class PlayerControlCommandTemplate;
	//AI����ģ��
	friend class PlayerAITemplate;


//��һ�����ǹ���Player�ķǾ�̬���ԡ�
protected:
	//���������;
	MyCamera *				m_pCamera;
	//��PlayerCommander�еĿ��Ƶ�Ԫָ�롣
	PlayerControlUnit *		m_pPlayerControl;
	//AI����ָ��
	AIUnit *				m_pAIUnit;
	//PlayerPawn�ɿ��Ƶ�ControlItem������ע�Ᵽ��Ķ���ָ�롣
	ControlItem*			m_arr_ControlItem[CONTROLITEM_NUM_PLAYER_PAWN];
	//���й�����������ӦControlItem��ע�������������������������������Ĺ�����
	Bone*					m_arr_Bones[CONTROLITEM_NUM_PLAYER_PAWN + 2];

public:
	//ָ�����PlayerPawn����������Ϸ���ԣ�����ǰ���ٶȣ�������ת�ٶȣ��ڵ���ȴʱ�䡭��
	PlayerProperty*			m_pProperty;

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

};

class PlayerAITemplate :
	public AITemplate
{
public:
	//AITemplate��������һ��vector�����洢�˸���״̬��ά��ʱ���ת����ʽ��
	//ע���ڹ��캯��������Զ���״̬��
	PlayerAITemplate();
	//���ݵ�ǰAI��״̬��ִ����Ҫ�Ŀ��Ʒ�����
	virtual void Runing(
		BasePawn* pPawn, AIStatue state,
		float consumedTime, const GameTimer& gt);

	DELETE_COPY_CONSTRUCTOR(PlayerAITemplate)

public:
	static void aim(PlayerPawn * pPlayer, const GameTimer& gt);


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
