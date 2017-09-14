#include "StaticPawn.h"

LinkedAllocator<StaticPawn> StaticPawn::PawnAllocator(MAX_STATIC_PAWN_NUM);

StaticPawn::StaticPawn()
{
}


StaticPawn::~StaticPawn()
{
}

void StaticPawn::Register()
{
	gStaticPawnType = gPawnMaster->AddCommandTemplate(
		std::make_unique<StaticPawnTemplate>());
}

StaticPawnProperty * StaticPawn::NewProperty()
{
	return gStaticPawnPropertyAllocator.Malloc();
}

ControlItem * StaticPawn::RootControl()
{
	return m_arr_ControlItem[CONTROLITEM_INDEX_STATIC_PAWN_ROOT];
}

//*****************************玩家生成模板***********************************

StaticPawnTemplate::StaticPawnTemplate()
{
}

StaticPawnTemplate::~StaticPawnTemplate()
{
}

BasePawn * StaticPawnTemplate::CreatePawn(
	PawnProperty * pProperty, Scence * pScence)
{
	ASSERT(pProperty && "StaticPawn必须指定属性才能完成初始化");

	auto m_pProperty = reinterpret_cast<StaticPawnProperty *>(pProperty);
	auto pNewPawn = StaticPawn::PawnAllocator.Malloc();

	pNewPawn->m_pawnType = gStaticPawnType;
	pNewPawn->m_pProperty = m_pProperty;

	//申请控制器。
	pNewPawn->m_arr_ControlItem[CONTROLITEM_INDEX_STATIC_PAWN_ROOT]
		= pScence->NewControlItem(
			m_pProperty->GeometryName,
			m_pProperty->ShapeName, 
			m_pProperty->MaterialName);
	//按照属性来设置平移和旋转。
	pNewPawn->RootControl()->RotateXYZ(
		m_pProperty->InitRotation.x, 
		m_pProperty->InitRotation.y, 
		m_pProperty->InitRotation.z);
	pNewPawn->RootControl()->MoveXYZ(
		m_pProperty->InitTranslation.x,
		m_pProperty->InitTranslation.y,
		m_pProperty->InitTranslation.z);

	//申请骨骼。
	pNewPawn->m_arr_Bones[BONE_INDEX_STATIC_PAWN_ROOT]
		= gBoneCommander->NewBone(pNewPawn->RootControl());
	
	//申请碰撞盒
	pNewPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_STATIC_PAWN_ROOT]
		= gCollideCommander-> NewCollideBox(
			m_pProperty->CBoxType, 
			pNewPawn->RootControl(),
			pNewPawn);

	//修改碰撞盒的大小。
	pNewPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_STATIC_PAWN_ROOT]
		->Resize(
			m_pProperty->CBoxSize.XM_MinXYZ, 
			m_pProperty->CBoxSize.XM_MaxXYZ);

	return pNewPawn;
}

void StaticPawnTemplate::DestoryPawn(BasePawn * pPawn, Scence * pScence)
{
	auto pStaticPawn = reinterpret_cast<StaticPawn * >(pPawn);

	//删除控制器。
	pScence->DeleteControlItem(pStaticPawn->RootControl());

	//回收属性空间。
	gStaticPawnPropertyAllocator.Free(pStaticPawn->m_pProperty);

	//删除骨骼。
	gBoneCommander->DeleteBone(pStaticPawn->m_arr_Bones[BONE_INDEX_STATIC_PAWN_ROOT]);

	//删除碰撞盒。
	gCollideCommander->DeleteCollideBox(pStaticPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_STATIC_PAWN_ROOT]);

	//收回Pawn空间。
	StaticPawn::PawnAllocator.Free(pStaticPawn);
}
