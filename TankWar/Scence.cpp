#include "Scence.h"


Scence::Scence(UINT totalRenderItemNum, UINT totalCameraNum,
	std::unordered_map<std::string, std::unique_ptr<Material>>* pMaterials,
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>* pGeometries)
{
	m_renderItemAllocator = std::make_unique<LinkedAllocator<MyRenderItem>>(totalRenderItemNum);
	m_controlItemAllocator = std::make_unique<LinkedAllocator<ControlItem>>(totalRenderItemNum);
	m_pMaterials = pMaterials;
	m_pGeometries = pGeometries;
}


Scence::~Scence()
{
	if (m_pCamera) 
	{
		delete m_pCamera;
	}
}

void Scence::UpdateData(const GameTimer & gt)
{
	DeLinkedElement<ControlItem>* pHead = m_controlItemAllocator->GetHead();
	DeLinkedElement<ControlItem>* pNode = pHead->m_pNext;
	while (pNode != pHead 
		&& pNode->element.NumFramesDirty > 0)
	{
		//获取ControlItem引用
		ControlItem& controlItem = pNode->element;
		//计数减小。
		--controlItem.NumFramesDirty;

		//计算局部变换矩阵。
		//计算旋转矩阵：俯仰、偏航、翻滚
		XMMATRIX TransformMatrix = XMMatrixRotationRollPitchYaw(
			controlItem.Rotation.x,
			controlItem.Rotation.y, 
			controlItem.Rotation.z);
		//平移
		XMFLOAT4 translation = {
			controlItem.Translation.x,
			controlItem.Translation.y,
			controlItem.Translation.z,
			1.0 };
		//存储平移信息。
		XMStoreFloat4(&translation, TransformMatrix.r[3]);
		//加载相对坐标变换矩阵。
		XMMATRIX referenceCoordinateMatrix = XMLoadFloat4x4(&controlItem.ReferenceCoordinate);
		

		//计算世界变换矩阵。
		TransformMatrix = TransformMatrix * referenceCoordinateMatrix;

		//更新世界变换矩阵。
		XMStoreFloat4x4(&controlItem.World, TransformMatrix);

		//更新ObjectConstants。
		UpdateMainCBForControlItem(&controlItem);

		//迭代
		pNode = pNode->m_pNext;
	}
}

MyCamera * Scence::GetCamera(UINT cameraIndex)
{
	//目前只考虑一个摄像机的情况，不允许添加其他摄像机。
	ASSERT(cameraIndex == 0);
	return m_pCamera;
}

MyCamera * Scence::AppendCamera()
{
	//目前只考虑一个摄像机的情况，不允许添加其他摄像机。
	ASSERT(m_pCamera == nullptr);
	m_pCamera = new MyCamera();
	m_pCamera->Id = 0;
	m_pCamera->Pos = m_controlItemAllocator->Malloc();
	m_pCamera->Target = m_controlItemAllocator->Malloc();
	return m_pCamera;
}


