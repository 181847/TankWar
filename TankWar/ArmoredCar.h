#pragma once
#include "BasePawn.h"
#include "PawnMaster.h"
#include "BoneCommander.h"
#include "PlayerPawn.h"
#include "AICommander.h"
#include "CollideCommander.h"

//最大的装甲车数量
#define MAX_PAWN_CAR_NUM 60

//装甲车单位的可控制网格数量，默认两个，一个根控制器控制位移，另一个控制旋转。
#define CONTROLITEM_NUM_ARMORED_CAR 2
//装甲车的根控制器序号
#define CONTROLITEM_INDEX_CAR_ROOT 0
//装甲车车身控制器
#define CONTROLITEM_INDEX_CAR_MAIN_BODY 1

//根骨骼数组序号
#define BONE_INDEX_PLAYER_CAR_ROOT CONTROLITEM_INDEX_CAR_ROOT
//车身骨骼序号
#define BONE_INDEX_PLAYER_CAR_MAIN_BODY CONTROLITEM_INDEX_CAR_MAIN_BODY

//碰撞体的数量。
#define COLLIDE_RECT_NUM_ARMORED_CAR 1
//根碰撞体序号
#define COLLIDE_RECT_INDEX_CAR_ROOT 0

//车身移动状态标志。
#define STORY_FRAGMENT_CAR_MOVE 1
//装甲车瞄准状态标志。
#define STORY_FRAGMENT_CAR_AIM 2
//装甲车开火状态标志。
#define STORY_FRAGMENT_CAR_SHOUT 3

extern const float CarProperty_default_MoveSpeed;
extern const float CarProperty_default_RotationSpeed;

//玩家属性定义
struct CarProperty : public PawnProperty
{
	//直线速度
	float MoveSpeed;
	//车身旋转速度
	float RotationSpeed;
};

//装甲车Pawn。
class ArmoredCar :
	public BasePawn
{
public:
	ArmoredCar();
	~ArmoredCar();

	//这一部分负责关于ArmoredCar类的静态属性
public:
	//在PawnMaster中的类型标识。
	static PawnType				pawnType;
	//玩家的控制模式。
	static AIControlType		aiControlType;

	//负责本类生成和销毁指令的PawnMaster。
	static PawnMaster *			pPawnMaster;
	static AICommander *		pAICommander;
	static BoneCommander *		pBoneCommander;
	static CollideCommander *	pCollideCommander;

	//属性分配池
	static MyStaticAllocator<CarProperty>	PropertyAllocator;
	//类对象分配池
	static MyStaticAllocator<ArmoredCar>	PawnAllocator;

public:
	//执行所有注册方法，包括下面的注册PawnMaster。
	static void RegisterAll(
		PawnMaster *		pPawnMaster,
		AICommander *		pAICommander,
		BoneCommander *		pBoneCommander,
		CollideCommander *	pCollideCommander
	);
	//注册PawnMaster，并从PawnMaster中获取一个PawnType。
	static void RegisterPawnMaster(PawnMaster * pPawnMaster);
	//申请一个装甲车属性指针。
	static CarProperty* NewProperty();

	//单位生成模板
	friend class CarPawnTemplate;
	//AI控制模板
	friend class AICommandTemplate;


	//这一部分是关于Player的非静态属性。
protected:
	//Pawn存储指针。
	PawnUnit*		m_pSaveUnit;
	//摄像机对象;
	MyCamera*		m_pCamera;
	//在PlayerCommander中的控制单元指针。
	AIUnit *		m_pAIUnit;
	//PlayerPawn可控制的ControlItem数量，注意保存的都是指针。
	ControlItem*	m_arr_ControlItem[CONTROLITEM_NUM_ARMORED_CAR];
	//所有骨骼，骨骼对应ControlItem。
	Bone*			m_arr_Bones[CONTROLITEM_NUM_ARMORED_CAR];
	//所有碰撞体。
	CollideBox*		m_arr_CBoxes[COLLIDE_RECT_NUM_ARMORED_CAR];

public:
	//指向这个PlayerPawn对象的相关游戏属性，比如前进速度，车身旋转速度，炮弹冷却时间……
	CarProperty*	m_pProperty;

	//一下是一些方便的工具方法。
public:
	//PlayerPawn的根控制器，控制整体的位移，这个网格默认不显示。
	ControlItem *	RootControl();
	//车身控制器，控制车身方向。
	ControlItem *	MainBody();

	//根部的碰撞盒。
	CollideBox *	RootBox();
};



//用于PawnMaster中自动化生成Pawn的命令模板，禁止直接生成Pawn对象。
class CarPawnTemplate : public PawnCommandTemplate
{
public:
	CarPawnTemplate();
	~CarPawnTemplate();

public:
	virtual BasePawn* CreatePawn(PawnProperty* pProperty, Scence* pScence);
	//pPawn是一个动态分配的内存，在销毁对应的Pawn之后，DestoryPawn需要自行释放这个内存到pPlayerAllocator，
	//同时Pawn中的PawnProperty也需要被放回对应的内存池内。
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence);

protected:
	//在AICommander添加一个自动控制单元。
	void AddAIControl		(ArmoredCar* pPawn);
	//在BoneCommander中添加骨骼。
	void AddBones			(ArmoredCar* pPawn);
	//申请碰撞单元。
	void AddCollideBoxes	(ArmoredCar* pPawn);

	void DeleteAIControl	(ArmoredCar* pPawn);
	void DeleteBones		(ArmoredCar* pPawn);
	void DeleteCollideBoxes	(ArmoredCar* pPawn);
};

//AI控制模板。
class CarAITemplate : public AITemplate
{
public:
	//AITemplate父类中有一个vector变量存储了各个状态的维持时间和转换样式，
	//注意在构造函数中添加自定义状态。
	CarAITemplate();
	//根据当前AI的状态，执行需要的控制方法。
	virtual void Runing(
		BasePawn* pPawn,	AIStatue state, 
		float consumedTime, const GameTimer& gt);

	//以下是一些针对ArmoredCar的简化操作。
protected:
	//移动装甲车。
	void move	(ArmoredCar * pCar, const GameTimer& gt);
	//装甲车瞄准。
	void aim	(ArmoredCar * pCar, const GameTimer& gt);
	//装甲车射击。
	void shout	(ArmoredCar * pCar, float consumedTime, const GameTimer& gt);

	//工具方法
protected:
	//让车子的向指定的ControlItem的世界坐标移动。
	void MoveToward(ArmoredCar * pCar, ControlItem * pTarget, const GameTimer& gt);
	//让车子的向指定的坐标移动。
	void MoveToward(ArmoredCar * pCar, XMFLOAT4 targetLocation, const GameTimer& gt);

};