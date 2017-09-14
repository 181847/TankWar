#pragma once
#include "Common/MathHelper.h"
#include "Common/UploadBuffer.h"
#include "Common/GeometryGenerator.h"
#include "Common/GameTimer.h"
#include <memory>
#include <vector>
#include "FrameResource.h"
#include "LinkedAllocator.h"
#include "ControlItem.h"

//************************************** ��Z��������Ϊǰ	��Y��������Ϊ��		X��������Ϊ��**********************************************


//������ṹ��������Ⱦһ����������Ҫ�����о�ͷ��Ϣ��
struct MyCamera
{
public:
	//���������ţ�һ�������п��Դ�������������ͨ���������������������
	UINT Id;
	//�����λ�ã��涨m_cameraPos.Next��������Ĺ۲�㡣
	ControlItem* Pos;
	//�������Ŀ�ꡣ
	ControlItem* Target;
};

//Scence�����������п���Ⱦ�����λ�ã����Ҹ��������λ�á�
class Scence
{
public:
	//һ��������������Ⱦ���������������������
	//���ò��ʶ���ָ�롢�ܵ��������ָ�롣
	Scence(UINT totalRenderItemNum,	UINT totalCameraNum,
		std::unordered_map<std::string, std::unique_ptr<Material>>* pMaterials,
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>* m_pGeometries);
	~Scence();
	//Scence() = delete;
	//Scence(const Scence&) = delete;
	//Scence& operator = (const Scence&) = delete;

	//��ControlItem�е���ת��ƽ����Ϣ���µ���Ӧ��RenderItem�С�
	void UpdateData(const GameTimer& gt, FrameResource* pCurrFrameResource);

	//������ָ����ŵ��������Ŀǰ�������ֻ�᷵�����Ϊ0����������������������û�д������ͷ���nullptr��
	MyCamera* GetCamera(UINT cameraIndex);
	//�����µ��������Ŀǰ�������û�й���ʵ�֣�ֻ����һ������ͷ�������
	MyCamera* AppendCamera();
	//ɾ���������
	void DeleteCamera(MyCamera* pCamera);

	//����һ��ControlItem��
	//NameofGeometry���������弯�ϵ����֣�
	//NameOfSubmesh��������������֣�
	//NameOfMaterial�����ʵ����֡�
	ControlItem* NewControlItem(const char* NameOfGeometry, const char* NameOfSubmesh, const char* NameOfMaterial);
	//����ControlItem���ڴ档
	void DeleteControlItem(ControlItem* pControlItem);
	//���Ƴ����е�������ʾ�����塣
	void DrawScence(ID3D12GraphicsCommandList * cmdList, FrameResource * pCurrFrameResource);


public:
	//���建���С
	const UINT ObjectConstantsSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	//���ʻ����С
	const UINT MaterialConstantsSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
	//ControlItem����ء�
	LinkedAllocator<ControlItem> m_controlItemAllocator;
	//ָ��������洴�������в��ʣ�ע�⣺���Ǹ�ָ�롣
	std::unordered_map<std::string, std::unique_ptr<Material>>* m_pMaterials;
	//ָ��������洴����������״��ע�⣺���Ǹ�ָ�롣
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>* m_pGeometries;
	//Ŀǰֻ����һ������ͷ�������
	MyCamera* m_pCamera = nullptr;
};