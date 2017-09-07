#pragma once
#include "ArmoredCarPawnProperty.h"
#include "PlayerPawn.h"

//装甲车Pawn。
class ArmoredCar :
	public BasePawn
{
public:
	ArmoredCar();
	~ArmoredCar();

	//这一部分负责关于ArmoredCar类的静态属性
public:

	//类对象分配池
	static MyStaticAllocator<ArmoredCar>	PawnAllocator;

public:
	//执行所有注册方法。
	static void Register();

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