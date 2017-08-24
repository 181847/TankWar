#include "Scence.h"


Scence::Scence(UINT totalRenderItemNum, UINT totalCameraNum,
	std::unordered_map<std::string, std::unique_ptr<Material>>* pMaterials,
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>* pGeometries)
	:m_controlItemAllocator(totalRenderItemNum)
{

	//对内存池中所有的ControlItem都设置ObjectIndex，在最后的时候添加分配池中。
	for (UINT i = 0; i < m_controlItemAllocator.GetRemainCount(); ++i) {
		m_controlItemAllocator.Malloc()->ObjCBIndex = i;
	}
	
	//获取分配池已分配元素的双重循环链表的头结点。
	auto pHead = m_controlItemAllocator.GetHead();

	//从头结点开始，将所有元素重新回收到内存池内，
	//直到头结点的后继是自身，即已分配元素的数量为0，分配池此时重新获得所有可分配的元素内存。
	while (pHead->m_pNext != pHead)
	{
		//回收这个结点内存。
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

	DeLinkedElement<ControlItem>* pHead = m_controlItemAllocator.GetHead();
	DeLinkedElement<ControlItem>* pNode = pHead->m_pNext;
	while (pNode != pHead)
	{
		if (pNode->element.NumFramesDirty > 0)
		{
			//对应物体的寄存器数据。
			ObjectConstants objectConstants;

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
			TransformMatrix.r[3] = XMLoadFloat4(&translation);
			//加载相对坐标变换矩阵。
			XMMATRIX referenceCoordinateMatrix = XMLoadFloat4x4(&controlItem.ReferenceCoordinate);


			//计算世界变换矩阵，注意：先局部变换、后相对坐标系变换。
			TransformMatrix = TransformMatrix * referenceCoordinateMatrix;

			//更新世界变换矩阵。
			XMStoreFloat4x4(&controlItem.World, TransformMatrix);

			//更新物体信息到ConstantsBuffer中。
			XMStoreFloat4x4(&objectConstants.World, XMMatrixTranspose(TransformMatrix));
		}

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
	m_pCamera->Pos = m_controlItemAllocator.Malloc();
	m_pCamera->Target = m_controlItemAllocator.Malloc();

	//初始化旋转和移动。
	m_pCamera->Pos->Rotation
		= m_pCamera->Pos->Translation
		= m_pCamera->Target->Rotation
		= m_pCamera->Target->Translation
		= { 0.0f, 0.0f, 0.0f };

	//初始化世界坐标和局部坐标。
	m_pCamera->Pos->World
		= m_pCamera->Pos->ReferenceCoordinate
		= m_pCamera->Target->World
		= m_pCamera->Target->ReferenceCoordinate
		= MathHelper::Identity4x4();

	//设定ControlItem的属性为隐藏。
	m_pCamera->Pos->Hide();
	m_pCamera->Target->Hide();

	return m_pCamera;
}

void Scence::DeleteCamera(MyCamera * pCamera)
{
	ASSERT(pCamera && pCamera->Id == 0 && "控制器序号不为0");
	DeleteControlItem(pCamera->Pos);
	DeleteControlItem(pCamera->Target);
}

ControlItem * Scence::NewControlItem(
	const char * NameOfGeometry, 
	const char * NameOfSubmesh, 
	const char * NameOfMaterial)
{
	//ControlItem此时无需设置ObjectIndex，所有的ObjectIndex都应该在Scence初始化的时候分配了序号，
	//详情参考Scence的构造函数。

	//获取图形集合，这个集合中包含了顶点和索引的值。
	MeshGeometry* geo = (*m_pGeometries)[NameOfGeometry].get();
	ControlItem* pNewCItem = m_controlItemAllocator.Malloc();
	pNewCItem->Geo = geo;
	pNewCItem->BaseVertexLocation = geo->DrawArgs[NameOfSubmesh].BaseVertexLocation;
	pNewCItem->IndexCount = geo->DrawArgs[NameOfSubmesh].IndexCount;
	pNewCItem->NumFramesDirty = gNumFrameResources;
	pNewCItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pNewCItem->StartIndexLocation = geo->DrawArgs[NameOfSubmesh].StartIndexLocation;
	//可见性设为显示。
	pNewCItem->Show();
	//设置材质。
	pNewCItem->Mat = (*m_pMaterials)[NameOfMaterial].get();

	//清空变换。
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
	//可见性设为隐藏。
	pControlItem->Hide();
	//清空材质。
	pControlItem->Mat = nullptr;
}


