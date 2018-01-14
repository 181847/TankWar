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

//************************************** 以Z轴正半轴为前	以Y轴正方向为上		X轴正方向为右**********************************************


//摄像机结构，包含渲染一个画面所需要的所有镜头信息。
struct MyCamera
{
public:
	//摄像机的序号，一个场景中可以创建多个摄像机，通过这个序号来区分摄像机。
	UINT Id;
	//摄像机位置，规定m_cameraPos.Next是摄像机的观察点。
	ControlItem* Pos;
	//摄像机的目标。
	ControlItem* Target;
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
	//Scence() = delete;
	//Scence(const Scence&) = delete;
	//Scence& operator = (const Scence&) = delete;

	//将ControlItem中的旋转和平移信息更新到对应的RenderItem中。
	void UpdateData(const GameTimer& gt, FrameResource* pCurrFrameResource);

	//场景中指定序号的摄像机，目前这个方法只会返回序号为0的摄像机，如果这个摄像机还没有创建，就返回nullptr。
	MyCamera* GetCamera(UINT cameraIndex);
	//创建新的摄像机，目前这个方法没有功能实现，只考虑一个摄像头的情况。
	MyCamera* AppendCamera();
	//删除摄像机。
	void DeleteCamera(MyCamera* pCamera);

	//返回一个ControlItem，
	//NameofGeometry，几何形体集合的名字；
	//NameOfSubmesh，具体网格的名字；
	//NameOfMaterial，材质的名字。
	ControlItem* NewControlItem(const char* NameOfGeometry, const char* NameOfSubmesh, const char* NameOfMaterial);
	//回收ControlItem的内存。
	void DeleteControlItem(ControlItem* pControlItem);
	//绘制场景中的所有显示的物体。
	void DrawScence(ID3D12GraphicsCommandList * cmdList, FrameResource * pCurrFrameResource);


public:
	//物体缓冲大小
	const UINT ObjectConstantsSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	//材质缓冲大小
	const UINT MaterialConstantsSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
	//ControlItem分配池。
	LinkedAllocator<ControlItem> m_controlItemAllocator;
	//指向程序里面创建的所有材质，注意：这是个指针。
	std::unordered_map<std::string, std::unique_ptr<Material>>* m_pMaterials;
	//指向程序里面创建的所有形状，注意：这是个指针。
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>* m_pGeometries;
	//目前只考虑一个摄像头的情况。
	MyCamera* m_pCamera = nullptr;
};