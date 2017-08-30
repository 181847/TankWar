#pragma once
#include "BasePawn.h"
#include "PawnMaster.h"
#include "BoneCommander.h"
#include "PlayerPawn.h"
#include "AICommander.h"

//����װ�׳�����
#define MAX_PAWN_CAR_NUM 60

//װ�׳���λ�Ŀɿ�������������Ĭ��������һ��������������λ�ƣ���һ��������ת��
#define CONTROLITEM_NUM_ARMORED_CAR 2

//װ�׳��ĸ����������
#define CONTROLITEM_INDEX_CAR_ROOT 0
//װ�׳����������
#define CONTROLITEM_INDEX_CAR_MAIN_BODY 1

//�������������
#define BONE_INDEX_PLAYER_CAR_ROOT CONTROLITEM_INDEX_CAR_ROOT
//����������
#define BONE_INDEX_PLAYER_CAR_MAIN_BODY CONTROLITEM_INDEX_CAR_MAIN_BODY

//�����ƶ�״̬��־��
#define STORY_FRAGMENT_CAR_MOVE 1
//װ�׳���׼״̬��־��
#define STORY_FRAGMENT_CAR_AIM 2
//װ�׳�����״̬��־��
#define STORY_FRAGMENT_CAR_SHOUT 3

extern const float CarProperty_default_MoveSpeed;
extern const float CarProperty_default_RotationSpeed;

//������Զ���
struct CarProperty : public PawnProperty
{
	//ֱ���ٶ�
	float MoveSpeed;
	//������ת�ٶ�
	float RotationSpeed;
};

class ArmoredCar :
	public BasePawn
{
public:
	ArmoredCar();
	~ArmoredCar();

	//��һ���ָ������PlayerPawn��ľ�̬����
public:
	//��PawnMaster�е����ͱ�ʶ��
	static PawnType PawnType;
	//��ҵĿ���ģʽ��
	static AIControlType aiControlType;

	//���������ɺ�����ָ���PawnMaster��
	static PawnMaster *			pPawnMaster;
	static AICommander *	pAICommander;
	static BoneCommander *		pBoneCommander;
	//static CollideCommander *	m_pCollideCommander;

	//���Է����
	static MyStaticAllocator<CarProperty> PropertyAllocator;
	//���������
	static MyStaticAllocator<ArmoredCar> PawnAllocator;

public:
	//ִ������ע�᷽�������������ע��PawnMaster��
	static void RegisterAll(
		PawnMaster *	pPawnMaster,
		AICommander *	pAICommander,
		BoneCommander *	pBoneCommander
		//	, CollideCommander *	pCollideCommander
	);
	//ע��PawnMaster������PawnMaster�л�ȡһ��PawnType��
	static void RegisterPawnMaster(PawnMaster * pPawnMaster);
	static CarProperty* NewProperty();

	//��λ����ģ��
	friend class CarPawnTemplate;
	//AI����ģ��
	friend class AICommandTemplate;


	//��һ�����ǹ���Player�ķǾ�̬���ԡ�
protected:
	//Pawn�洢ָ�롣
	PawnUnit* m_pSaveUnit;
	//���������;
	MyCamera* m_pCamera;
	//��PlayerCommander�еĿ��Ƶ�Ԫָ�롣
	AIUnit * m_pAIUnit;
	//PlayerPawn�ɿ��Ƶ�ControlItem������ע�Ᵽ��Ķ���ָ�롣
	ControlItem* m_arr_ControlItem[CONTROLITEM_NUM_ARMORED_CAR];
	//���й�����������ӦControlItem��
	Bone* m_arr_Bones[CONTROLITEM_NUM_ARMORED_CAR];

public:
	//ָ�����PlayerPawn����������Ϸ���ԣ�����ǰ���ٶȣ�������ת�ٶȣ��ڵ���ȴʱ�䡭��
	CarProperty* m_pProperty;

	//һ����һЩ����Ĺ��߷�����
public:
	//PlayerPawn�ĸ������������������λ�ƣ��������Ĭ�ϲ���ʾ��
	ControlItem * RootControl();
	//��������������Ƴ�����
	ControlItem * MainBody();
};



//����PawnMaster���Զ�������Pawn������ģ�壬��ֱֹ������Pawn����
class CarPawnTemplate : public PawnCommandTemplate
{
public:
	CarPawnTemplate();
	~CarPawnTemplate();

public:
	virtual BasePawn* CreatePawn(PawnUnit * saveUnit, PawnProperty* pProperty, Scence* pScence);
	//pPawn��һ����̬������ڴ棬�����ٶ�Ӧ��Pawn֮��DestoryPawn��Ҫ�����ͷ�����ڴ浽m_playerAllocator��
	//ͬʱPawn�е�PawnPropertyҲ��Ҫ���Żض�Ӧ���ڴ���ڡ�
	virtual PawnUnit* DestoryPawn(BasePawn* pPawn, Scence* pScence);

protected:
	//��AICommander���һ���Զ����Ƶ�Ԫ��
	void AddAIControl(ArmoredCar* pPawn);
	//��BoneCommander����ӹ�����
	void AddBones(ArmoredCar* pPawn);

	void DeleteAIControl(ArmoredCar* pPawn);
	void DeleteBones(ArmoredCar* pPawn);
};


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
	void move(ArmoredCar * pCar, const GameTimer& gt);
	//װ�׳���׼��
	void aim(ArmoredCar * pCar, const GameTimer& gt);
	//װ�׳������
	void shout(ArmoredCar * pCar, float consumedTime, const GameTimer& gt);

	//���߷���
protected:
	//�ó��ӵ���ָ����ControlItem�����������ƶ���
	void MoveToward(ArmoredCar * pCar, ControlItem * pTarget, const GameTimer& gt);
	//�ó��ӵ���ָ���������ƶ���
	void MoveToward(ArmoredCar * pCar, XMFLOAT4 targetLocation, const GameTimer& gt);

};