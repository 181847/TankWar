#pragma once
#include "../../../../Common/d3dApp.h"
#include "../../../../Common/MathHelper.h"
#include "../../../../Common/UploadBuffer.h"
#include "../../../../Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "Scence.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

//FrameResource数量。
//一个FrameResource包含一次绘图操作中所有要用到的CommandAllocator和IResource。
const int gNumFrameResources = 3;

//灯光数量。
const int gNumDirectLights = 3;

struct RenderItem 
{
	RenderItem() = default;

	//相对世界坐标。
	XMFLOAT4X4 World = MathHelper::Identity4x4();


	//剩余的FrameResource更新数量。
	int NumFramesDirty = gNumFrameResources;

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
};


class MyShapesApp : public D3DApp
{
public:
	MyShapesApp(HINSTANCE hInstance);
	//删除所有的复制函数。
	MyShapesApp(const MyShapesApp& rhs) = delete;
	MyShapesApp& operator=(const MyShapesApp& rhs) = delete;
	~MyShapesApp();

	//初始化操作。
	virtual bool Initialize() override;

private:
	//更新窗口大小。
	virtual void OnResize()override;
	//更新物体和摄像机空间数据。
	virtual void Update(const GameTimer& gt)override;
	//向GPU记录绘制一帧画面的Command。
	virtual void Draw(const GameTimer& gt)override;

	//鼠标事件响应。
	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);
	void UpdateCamera(const GameTimer& gt);
	void UpdateLights(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	//灯光数据也在这里更新。
	void UpdateMainPassCB(const GameTimer& gt);
	void UpdateMaterialCB(const GameTimer& gt);


	//创建一个DescriptorHeap，这个Heap中存储了程序中所有要用到Resource的Descriptor，
	//包括每个FrameResource中的资源的Descriptor都会在这一个Heap中。
	void BuildDescriptorHeap();
	//为各个ConstantBuffer创建view。
	void BuildConstantsBufferView();
	//创建RootSignature，用于Pipline中所有Shader的参数定义
	void BuildRootSignature();
	//编译Shader和创建顶点输入格式。
	void BuildShadersAndInputLayout();
	//创建灯光。
	void BuildLights();
	//创建材质。
	void BuildMaterials();
	//创建所有要用到的网格，注：只是网格，不是可渲染对象。
	void BuildShapeGeometry();
	//创建PiplinStateObject。
	void BuildPSOs();
	//创建每帧资源。
	void BuildFrameResources();
	//创建所有可渲染对象。
	void BuildRenderItems();

	//向CommandList记录渲染RenderItem中所有物体的CommandList。
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	//辅助计算材质所需的Fresnel特征值，
	//refractionIndex代表折射率，1代表空气的折射率。
	XMFLOAT3 HelpCalculateFresnelR0(float refractionIndex);

private:
	//每一个帧需要的资源。
	std::vector<std::unique_ptr<FrameResource>> mFrameResource;
	//当前正在渲染的Frame资源指针。
	FrameResource* mCurrFrameResource = nullptr;
	//当前正在渲染的Frame资源序号。
	int mCurrFrameResourceIndex = 0;

	//RootSignature，存储了Pipline中所需要资源的参数形式。
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	//所有ConstantsBuffer的View。
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	//本程序中未用到的成员变量，功能不明。
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	//所有的灯光。
	std::unordered_map<std::string, std::unique_ptr<Light>> mLights;
	//所有的材质。
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	//存储多个网格资源的集合，一个玩个资源内部可能又包含多个物体网格，
	//本程序中只使用了一个网格资源，其中包含所有要用到的网格物体。
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	//存储所有编译好的Shader。
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	//所有PipelineStateObject，后期可能会分成透明和不透明的PSO。
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	//顶点格式声明。
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	//所有可渲染对象的定义。
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	//所有不透明渲染对象指针，所有指针是mAllRitems中的具体指针。
	std::vector<RenderItem*> mOpaqueRitems;

	//关于摄像机、投影矩阵、时间的相关信息。
	PassConstants mMainPassCB;

	//记录了在mCbvHeap中，PassConstants所在的序号位置，
	//方便偏移到对应的PassConstantsHandle。
	UINT mPassCbvOffset = 0;

	//是否以线框形式渲染。
	bool mIsWireframe = false;

	//摄像机位置。
	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
	//世界至观察矩阵。
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	//投影矩阵。
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	//经度。
	float mTheta = 1.5f*XM_PI;
	//纬度。
	float mPhi = 0.2f*XM_PI;
	//半径。
	float mRadius = 15.0f;

	POINT mLastMousePos;

	//此角度指向主要灯光。
	float mKeyLightTheta = 1.5f*XM_PI;
	//此角度指向主要灯光
	float mKeyLightPhi = 0.2f*XM_PI;
	float mKeyLightRadius = 1.0f;


	//*******************************************游戏代码定义部分**************************************************************************************************************************************************
public:
	void UpdateScence(const GameTimer& gt);
	//*******************************************游戏代码定义部分**************************************************************************************************************************************************
};
