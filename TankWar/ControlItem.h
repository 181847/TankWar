#pragma once
#include "../../../../Common/d3dUtil.h"
using namespace DirectX;
using namespace DirectX::PackedVector;

//�ж�ControlItem�Ƿ�����ʾ״̬
#define IS_CONTROLITEM_VISIBLE(ControlItemRef) (ControlItemRef.Property & (0x01))

//����һ���򵥵Ĵ�Float3��ȡ������Աxyz��Ϊ�����Ĳ����ĺꡣ
#define GET_X_Y_Z_Float3_ARGS(Float3) Float3.x, Float3.y, Float3.z

//�ɿ��ƶ���
struct ControlItem
{
	//��ת��x��Ӧ������(Pitch)��y��Ӧƫ���ǣ�Yaw����z��Ӧ�����ǣ�Roll����
	XMFLOAT3 Rotation = {0.0f, 0.0f, 0.0f};

	//ƽ����Ϣ��
	XMFLOAT3 Translation = { 0.0f, 0.0f, 0.0f };

	//�������ϵ��Ĭ�����������꣬��һ����λ����ֻ�ܰ�����ת��ƽ�Ʊ任��
	XMFLOAT4X4 ReferenceCoordinate = MathHelper::Identity4x4();

	//ControlItem���������Ա�ǣ�
	//**** ***1��ʾ��Ⱦ���壬	**** ***0��ʾ������Ⱦ���塣
	BYTE Property;

	//��ӦFrameResource�ĸ��´�����
	UINT NumFramesDirty = gNumFrameResources;

	//����任����
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	//��һ��buffer�У����������ڵ�Resource����š�
	UINT ObjCBIndex = 1;

	//���ʶ���
	Material* Mat = NULL;

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

public:
	//���Ӹ�����
	void RotatePitch(float dx);
	//����ƫ����
	void RotateYaw(float dy);
	//���ӹ�����
	void RotateRoll(float dz);
	//��X�������ƶ���
	void MoveX(float dx);
	//��Y�������ƶ���
	void MoveY(float dy);
	//��Z�������ƶ���
	void MoveZ(float dz);

	//��XYZ��ͬʱ�ƶ���
	void MoveXYZ(float dx, float dy, float dz);

	//��ControlItemd����ʾ������Ϊ���ء�
	void Hide();
	//��ControlItemd����ʾ������Ϊ��ʾ��
	void Show();
};
