#pragma once
#include "../../../../Common/d3dUtil.h"
using namespace DirectX;
using namespace DirectX::PackedVector;

//判断ControlItem是否处于显示状态
#define IS_CONTROLITEM_VISIBLE(ControlItemRef) (ControlItemRef.Property & (0x01))

//定义一个简单的从Float3获取三个成员xyz作为函数的参数的宏。
#define GET_X_Y_Z_Float3_ARGS(Float3) Float3.x, Float3.y, Float3.z

//可控制对象。
struct ControlItem
{
	//旋转，x对应俯仰角(Pitch)，y对应偏航角（Yaw），z对应滚动角（Roll）。
	XMFLOAT3 Rotation = {0.0f, 0.0f, 0.0f};

	//平移信息。
	XMFLOAT3 Translation = { 0.0f, 0.0f, 0.0f };

	//相对坐标系，默认是世界坐标，即一个单位矩阵，只能包含旋转和平移变换。
	XMFLOAT4X4 ReferenceCoordinate = MathHelper::Identity4x4();

	//ControlItem的其他属性标记，
	//**** ***1显示渲染物体，	**** ***0表示隐藏渲染物体。
	BYTE Property;

	//对应FrameResource的更新次数。
	UINT NumFramesDirty = gNumFrameResources;

	//世界变换矩阵。
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	//在一个buffer中，此物体所在的Resource的序号。
	UINT ObjCBIndex = 1;

	//材质对象。
	Material* Mat = NULL;

	//包含此物体网格定义的对象指针。
	MeshGeometry *Geo = nullptr;

	//图元格式，默认为三角形列表。
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	//顶点数量。
	UINT IndexCount = 0;
	//索引的开始数量。
	UINT StartIndexLocation = 0;
	//基本顶点位置。
	int BaseVertexLocation = 0;

public:
	//增加偏航角
	void RotateYaw(float d);
	//增加俯仰角
	void RotatePitch(float d);
	//增加滚动角
	void RotateRoll(float d);
	//向X轴正向移动。
	void MoveX(float d);
	//向Y轴正向移动。
	void MoveY(float d);
	//向Z轴正向移动。
	void MoveZ(float d);

	//将ControlItemd的显示属性设为隐藏。
	void Hide();
	//将ControlItemd的显示属性设为显示。
	void Show();
};
