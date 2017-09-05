#pragma once
#include "BasePawn.h"
#include "ShellProperty.h"

class ShellPawn :
	public BasePawn
{
public:
	ShellPawn();
	ShellPawn(const ShellPawn&) = delete;
	ShellPawn& operator = (const ShellPawn&) = delete;
	~ShellPawn();

public:
	//申请弹药属性。
	static ShellProperty* NewProperty();

protected:
	//AI控制单位。
	AIUnit *		m_pAIUnit;
	//控制器。
	ControlItem *	m_arr_ControlItem[MAX_SHELL_PAWN_NUM];
	//骨骼。
	Bone *			m_arr_Bones[MAX_SHELL_PAWN_NUM];
	//射线检测结构。
	RayDetect *		m_pRayDetect;

public:
	ShellProperty * m_pProperty;

public:
	//根控制器。
	ControlItem * RootControl();
};

class ShellPawnTemplate :
	public PawnCommandTemplate
{
public:
	ShellPawnTemplate();
	ShellPawnTemplate(const ShellPawnTemplate&) = delete;
	ShellPawnTemplate& operator = (const ShellPawnTemplate&) = delete;
	~ShellPawnTemplate();
};

