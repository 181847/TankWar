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

//*****************************�������ģ��***********************************

StaticPawnTemplate::StaticPawnTemplate()
{
}

StaticPawnTemplate::~StaticPawnTemplate()
{
}

BasePawn * StaticPawnTemplate::CreatePawn(
	PawnProperty * pProperty, Scence * pScence)
{
	ASSERT(pProperty && "StaticPawn����ָ�����Բ�����ɳ�ʼ��");

	auto m_pProperty = reinterpret_cast<StaticPawnProperty *>(pProperty);
	auto pNewPawn = StaticPawn::PawnAllocator.Malloc();

	pNewPawn->m_pawnType = gStaticPawnType;
	pNewPawn->m_pProperty = m_pProperty;

	//�����������
	pNewPawn->m_arr_ControlItem[CONTROLITEM_INDEX_STATIC_PAWN_ROOT]
		= pScence->NewControlItem(
			m_pProperty->GeometryName,
			m_pProperty->ShapeName, 
			m_pProperty->MaterialName);
	//��������������ƽ�ƺ���ת��
	pNewPawn->RootControl()->RotateXYZ(
		m_pProperty->InitRotation.x, 
		m_pProperty->InitRotation.y, 
		m_pProperty->InitRotation.z);
	pNewPawn->RootControl()->MoveXYZ(
		m_pProperty->InitTranslation.x,
		m_pProperty->InitTranslation.y,
		m_pProperty->InitTranslation.z);

	//���������
	pNewPawn->m_arr_Bones[BONE_INDEX_STATIC_PAWN_ROOT]
		= gBoneCommander->NewBone(pNewPawn->RootControl());
	
	//������ײ��
	pNewPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_STATIC_PAWN_ROOT]
		= gCollideCommander-> NewCollideBox(
			m_pProperty->CBoxType, 
			pNewPawn->RootControl(),
			pNewPawn);

	//�޸���ײ�еĴ�С��
	pNewPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_STATIC_PAWN_ROOT]
		->Resize(
			m_pProperty->CBoxSize.XM_MinXYZ, 
			m_pProperty->CBoxSize.XM_MaxXYZ);

	return pNewPawn;
}

void StaticPawnTemplate::DestoryPawn(BasePawn * pPawn, Scence * pScence)
{
	auto pStaticPawn = reinterpret_cast<StaticPawn * >(pPawn);

	//ɾ����������
	pScence->DeleteControlItem(pStaticPawn->RootControl());

	//�������Կռ䡣
	gStaticPawnPropertyAllocator.Free(pStaticPawn->m_pProperty);

	//ɾ��������
	gBoneCommander->DeleteBone(pStaticPawn->m_arr_Bones[BONE_INDEX_STATIC_PAWN_ROOT]);

	//ɾ����ײ�С�
	gCollideCommander->DeleteCollideBox(pStaticPawn->m_arr_CBoxes[COLLIDE_RECT_INDEX_STATIC_PAWN_ROOT]);

	//�ջ�Pawn�ռ䡣
	StaticPawn::PawnAllocator.Free(pStaticPawn);
}
