#pragma once
#include "ArmoredCarPawnProperty.h"
#include "PlayerPawn.h"

//װ�׳�Pawn��
class ArmoredCar :
	public BasePawn
{
public:
	ArmoredCar();
	~ArmoredCar();

	//��һ���ָ������ArmoredCar��ľ�̬����
public:

	//���������
	static MyStaticAllocator<ArmoredCar>	PawnAllocator;

public:
	//ִ������ע�᷽����
	static void Register();

	//����һ��װ�׳�����ָ�롣
	static CarProperty* NewProperty();

	//��λ����ģ��
	friend class CarPawnTemplate;
	//AI����ģ��
	friend class AICommandTemplate;


	//��һ�����ǹ���Player�ķǾ�̬���ԡ�
protected:
	//Pawn�洢ָ�롣
	PawnUnit*		m_pSaveUnit;
	//���������;
	MyCamera*		m_pCamera;
	//��PlayerCommander�еĿ��Ƶ�Ԫָ�롣
	AIUnit *		m_pAIUnit;
	//PlayerPawn�ɿ��Ƶ�ControlItem������ע�Ᵽ��Ķ���ָ�롣
	ControlItem*	m_arr_ControlItem[CONTROLITEM_NUM_ARMORED_CAR];
	//���й�����������ӦControlItem��
	Bone*			m_arr_Bones[CONTROLITEM_NUM_ARMORED_CAR];
	//������ײ�塣
	CollideBox*		m_arr_CBoxes[COLLIDE_RECT_NUM_ARMORED_CAR];

public:
	//ָ�����PlayerPawn����������Ϸ���ԣ�����ǰ���ٶȣ�������ת�ٶȣ��ڵ���ȴʱ�䡭��
	CarProperty*	m_pProperty;

	//һ����һЩ����Ĺ��߷�����
public:
	//PlayerPawn�ĸ������������������λ�ƣ��������Ĭ�ϲ���ʾ��
	ControlItem *	RootControl();
	//��������������Ƴ�����
	ControlItem *	MainBody();

	//��������ײ�С�
	CollideBox *	RootBox();
};



//����PawnMaster���Զ�������Pawn������ģ�壬��ֱֹ������Pawn����
class CarPawnTemplate : public PawnCommandTemplate
{
public:
	CarPawnTemplate();
	~CarPawnTemplate();

public:
	virtual BasePawn* CreatePawn(PawnProperty* pProperty, Scence* pScence);
	//pPawn��һ����̬������ڴ棬�����ٶ�Ӧ��Pawn֮��DestoryPawn��Ҫ�����ͷ�����ڴ浽pPlayerAllocator��
	//ͬʱPawn�е�PawnPropertyҲ��Ҫ���Żض�Ӧ���ڴ���ڡ�
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence);

protected:
	//��AICommander���һ���Զ����Ƶ�Ԫ��
	void AddAIControl		(ArmoredCar* pPawn);
	//��BoneCommander����ӹ�����
	void AddBones			(ArmoredCar* pPawn);
	//������ײ��Ԫ��
	void AddCollideBoxes	(ArmoredCar* pPawn);

	void DeleteAIControl	(ArmoredCar* pPawn);
	void DeleteBones		(ArmoredCar* pPawn);
	void DeleteCollideBoxes	(ArmoredCar* pPawn);
};

//AI����ģ�塣
class CarAITemplate : public AITemplate
{
public:
	//AITemplate��������һ��vector�����洢�˸���״̬��ά��ʱ���ת����ʽ��
	//ע���ڹ��캯��������Զ���״̬��
	CarAITemplate();
	//���ݵ�ǰAI��״̬��ִ����Ҫ�Ŀ��Ʒ�����
	virtual void Runing(
		BasePawn* pPawn,	AIStatue state, 
		float consumedTime, const GameTimer& gt);

	//������һЩ���ArmoredCar�ļ򻯲�����
protected:
	//�ƶ�װ�׳���
	void move	(ArmoredCar * pCar, const GameTimer& gt);
	//װ�׳���׼��
	void aim	(ArmoredCar * pCar, const GameTimer& gt);
	//װ�׳������
	void shout	(ArmoredCar * pCar, float consumedTime, const GameTimer& gt);

	//���߷���
protected:
	//�ó��ӵ���ָ����ControlItem�����������ƶ���
	void MoveToward(ArmoredCar * pCar, ControlItem * pTarget, const GameTimer& gt);
	//�ó��ӵ���ָ���������ƶ���
	void MoveToward(ArmoredCar * pCar, XMFLOAT4 targetLocation, const GameTimer& gt);

};