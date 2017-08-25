#pragma once
#include "../../../../Common/d3dUtil.h"
#include "../../../../Common/MathHelper.h"
#include "../../../../Common/UploadBuffer.h"
using namespace DirectX::PackedVector;
using namespace DirectX;

//64Byte��
//ConstantsBuffer���192Byte��
struct ObjectConstants 
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
};

/*
struct MaterialConstants
{
//��������
XMFLOAT4 DiffuseAlbedo;
//��߹��йص�Fresnel���ԣ�����ʹ��(n-1)/(n+1)�ĸ�ʽ����ֵ��n������ʵ������ʣ�����1��������������ʡ�
XMFLOAT3 FresnelR0;
//�ֲڳ̶�
float Roughness;
//����ͼ�йصı任����
XMFLOAT4X4 MatTransform;
};
*/


//256Byte + 128Byte + 16Byte + 16Byte + 16Byte = 432��
//ConstantsBuffer���80Byte��
struct PassConstants
{
	//������ռ�仯����
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	//ͶӰ����
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();

	//������ռ�͸�Ӿ���
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();

	//�۲���λ�á�
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	//����ڴ�ռ�����������ݡ�
	float cbPerObjectPad1 = 0.0f;

	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };

	//��ƽ����롣
	float NearZ = 0.0f;
	//Զƽ����롣
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	//���������еĻ������ա�
	XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	Light lights[16];
};

//12Byte + 16Byte = 28Byte��
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

	//CommandAllocator��
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAllocator;
	//�������ʱ�䡢ͶӰ������Դ��
	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	//����������Դ��
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
	//�õ������в�����Դ��
	std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
	//ר�����ڳ����е���Ⱦ�������Դ��
	std::unique_ptr<UploadBuffer<ObjectConstants>> ScenceObjectCB = nullptr;

	//ͬ��������
	UINT64 Fence = 0;
};

