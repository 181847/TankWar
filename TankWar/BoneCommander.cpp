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
	//翻转状态，按位取反。
	FLIP_THE_STATE(CurrState);
	auto pHead = BoneAllocator.GetHead();
	auto pNode = pHead->m_pNext;

	//遍历整个循环链表
	while (pNode != pHead)
	{
		//检查当前骨骼是否已经被更新。
		if (pNode->element.flipState != CurrState)
		{
			//更新骨骼数据。
			UpdateTheBone(&pNode->element);
		}

		pNode = pNode->m_pNext;
	}
}

void BoneCommander::UpdateTheBone(Bone * pBone)
{
	ASSERT(pBone->flipState != CurrState);
	//判断这个骨骼是否有父骨骼。
	if (pBone->pRoot)
	{

		//如果含有父骨骼，就要先更新父骨骼。
		//检查父骨骼是否已经更新。
		if (pBone->pRoot->flipState != CurrState)
		{
			//父骨骼还没有更新，先更新父骨骼
			UpdateTheBone(pBone->pRoot);
		}

		//当前骨骼控制的ControlItem。
		auto pCItem = pBone->pControlItem;
		//父骨骼控制的ControlItem。
		auto pRefCoodCItem = pBone->pRoot->pControlItem;

		//使用pRefCoodCItem的世界变换矩阵作为pCItem的局部变换矩阵。
		//先计算旋转矩阵
		XMMATRIX TransformMatrix = XMMatrixRotationRollPitchYaw(
			GET_X_Y_Z_Float3_ARGS(pRefCoodCItem->Rotation));
		//平移
		XMFLOAT4 translation = { GET_X_Y_Z_Float3_ARGS(pRefCoodCItem->Translation), 1.0 };
		//存储平移信息。
		TransformMatrix.r[3] = XMLoadFloat4(&translation);
		//加载相对坐标变换矩阵。
		XMMATRIX referenceCoordinateMatrix = XMLoadFloat4x4(&pRefCoodCItem->ReferenceCoordinate);


		//计算世界变换矩阵，注意：先局部变换、后相对坐标系变换。
		TransformMatrix = TransformMatrix * referenceCoordinateMatrix;

		//将这个pRefCoodCItem的计算的局部变换作为自己的相对坐标。
		XMStoreFloat4x4(&pCItem->ReferenceCoordinate, TransformMatrix);

		pCItem->NumFramesDirty = gNumFrameResources;

	}
	FLIP_THE_STATE(pBone->flipState);
}

void Bone::LinkTo(Bone * pRoot)
{
	this->pRoot = pRoot;
}
