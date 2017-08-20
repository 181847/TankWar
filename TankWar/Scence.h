#pragma once
#include "../../../../Common/MathHelper.h"
#include "../../../../Common/UploadBuffer.h"
#include "../../../../Common/GeometryGenerator.h"
#include "../../../../Common/GameTimer.h"
#include <memory>
#include <vector>
#include "FrameResource.h"
#include "LinkedAllocator.h"
//************************************** 以Z轴正半轴为前	以Y轴正方向为上		X轴正方向为右**********************************************

struct MyRenderItem
{
	//世界变换矩阵。
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	//在一个buffer中，此物体所在的Resource的序号。
	UINT ObjCBIndex = 1;

	//材质对象。
	Material* Mat = nullptr;

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

	//网格是否可渲染，利用此项来隐藏部分物体。
	//**** ***1显示渲染物体，	**** ***0表示隐藏渲染物体；
	BYTE Property;
};

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

	UINT NumFramesDirty = gNumFrameResources;

	//被控制的渲染物体指针。
	MyRenderItem* pRenderItem;
};

//摄像机结构，包含渲染一个画面所需要的所有镜头信息。
struct MyCamera
{
	//摄像机的序号，一个场景中可以创建多个摄像机，通过这个序号来区分摄像机。
	UINT Id;
	//摄像机位置，规定m_cameraPos.Next是摄像机的观察点。
	ControlItem* m_cameraPos;
};

//Scence用来保存所有可渲染物体的位置，并且更新物体的位置。
class Scence
{
public:
	//一个场景中最多的渲染物体数量、摄像机数量，
	//设置材质对象指针、总的网格对象指针。
	Scence(UINT totalRenderItemNum,	UINT totalCameraNum,
		std::unordered_map<std::string, std::unique_ptr<Material>>* pMaterials,
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>* m_pGeometries);
	~Scence();
	//将ControlItem中的旋转和平移信息更新到对应的RenderItem中。
	void UpdateData(const GameTimer& gt);
	//场景中会存储固定数量个摄像机，每个摄像机对应一个序号，
	//一个摄像机由两个ControlItem来控制。
	MyCamera* GetCamera(UINT cameraIndex);
	//创建新的摄像机，目前这个方法没有功能实现，只考虑一个摄像头的情况。
	MyCamera* AppendCamera();

public:
	//ControlItem分配池。
	std::unique_ptr<LinkedAllocator<ControlItem>> m_controlItemAllocator;
	//RenderItem分配池。
	std::unique_ptr<LinkedAllocator<MyRenderItem>> m_renderItemAllocator;
	//指向程序里面创建的所有材质，注意：这是个指针。
	std::unordered_map<std::string, std::unique_ptr<Material>>* m_pMaterials;
	//指向程序里面创建的所有形状，注意：这是个指针。
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>* m_pGeometries;
	//目前只考虑一个摄像头的情况。
	MyCamera* m_pCamera = nullptr;
};