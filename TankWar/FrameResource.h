#pragma once
#include "../../../../Common/d3dUtil.h"
#include "../../../../Common/MathHelper.h"
#include "../../../../Common/UploadBuffer.h"
using namespace DirectX::PackedVector;
using namespace DirectX;

//64Byte，
//ConstantsBuffer填充192Byte。
struct ObjectConstants 
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
};

/*
struct MaterialConstants
{
//漫反射率
XMFLOAT4 DiffuseAlbedo;
//与高光有关的Fresnel属性，建议使用(n-1)/(n+1)的格式来赋值，n代表材质的折射率，其中1代表空气的折射率。
XMFLOAT3 FresnelR0;
//粗糙程度
float Roughness;
//与贴图有关的变换矩阵。
XMFLOAT4X4 MatTransform;
};
*/


//256Byte + 128Byte + 16Byte + 16Byte + 16Byte = 432，
//ConstantsBuffer填充80Byte。
struct PassConstants
{
	//摄像机空间变化矩阵。
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	//投影矩阵。
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();

	//摄像机空间透视矩阵。
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();

	//观察者位置。
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	//填充内存空间的无意义数据。
	float cbPerObjectPad1 = 0.0f;

	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };

	//近平面距离。
	float NearZ = 0.0f;
	//远平面距离。
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	//整个场景中的环境光照。
	XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	Light lights[16];
};

//12Byte + 16Byte = 28Byte。
struct Vertex 
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
};

struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT scenceObjectCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	//CommandAllocator。
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAllocator;
	//摄像机、时间、投影坐标资源。
	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	//物体坐标资源。
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
	//用到的所有材质资源。
	std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
	//专门用于场景中的渲染物体的资源。
	std::unique_ptr<UploadBuffer<ObjectConstants>> ScenceObjectCB = nullptr;

	//同步计数。
	UINT64 Fence = 0;
};

