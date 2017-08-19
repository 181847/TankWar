#pragma once
#include "../../../../Common/MathHelper.h"
#include "../../../../Common/UploadBuffer.h"
#include "../../../../Common/GeometryGenerator.h"
#include <memory>
#include <vector>
#include "FrameResource.h"
#include "LinkedAllocator.h"
//************************************** 以Z轴正半轴为前	以Y轴正方向为上		X轴正方向为右**********************************************
struct ControlItem
{
	//旋转，x对应翻滚角，y对应俯仰角，z对应偏航角。
	XMFLOAT3 Rotation;

	//平移信息。
	XMFLOAT3 Translation;

	//下一个ControlItem，这个指针不是用来连接Scence中的ControlItem的，
	//而是用来连接Pawn中的ControlItem的。
	ControlItem* Next;

	//ControlItem的其他属性标记，
	//**** ***1显示渲染物体，	**** ***0表示隐藏渲染物体；
	//**** **1*表示被跟随、		**** **0*表示没有被跟随；
	//**** *1**表示主动跟随、	**** *0**表示没有主动跟随；
	BYTE Property;

	//被控制的渲染物体指针。
	RenderItem* pRenderItem;
};

struct RenderItem
{
	//世界变换矩阵。
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	//剩余的FrameResource更新数量。
	int NumFramesDirty = 0;

	//在一个buffer中，此物体所在的Resource的序号。
	UINT ObjCBIndex = 1;

	//材质序号。
	UINT Mat = 0;
	//包含此物体网格定义的对象序号。
	UINT Geo = 0;

	//图元格式，默认为三角形列表。
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	//顶点数量。
	UINT IndexCount = 0;
	//索引的开始数量。
	UINT StartIndexLocation = 0;
	//基本顶点位置。
	int BaseVertexLocation = 0;

	//网格是否可渲染，利用此项来隐藏部分物体。
	bool IsVisible;
};

class Scence
{
public:
	//一个场景中最多的控制物体数量，和渲染物体数量。
	Scence(UINT totalControlItemNum, UINT totalRenderItemNum);
	~Scence();
	//将ControlItem中的旋转和平移信息更新到对应的RenderItem中。
	HRESULT UpdateData();
	//场景中会存储固定数量个摄像机，每个摄像机对应一个序号，
	//一个摄像机由两个ControlItem来控制。
	HRESULT DrawFromCamera(UINT cameraIndex);

public:
	//ControlItem分配池。
	std::unique_ptr<LinkedAllocator<ControlItem>> m_controlItemAllocator;
	//RenderItem分配池。
	std::unique_ptr<LinkedAllocator<RenderItem>> m_renderItemAllocator;
	//网格定义
	std::vector<MeshGeometry> m_meshGeometries;
	//材质定义
	std::vector<Material> m_meterials;
};