#include "FrameResource.h"



FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT scenceObjectCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAllocator.GetAddressOf())));
	PassCB = std::make_unique<UploadBuffer<PassConstants>>(
		device, 
		passCount,		//��Ӧ��������ע��������GPUʵ����Դ��С��256 * passCount���Ա�֤ÿһ������Դ���׵�ַ����256�ֽڶ��롣
		true);
	ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(
		device,
		objectCount,
		true);
	MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(
		device,
		materialCount,
		true);
	ScenceObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(
		device,
		scenceObjectCount,
		true
		);
	Fence = 0;
}


FrameResource::~FrameResource()
{
}
