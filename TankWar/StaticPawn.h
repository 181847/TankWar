#pragma once
#include "StaticPawnProperty.h"


class StaticPawn :
	public BasePawn
{
public:
	StaticPawn();
	~StaticPawn();
	DELETE_COPY_CONSTRUCTOR(StaticPawn)

public:
	//类对象分配池。
	static LinkedAllocator<StaticPawn> PawnAllocator;

public:
	//注册方法。
	static void Register();

	//申请属性。
	static StaticPawnProperty * NewProperty();

	friend class StaticPawnTemplate;

protected:
	ControlItem * m_arr_ControlItem[CONTROLITEM_NUM_STATIC_PAWN];
	Bone * m_arr_Bones[CONTROLITEM_NUM_STATIC_PAWN];
	CollideBox * m_arr_CBoxes[COLLIDE_RECT_NUM_ARMORED_CAR];

public:
	StaticPawnProperty * m_pProperty;

public:
	ControlItem * RootControl();
};

class StaticPawnTemplate :
	public PawnCommandTemplate
{
public:
	StaticPawnTemplate();
	~StaticPawnTemplate();
	DELETE_COPY_CONSTRUCTOR(StaticPawnTemplate)

public:
	virtual BasePawn * CreatePawn(PawnProperty * pProperty, Scence * pScence);
	virtual void DestoryPawn(BasePawn * pPawn, Scence * pScence);

};
