#pragma once
#include "../../../../Common/MathHelper.h"
#include "../../../../Common/UploadBuffer.h"
#include "../../../../Common/GeometryGenerator.h"
#include "../../../../Common/GameTimer.h"
#include <memory>
#include <vector>
#include "FrameResource.h"
#include "LinkedAllocator.h"
//************************************** ��Z��������Ϊǰ	��Y��������Ϊ��		X��������Ϊ��**********************************************

struct MyRenderItem
{
	//����任����
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	//��һ��buffer�У����������ڵ�Resource����š�
	UINT ObjCBIndex = 1;

	//���ʶ���
	Material* Mat = nullptr;

	//����������������Ķ���ָ�롣
	MeshGeometry *Geo = nullptr;

	//ͼԪ��ʽ��Ĭ��Ϊ�������б�
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	//����������
	UINT IndexCount = 0;
	//�����Ŀ�ʼ������
	UINT StartIndexLocation = 0;
	//��������λ�á�
	int BaseVertexLocation = 0;

	//�����Ƿ����Ⱦ�����ô��������ز������塣
	//**** ***1��ʾ��Ⱦ���壬	**** ***0��ʾ������Ⱦ���壻
	BYTE Property;
};

struct ControlItem
{
	//��ת��x��Ӧ�����ǣ�y��Ӧ�����ǣ�z��Ӧƫ���ǡ�
	XMFLOAT3 Rotation;

	//ƽ����Ϣ��
	XMFLOAT3 Translation;

	//��һ��ControlItem�����ָ�벻����������Scence�е�ControlItem�ģ�
	//������������Pawn�е�ControlItem�ġ�
	ControlItem* Next;

	//ControlItem���������Ա�ǣ�
	//**** ***1��ʾ��Ⱦ���壬	**** ***0��ʾ������Ⱦ���壻
	//**** **1*��ʾ�����桢		**** **0*��ʾû�б����棻
	//**** *1**��ʾ�������桢	**** *0**��ʾû���������棻
	BYTE Property;

	UINT NumFramesDirty = gNumFrameResources;

	//�����Ƶ���Ⱦ����ָ�롣
	MyRenderItem* pRenderItem;
};

//������ṹ��������Ⱦһ����������Ҫ�����о�ͷ��Ϣ��
struct MyCamera
{
	//���������ţ�һ�������п��Դ�������������ͨ���������������������
	UINT Id;
	//�����λ�ã��涨m_cameraPos.Next��������Ĺ۲�㡣
	ControlItem* m_cameraPos;
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
	//��ControlItem�е���ת��ƽ����Ϣ���µ���Ӧ��RenderItem�С�
	void UpdateData(const GameTimer& gt);
	//�����л�洢�̶��������������ÿ���������Ӧһ����ţ�
	//һ�������������ControlItem�����ơ�
	MyCamera* GetCamera(UINT cameraIndex);
	//�����µ��������Ŀǰ�������û�й���ʵ�֣�ֻ����һ������ͷ�������
	MyCamera* AppendCamera();

public:
	//ControlItem����ء�
	std::unique_ptr<LinkedAllocator<ControlItem>> m_controlItemAllocator;
	//RenderItem����ء�
	std::unique_ptr<LinkedAllocator<MyRenderItem>> m_renderItemAllocator;
	//ָ��������洴�������в��ʣ�ע�⣺���Ǹ�ָ�롣
	std::unordered_map<std::string, std::unique_ptr<Material>>* m_pMaterials;
	//ָ��������洴����������״��ע�⣺���Ǹ�ָ�롣
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>* m_pGeometries;
	//Ŀǰֻ����һ������ͷ�������
	MyCamera* m_pCamera = nullptr;
};