#pragma once
#include "../../../../Common/MathHelper.h"
#include "../../../../Common/UploadBuffer.h"
#include "../../../../Common/GeometryGenerator.h"
#include <memory>
#include <vector>
#include "FrameResource.h"
#include "LinkedAllocator.h"
//************************************** ��Z��������Ϊǰ	��Y��������Ϊ��		X��������Ϊ��**********************************************
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

	//�����Ƶ���Ⱦ����ָ�롣
	RenderItem* pRenderItem;
};

struct RenderItem
{
	//����任����
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	//ʣ���FrameResource����������
	int NumFramesDirty = 0;

	//��һ��buffer�У����������ڵ�Resource����š�
	UINT ObjCBIndex = 1;

	//������š�
	UINT Mat = 0;
	//����������������Ķ�����š�
	UINT Geo = 0;

	//ͼԪ��ʽ��Ĭ��Ϊ�������б�
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	//����������
	UINT IndexCount = 0;
	//�����Ŀ�ʼ������
	UINT StartIndexLocation = 0;
	//��������λ�á�
	int BaseVertexLocation = 0;

	//�����Ƿ����Ⱦ�����ô��������ز������塣
	bool IsVisible;
};

class Scence
{
public:
	//һ�����������Ŀ�����������������Ⱦ����������
	Scence(UINT totalControlItemNum, UINT totalRenderItemNum);
	~Scence();
	//��ControlItem�е���ת��ƽ����Ϣ���µ���Ӧ��RenderItem�С�
	HRESULT UpdateData();
	//�����л�洢�̶��������������ÿ���������Ӧһ����ţ�
	//һ�������������ControlItem�����ơ�
	HRESULT DrawFromCamera(UINT cameraIndex);

public:
	//ControlItem����ء�
	std::unique_ptr<LinkedAllocator<ControlItem>> m_controlItemAllocator;
	//RenderItem����ء�
	std::unique_ptr<LinkedAllocator<RenderItem>> m_renderItemAllocator;
	//������
	std::vector<MeshGeometry> m_meshGeometries;
	//���ʶ���
	std::vector<Material> m_meterials;
};