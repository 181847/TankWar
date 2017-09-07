#pragma once
#include "BasePawn.h"
#include "ShellPawnProperty.h"

class ShellPawn :
	public BasePawn
{
public:
	ShellPawn();
	~ShellPawn();
	DELETE_COPY_CONSTRUCTOR(ShellPawn)

public:
	//Pawn对象分配池。
	static LinkedAllocator<ShellPawn> PawnAllocator;

public:
	//注册对象，包括Pawn生成模板和AI控制模板的注册。
	static void Register();
	//申请弹药属性。
	static ShellProperty* NewProperty();

protected:
	//AI控制单位。
	AIUnit *		m_pAIUnit;
	//控制器。
	ControlItem *	m_arr_ControlItem[CONTROLITEM_NUM_SHELL];
	//骨骼。
	Bone *			m_arr_Bones[CONTROLITEM_NUM_SHELL];
	//射线检测结构。
	RayDetect *		m_pRayDetect;

public:
	//属性。
	ShellProperty * m_pProperty;

public:
	//根控制器。
	ControlItem *	RootControl();

public:
	friend class	ShellPawnTemplate;
	friend class	ShellAITemplate;
};

class ShellPawnTemplate :
	public PawnCommandTemplate
{
public:
	ShellPawnTemplate();
	~ShellPawnTemplate();
	//删除复制构造函数和等于号重载符。
	DELETE_COPY_CONSTRUCTOR(ShellPawnTemplate)

public:
	virtual BasePawn * CreatePawn(PawnProperty* pProperty, Scence* pScence);
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence);

protected:
	//假设当前ShellPawn的属性任然是初始值，
	//根据属性中的StartPos来设置Pawn的根控制器位置和旋转，
	//并且计算一个方向向量。
	void ReLocateShell(ShellPawn* pPawn);
};

class ShellAITemplate :
	public AITemplate
{
public:
	ShellAITemplate();
	~ShellAITemplate();
	DELETE_COPY_CONSTRUCTOR(ShellAITemplate)


public:
	virtual void Runing(
		BasePawn * pPawn, AIStatue statue,
		float consumedTime, const GameTimer& gt);

protected:
	//按照指定的速度方向移动。
	void move(ShellPawn * pShell, const GameTimer& gt);
	//进行碰撞检测，
	void collideDetect(ShellPawn * pShell, const GameTimer& gt);
	//计算当前帧中，弹药的移动距离，以及计算剩余的移动距离，
	//如果剩余移动距离为0，就要删除弹药Pawn，
	//并且设置弹药属性状态为Dead。
	void caculateDeltaDist(ShellPawn * pShell, const GameTimer& gt);
};


