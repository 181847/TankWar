#include "Scence.h"


Scence::Scence(UINT totalRenderItemNum, UINT totalCameraNum,
	std::unordered_map<std::string, std::unique_ptr<Material>>* pMaterials,
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>* pGeometries)
{
	m_renderItemAllocator = std::make_unique<LinkedAllocator<RenderItem>>(totalRenderItemNum);
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
		&& pNode->element.NumFramesDirty > 0
		&& pNode->element.pRenderItem)
	{
		//计数减小。
		--pNode->element.NumFramesDirty;
		XMFLOAT3 rotation = pNode->element.Rotation;
		XMFLOAT3 translation = pNode->element.Translation;

		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		XMMATRIX translationMatrix = XMMatrixTranslation(translation.x, translation.y, translation.z);
		
		//计算世界变换矩阵。
		translationMatrix = rotationMatrix * translationMatrix;
		
		//更新RenderItem中的矩阵。
		XMStoreFloat4x4(&pNode->element.pRenderItem->World, translationMatrix);
	}
}

Camera * Scence::AppendCamera()
{
	m_pCamera = new Camera();
	m_pCamera->Id = 0;
	m_pCamera->m_cameraPos = m_controlItemAllocator->Malloc();
	m_pCamera->m_cameraPos->Next = m_controlItemAllocator->Malloc();
	m_pCamera->m_cameraPos->Next->Next = nullptr;
	return m_pCamera;
}


