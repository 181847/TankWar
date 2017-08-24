#include "BoneCommander.h"

extern const int gNumFrameResources;

BoneCommander::BoneCommander(UINT maxBoneNum)
	:BoneAllocator(maxBoneNum)
{
}


BoneCommander::~BoneCommander()
{
}

Bone * BoneCommander::NewBone(ControlItem * pControlItem)
{
	auto pNewBone = BoneAllocator.Malloc();
	pNewBone->pControlItem = pControlItem;
	pNewBone->flipState = this->CurrState;
	pNewBone->pRoot = nullptr;
	return pNewBone;
}

void BoneCommander::DeleteBone(Bone * pTheBone)
{
	pTheBone->pRoot = nullptr;
	pTheBone->pControlItem = nullptr;
	pTheBone->flipState = FLIP_STATE_1;
	BoneAllocator.Free(pTheBone);
}

void BoneCommander::Update()
{
	//��ת״̬����λȡ����
	FLIP_THE_STATE(CurrState);
	auto pHead = BoneAllocator.GetHead();
	auto pNode = pHead->m_pNext;

	//��������ѭ������
	while (pNode != pHead)
	{
		//��鵱ǰ�����Ƿ��Ѿ������¡�
		if (pNode->element.flipState != CurrState)
		{
			//���¹������ݡ�
			UpdateTheBone(&pNode->element);
		}

		pNode = pNode->m_pNext;
	}
}

void BoneCommander::UpdateTheBone(Bone * pBone)
{
	ASSERT(pBone->flipState != CurrState);
	//�ж���������Ƿ��и�������
	if (pBone->pRoot)
	{

		//������и���������Ҫ�ȸ��¸�������
		//��鸸�����Ƿ��Ѿ����¡�
		if (pBone->pRoot->flipState != CurrState)
		{
			//��������û�и��£��ȸ��¸�����
			UpdateTheBone(pBone->pRoot);
		}

		//��ǰ�������Ƶ�ControlItem��
		auto pCItem = pBone->pControlItem;
		//���������Ƶ�ControlItem��
		auto pRefCoodCItem = pBone->pRoot->pControlItem;

		//ʹ��pRefCoodCItem������任������ΪpCItem�ľֲ��任����
		//�ȼ�����ת����
		XMMATRIX TransformMatrix = XMMatrixRotationRollPitchYaw(
			GET_X_Y_Z_Float3_ARGS(pRefCoodCItem->Rotation));
		//ƽ��
		XMFLOAT4 translation = { GET_X_Y_Z_Float3_ARGS(pRefCoodCItem->Translation), 1.0 };
		//�洢ƽ����Ϣ��
		TransformMatrix.r[3] = XMLoadFloat4(&translation);
		//�����������任����
		XMMATRIX referenceCoordinateMatrix = XMLoadFloat4x4(&pRefCoodCItem->ReferenceCoordinate);


		//��������任����ע�⣺�Ⱦֲ��任�����������ϵ�任��
		TransformMatrix = TransformMatrix * referenceCoordinateMatrix;

		//�����pRefCoodCItem�ļ���ľֲ��任��Ϊ�Լ���������ꡣ
		XMStoreFloat4x4(&pCItem->ReferenceCoordinate, TransformMatrix);

		pCItem->NumFramesDirty = gNumFrameResources;

	}
	FLIP_THE_STATE(pBone->flipState);
}

void Bone::LinkTo(Bone * pRoot)
{
	this->pRoot = pRoot;
}
