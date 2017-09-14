#include "Scence.h"


Scence::Scence(UINT totalRenderItemNum, UINT totalCameraNum,
	std::unordered_map<std::string, std::unique_ptr<Material>>* pMaterials,
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>* pGeometries)
	:m_controlItemAllocator(totalRenderItemNum)
{

	//���ڴ�������е�ControlItem������ObjectIndex��������ʱ����ӷ�����С�
	for (UINT i = 0; i < m_controlItemAllocator.GetRemainCount(); ++i) {
		m_controlItemAllocator.Malloc()->ObjCBIndex = i;
	}
	
	//��ȡ������ѷ���Ԫ�ص�˫��ѭ�������ͷ��㡣
	auto pHead = m_controlItemAllocator.GetHead();

	//��ͷ��㿪ʼ��������Ԫ�����»��յ��ڴ���ڣ�
	//ֱ��ͷ���ĺ�����������ѷ���Ԫ�ص�����Ϊ0������ش�ʱ���»�����пɷ����Ԫ���ڴ档
	while (pHead->m_pNext != pHead)
	{
		//�����������ڴ档
		m_controlItemAllocator.Remove(pHead->m_pNext);
	}

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

void Scence::UpdateData(const GameTimer & gt, FrameResource* pCurrFrameResource)
{
	auto* currentCB = pCurrFrameResource->ObjectCB.get();
	auto ObjectCB = pCurrFrameResource->ScenceObjectCB.get();
	//��Ӧ����ļĴ������ݡ�
	ObjectConstants objectConstants;

	DeLinkedElement<ControlItem>* pHead = m_controlItemAllocator.GetHead();
	DeLinkedElement<ControlItem>* pNode = pHead->m_pNext;
	while (pNode != pHead)
	{
		if (pNode->element.NumFramesDirty > 0)
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
			TransformMatrix.r[3] = XMLoadFloat4(&translation);
			//�����������任����
			XMMATRIX referenceCoordinateMatrix = XMLoadFloat4x4(&controlItem.ReferenceCoordinate);


			//��������任����ע�⣺�Ⱦֲ��任�����������ϵ�任��
			TransformMatrix = TransformMatrix * referenceCoordinateMatrix;

			//��������任����
			XMStoreFloat4x4(&controlItem.World, TransformMatrix);

			//����������Ϣ��ConstantsBuffer�С�
			XMStoreFloat4x4(&objectConstants.World, XMMatrixTranspose(TransformMatrix));

			ObjectCB->CopyData(controlItem.ObjCBIndex, objectConstants);
		}

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
	//m_pCamera->Pos = m_controlItemAllocator.Malloc();
	//m_pCamera->Target = m_controlItemAllocator.Malloc();
	m_pCamera->Pos = NewControlItem("Allocator", "Allocator", "box");
	m_pCamera->Target = NewControlItem("Allocator", "Allocator", "box");

	//��ʼ����ת���ƶ���
	m_pCamera->Pos->Rotation
		= m_pCamera->Pos->Translation
		= m_pCamera->Target->Rotation
		= m_pCamera->Target->Translation
		= { 0.0f, 0.0f, 0.0f };

	//��ʼ����������;ֲ����ꡣ
	m_pCamera->Pos->World
		= m_pCamera->Pos->ReferenceCoordinate
		= m_pCamera->Target->World
		= m_pCamera->Target->ReferenceCoordinate
		= MathHelper::Identity4x4();

	//�趨ControlItem������Ϊ���ء�
	m_pCamera->Pos->Hide();
	m_pCamera->Target->Hide();

	return m_pCamera;
}

void Scence::DeleteCamera(MyCamera * pCamera)
{
	ASSERT(pCamera && pCamera->Id == 0 && "��������Ų�Ϊ0");
	DeleteControlItem(pCamera->Pos);
	DeleteControlItem(pCamera->Target);
}

ControlItem * Scence::NewControlItem(
	const char * NameOfGeometry, 
	const char * NameOfSubmesh, 
	const char * NameOfMaterial)
{
	//�鿴�Ƿ���ָ�����Ƶļ����弯�ϡ�
	ASSERT(
		(*m_pGeometries).find(NameOfGeometry) != (*m_pGeometries).end()
		&& "�����ڵļ����弯��");
	//��ȡͼ�μ��ϣ���������а����˶����������ֵ��
	MeshGeometry* geo = (*m_pGeometries)[NameOfGeometry].get();

	//�鿴�Ƿ���ָ�����Ƶ�������
	ASSERT(
		geo->DrawArgs.find(NameOfSubmesh) != geo->DrawArgs.end()
		&& "�����ڵ�������");
	//�鿴�Ƿ���ָ�����ƵĲ��ʡ�
	ASSERT(
		(*m_pMaterials).find(NameOfMaterial) != (*m_pMaterials).end()
		&& "�����ڵĲ���");


	//ControlItem��ʱ��������ObjectIndex�����е�ObjectIndex��Ӧ����Scence��ʼ����ʱ���������ţ�
	//����ο�Scence�Ĺ��캯����
	ControlItem* pNewCItem = m_controlItemAllocator.Malloc();
	pNewCItem->Geo = geo;
	pNewCItem->BaseVertexLocation = geo->DrawArgs[NameOfSubmesh].BaseVertexLocation;
	pNewCItem->IndexCount = geo->DrawArgs[NameOfSubmesh].IndexCount;
	pNewCItem->NumFramesDirty = gNumFrameResources;
	pNewCItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pNewCItem->StartIndexLocation = geo->DrawArgs[NameOfSubmesh].StartIndexLocation;
	//�ɼ�����Ϊ��ʾ��
	pNewCItem->Show();
	//���ò��ʡ�
	pNewCItem->Mat = (*m_pMaterials)[NameOfMaterial].get();

	//��ձ任��
	pNewCItem->Rotation = { 0.0f,0.0f,0.0f };
	pNewCItem->Translation = { 0.0f,0.0f,0.0f };
	pNewCItem->World 
		= pNewCItem->ReferenceCoordinate 
		= MathHelper::Identity4x4();
	return pNewCItem;
}

void Scence::DeleteControlItem(ControlItem * pControlItem)
{
	pControlItem->Geo = nullptr;
	pControlItem->BaseVertexLocation = 0;
	pControlItem->IndexCount = 0;
	pControlItem->NumFramesDirty = 0;
	pControlItem->StartIndexLocation = 0;
	//�ɼ�����Ϊ���ء�
	pControlItem->Hide();
	//��ղ��ʡ�
	pControlItem->Mat = nullptr;
}

void Scence::DrawScence(ID3D12GraphicsCommandList * cmdList, FrameResource * pCurrFrameResource)
{
	auto* currentCB = pCurrFrameResource->ObjectCB.get();
	auto ObjectCB = pCurrFrameResource->ScenceObjectCB.get();
	//��Ӧ����ļĴ������ݡ�
	ObjectConstants objectConstants;

	DeLinkedElement<ControlItem>* pHead = m_controlItemAllocator.GetHead();
	DeLinkedElement<ControlItem>* pNode = pHead->m_pNext;

	//������ʼλ�á�
	D3D12_GPU_VIRTUAL_ADDRESS objectBufferAddress =
		pCurrFrameResource
		->ScenceObjectCB
		->Resource()
		->GetGPUVirtualAddress();
	D3D12_GPU_VIRTUAL_ADDRESS materialbufferAddress =
		pCurrFrameResource
		->MaterialCB
		->Resource()
		->GetGPUVirtualAddress();
	//����������Ҫ�Ļ���λ�á�
	D3D12_GPU_VIRTUAL_ADDRESS perObjectBufferAddress = objectBufferAddress;
	D3D12_GPU_VIRTUAL_ADDRESS perMaterialBufferAddress = materialbufferAddress;

	while (pNode != pHead)
	{
		if (IS_CONTROLITEM_VISIBLE(pNode->element))
		{
			//��ȡControlItem����
			ControlItem& controlItem = pNode->element;


			cmdList->IASetVertexBuffers(0, 1, &controlItem.Geo->VertexBufferView());
			cmdList->IASetIndexBuffer(&controlItem.Geo->IndexBufferView());

			//ƫ�Ƶ�ָ������Ļ���λ�á�
			perObjectBufferAddress = objectBufferAddress + controlItem.ObjCBIndex * ObjectConstantsSize;
			perMaterialBufferAddress = materialbufferAddress + controlItem.Mat->MatCBIndex * MaterialConstantsSize;

			//�趨ͼԪ���͡�
			cmdList->IASetPrimitiveTopology(controlItem.PrimitiveType);
			//�趨���建�塣
			cmdList->SetGraphicsRootConstantBufferView(
				0, perObjectBufferAddress);
			//�趨���ʻ��塣
			cmdList->SetGraphicsRootConstantBufferView(
				1, perMaterialBufferAddress);
			//����ͼԪ��
			cmdList->DrawIndexedInstanced(
				controlItem.IndexCount, 
				1, 
				controlItem.StartIndexLocation, 
				controlItem.BaseVertexLocation, 
				0);
		}

		//����
		pNode = pNode->m_pNext;
	}
}


