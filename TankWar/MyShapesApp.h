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

//FrameResource������
//һ��FrameResource����һ�λ�ͼ����������Ҫ�õ���CommandAllocator��IResource��
const int gNumFrameResources = 3;

//�ƹ�������
const int gNumDirectLights = 3;

struct RenderItem 
{
	RenderItem() = default;

	//����������ꡣ
	XMFLOAT4X4 World = MathHelper::Identity4x4();


	//ʣ���FrameResource����������
	int NumFramesDirty = gNumFrameResources;

	//��һ��buffer�У����������ڵ�Resource����š�
	UINT ObjCBIndex = 1;

	//���ʶ���
	Material* Mat = nullptr;
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
};


class MyShapesApp : public D3DApp
{
public:
	MyShapesApp(HINSTANCE hInstance);
	//ɾ�����еĸ��ƺ�����
	MyShapesApp(const MyShapesApp& rhs) = delete;
	MyShapesApp& operator=(const MyShapesApp& rhs) = delete;
	~MyShapesApp();

	//��ʼ��������
	virtual bool Initialize() override;

private:
	//���´��ڴ�С��
	virtual void OnResize()override;
	//���������������ռ����ݡ�
	virtual void Update(const GameTimer& gt)override;
	//��GPU��¼����һ֡�����Command��
	virtual void Draw(const GameTimer& gt)override;

	//����¼���Ӧ��
	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);
	void UpdateCamera(const GameTimer& gt);
	void UpdateLights(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	//�ƹ�����Ҳ��������¡�
	void UpdateMainPassCB(const GameTimer& gt);
	void UpdateMaterialCB(const GameTimer& gt);


	//����һ��DescriptorHeap�����Heap�д洢�˳���������Ҫ�õ�Resource��Descriptor��
	//����ÿ��FrameResource�е���Դ��Descriptor��������һ��Heap�С�
	void BuildDescriptorHeap();
	//Ϊ����ConstantBuffer����view��
	void BuildConstantsBufferView();
	//����RootSignature������Pipline������Shader�Ĳ�������
	void BuildRootSignature();
	//����Shader�ʹ������������ʽ��
	void BuildShadersAndInputLayout();
	//�����ƹ⡣
	void BuildLights();
	//�������ʡ�
	void BuildMaterials();
	//��������Ҫ�õ�������ע��ֻ�����񣬲��ǿ���Ⱦ����
	void BuildShapeGeometry();
	//����PiplinStateObject��
	void BuildPSOs();
	//����ÿ֡��Դ��
	void BuildFrameResources();
	//�������п���Ⱦ����
	void BuildRenderItems();

	//��CommandList��¼��ȾRenderItem�����������CommandList��
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	//����������������Fresnel����ֵ��
	//refractionIndex���������ʣ�1��������������ʡ�
	XMFLOAT3 HelpCalculateFresnelR0(float refractionIndex);

private:
	//ÿһ��֡��Ҫ����Դ��
	std::vector<std::unique_ptr<FrameResource>> mFrameResource;
	//��ǰ������Ⱦ��Frame��Դָ�롣
	FrameResource* mCurrFrameResource = nullptr;
	//��ǰ������Ⱦ��Frame��Դ��š�
	int mCurrFrameResourceIndex = 0;

	//RootSignature���洢��Pipline������Ҫ��Դ�Ĳ�����ʽ��
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	//����ConstantsBuffer��View��
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	//��������δ�õ��ĳ�Ա���������ܲ�����
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	//���еĵƹ⡣
	std::unordered_map<std::string, std::unique_ptr<Light>> mLights;
	//���еĲ��ʡ�
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	//�洢���������Դ�ļ��ϣ�һ�������Դ�ڲ������ְ��������������
	//��������ֻʹ����һ��������Դ�����а�������Ҫ�õ����������塣
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	//�洢���б���õ�Shader��
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	//����PipelineStateObject�����ڿ��ܻ�ֳ�͸���Ͳ�͸����PSO��
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	//�����ʽ������
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	//���п���Ⱦ����Ķ��塣
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	//���в�͸����Ⱦ����ָ�룬����ָ����mAllRitems�еľ���ָ�롣
	std::vector<RenderItem*> mOpaqueRitems;

	//�����������ͶӰ����ʱ��������Ϣ��
	PassConstants mMainPassCB;

	//��¼����mCbvHeap�У�PassConstants���ڵ����λ�ã�
	//����ƫ�Ƶ���Ӧ��PassConstantsHandle��
	UINT mPassCbvOffset = 0;

	//�Ƿ����߿���ʽ��Ⱦ��
	bool mIsWireframe = false;

	//�����λ�á�
	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
	//�������۲����
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	//ͶӰ����
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	//���ȡ�
	float mTheta = 1.5f*XM_PI;
	//γ�ȡ�
	float mPhi = 0.2f*XM_PI;
	//�뾶��
	float mRadius = 15.0f;

	POINT mLastMousePos;

	//�˽Ƕ�ָ����Ҫ�ƹ⡣
	float mKeyLightTheta = 1.5f*XM_PI;
	//�˽Ƕ�ָ����Ҫ�ƹ�
	float mKeyLightPhi = 0.2f*XM_PI;
	float mKeyLightRadius = 1.0f;


	//*******************************************��Ϸ���붨�岿��**************************************************************************************************************************************************
public:
	void UpdateScence(const GameTimer& gt);
	//*******************************************��Ϸ���붨�岿��**************************************************************************************************************************************************
};
