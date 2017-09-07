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
	//Pawn�������ء�
	static LinkedAllocator<ShellPawn> PawnAllocator;

public:
	//ע����󣬰���Pawn����ģ���AI����ģ���ע�ᡣ
	static void Register();
	//���뵯ҩ���ԡ�
	static ShellProperty* NewProperty();

protected:
	//AI���Ƶ�λ��
	AIUnit *		m_pAIUnit;
	//��������
	ControlItem *	m_arr_ControlItem[CONTROLITEM_NUM_SHELL];
	//������
	Bone *			m_arr_Bones[CONTROLITEM_NUM_SHELL];
	//���߼��ṹ��
	RayDetect *		m_pRayDetect;

public:
	//���ԡ�
	ShellProperty * m_pProperty;

public:
	//����������
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
	//ɾ�����ƹ��캯���͵��ں����ط���
	DELETE_COPY_CONSTRUCTOR(ShellPawnTemplate)

public:
	virtual BasePawn * CreatePawn(PawnProperty* pProperty, Scence* pScence);
	virtual void DestoryPawn(BasePawn* pPawn, Scence* pScence);

protected:
	//���赱ǰShellPawn��������Ȼ�ǳ�ʼֵ��
	//���������е�StartPos������Pawn�ĸ�������λ�ú���ת��
	//���Ҽ���һ������������
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
	//����ָ�����ٶȷ����ƶ���
	void move(ShellPawn * pShell, const GameTimer& gt);
	//������ײ��⣬
	void collideDetect(ShellPawn * pShell, const GameTimer& gt);
	//���㵱ǰ֡�У���ҩ���ƶ����룬�Լ�����ʣ����ƶ����룬
	//���ʣ���ƶ�����Ϊ0����Ҫɾ����ҩPawn��
	//�������õ�ҩ����״̬ΪDead��
	void caculateDeltaDist(ShellPawn * pShell, const GameTimer& gt);
};


