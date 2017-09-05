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
	//���뵯ҩ���ԡ�
	static ShellProperty* NewProperty();

protected:
	//AI���Ƶ�λ��
	AIUnit *		m_pAIUnit;
	//��������
	ControlItem *	m_arr_ControlItem[MAX_SHELL_PAWN_NUM];
	//������
	Bone *			m_arr_Bones[MAX_SHELL_PAWN_NUM];
	//���߼��ṹ��
	RayDetect *		m_pRayDetect;

public:
	ShellProperty * m_pProperty;

public:
	//����������
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

