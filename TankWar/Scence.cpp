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
		//��ȡControlItem����
		ControlItem& controlItem = pNode->element;
		//������С��
		--controlItem.NumFramesDirty;

		//����ֲ��任����
		//������ת���󣺸�����ƫ��������
		XMMATRIX TransformMatrix = XMMatrixRotationRollPitchYaw(
			controlItem.Rotation.x,
			controlItem.Rotation.y, 
			controlItem.Rotation.z);
		//ƽ��
		XMFLOAT4 translation = {
			controlItem.Translation.x,
			controlItem.Translation.y,
			controlItem.Translation.z,
			1.0 };
		//�洢ƽ����Ϣ��
		XMStoreFloat4(&translation, TransformMatrix.r[3]);
		//�����������任����
		XMMATRIX referenceCoordinateMatrix = XMLoadFloat4x4(&controlItem.ReferenceCoordinate);
		

		//��������任����
		TransformMatrix = TransformMatrix * referenceCoordinateMatrix;

		//��������任����
		XMStoreFloat4x4(&controlItem.World, TransformMatrix);

		//����ObjectConstants��
		UpdateMainCBForControlItem(&controlItem);

		//����
		pNode = pNode->m_pNext;
	}
}

MyCamera * Scence::GetCamera(UINT cameraIndex)
{
	//Ŀǰֻ����һ����������������������������������
	ASSERT(cameraIndex == 0);
	return m_pCamera;
}

MyCamera * Scence::AppendCamera()
{
	//Ŀǰֻ����һ����������������������������������
	ASSERT(m_pCamera == nullptr);
	m_pCamera = new MyCamera();
	m_pCamera->Id = 0;
	m_pCamera->Pos = m_controlItemAllocator->Malloc();
	m_pCamera->Target = m_controlItemAllocator->Malloc();
	return m_pCamera;
}


