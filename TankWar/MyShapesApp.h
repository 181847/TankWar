#pragma once
#include "../../../../Common/d3dApp.h"
#include "../../../../Common/MathHelper.h"
#include "../../../../Common/UploadBuffer.h"
#include "../../../../Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "Scence.h"
#include "PawnMaster.h"
#include "PlayerCommander.h"
#include "BoneCommander.h"
#include "PlayerPawn.h"
#include "ObjReader.h"

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

	//�ӳ�����ָ����ŵ�����ͷ�����������
	void UpdateCameraFromScence(const GameTimer& gt, UINT cameraIndexInScence);


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


	//�����������Ⱦ�����������
	UINT totalRitemInScence = 5000;
	//�������������ͷ��������
	UINT totalCameraInScence = 3;

//PawnMasterһ������ܹ���ס���ٸ����ɻ���ɾ��Pawn��ָ�
#define PAWN_MASTER_COMMAND_MAX_NUM 200
//PlayerCommanderһ������ܹ���ס��Player������
#define COMMANDER_PLAYER_MAX_COMMANDS 3
//FollowCommanderһ������ܹ���¼�ĸ���ָ��������
#define COMMANDER_FOLLOW_MAX_COMMANDS 1000
//AICommander���ɿ��ƶ��ٸ�AI����
#define COMMANDER_AI_UNIT_MAX_NUM 100
//����ж��ٸ���ײ�塣
#define COMMANDER_COLLIDE_MAX_NUM 2000

public:
	//������������
	void BuildScence();
	//����PawnMaster�������Զ�������Pawn��
	void BuildPawnMaster();
	//�������ָ��١�AIָ��١����Ƹ���ָ��١�
	void BuildPlayerCommander();
	void BuildAICommander();
	void BuildBoneCommander();
	//����Scence��������Ҫ�õ���������Щ����ȫ����Obj�ļ��ж�ȡ��
	//һ��Obj�ļ���Ϊһ��������MeshGeometry��
	//һ��Obj�ļ��е����ļ�������Ϊ���Ӧ��һ��SubMesh��
	void BuildShapeGeometry_for_Scence();
	//void BuildCollideCommander();

	//ע��Pawn�ൽ������Ҫ��PawnMaster��Commander�С�
	void RegisterPawnClass();
	//������ʼ��pawn���󣬿��������ﴴ����ҽ�ɫ����ʼ��������
	//����ͨ��PawnMaster��������
	void BuildInitPawn();
	//��ָ�����ļ�����Ӽ�����ϼ���
	//Ĭ���ļ���Ϊ��.obj����Ĭ���ļ�·������Ŀ��Դ�ġ�objs���ļ���֮�£�
	//���Բ���ֻ��Ҫ�����ļ����ֵ�ǰһ���־Ϳ����ˣ�
	//��ӵ�Geometry���ֽ��ᱻ�趨Ϊ���fileName��
	void AddGeometry(const string& fileName);


public:
	//��������
	std::unique_ptr<Scence> m_pScence;
	//PawnMaster
	std::unique_ptr<PawnMaster> m_pPawnMaster;
	
	std::unique_ptr<PlayerCommander> m_pPlayerCommander;
	//std::unique_ptr<AICommander> m_AICommander;
	std::unique_ptr<BoneCommander> m_pBoneCommander;
	//std::unique_ptr<CollideCommander> m_collideCommander;
	//*******************************************��Ϸ���붨�岿��**************************************************************************************************************************************************
};
