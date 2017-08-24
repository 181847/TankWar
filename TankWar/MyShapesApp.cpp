#include "MyShapesApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE preInstance, PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	try
	{
		MyShapesApp theApp(hInstance);
		if (!theApp.Initialize()) 
		{
			return 0;
		}

		return theApp.Run();
	}
	catch (DxException e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
	catch (SimpleException e)
	{
		char lineInfo[40];
		sprintf_s(lineInfo, "%d", e.line, 40);
		std::string exceptionMessage;
		exceptionMessage += "\n�ļ���\n";
		exceptionMessage += e.file;
		exceptionMessage += "\n������\n";
		exceptionMessage += lineInfo;
		exceptionMessage += "\n���ʽ��\n";
		exceptionMessage += e.expr;
		MessageBox(nullptr, AnsiToWString(exceptionMessage).c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}

MyShapesApp::MyShapesApp(HINSTANCE hInstance)
	:D3DApp(hInstance)
{
}


MyShapesApp::~MyShapesApp()
{
	if (md3dDevice != nullptr)
	{
		//�����ǿ���������GPU Command��
		FlushCommandQueue();
	}
}

bool MyShapesApp::Initialize()
{
	if (!D3DApp::Initialize())
	{
		return false;
	}

	//��Ϊ���ʱ��FrameResource�е�CommandAllocator��û�д�����ϣ�
	//��������ʹ��D3DApp��ԭ�ж����CommandAllocator����ɳ�ʼ���趨��
	//�������еĹ��������Allocator�����Ͼ�û��ʲô���ˡ�
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	//����Shader�������塣
	BuildRootSignature();
	//����Shader�Ķ�������ͱ���Shader��
	BuildShadersAndInputLayout();
	//�����ƹ⡣
	BuildLights();
	//�������ʡ�
	BuildMaterials();
	//��������Ҫ�õ������񶥵㡣
	BuildShapeGeometry();
	//����������Ⱦ����
	BuildRenderItems();
	//����FrameResource��
	BuildFrameResources();
	//����DescriptorHeap��
	BuildDescriptorHeap();
	//����Descriptor��View����
	BuildConstantsBufferView();
	//����PipelineState��
	BuildPSOs();

	//����������
	BuildScence();

	//ǿ��ִ�����Command��
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = {mCommandList.Get()};
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	FlushCommandQueue();

	return true;
}

void MyShapesApp::OnResize()
{
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);

	FlushCommandQueue();

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		mSwapChainBuffer[i].Reset();
	}
	mDepthStencilBuffer.Reset();

	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount, 
		mClientWidth, mClientHeight, 
		mBackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; ++i)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}

	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;

	D3D12_CLEAR_VALUE clearOpt;
	clearOpt.Format = mDepthStencilFormat;
	clearOpt.DepthStencil.Depth = 1.0f;
	clearOpt.DepthStencil.Stencil = 0;
	ThrowIfFailed(md3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&clearOpt,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
	dsViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsViewDesc.Format = mDepthStencilFormat;
	dsViewDesc.Texture2D.MipSlice = 0;
	md3dDevice->CreateDepthStencilView(
		mDepthStencilBuffer.Get(), 
		&dsViewDesc, 
		DepthStencilView());

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE));

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	FlushCommandQueue();

	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.MaxDepth = 1.0f;
	mScreenViewport.MinDepth = 0.1f;
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;

	mScissorRect = { 0, 0, mClientWidth, mClientHeight };

	//����ͶӰ����
	XMMATRIX newView = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, newView);
}

void MyShapesApp::Update(const GameTimer& gt)
{
	

	//���¼������롣
	m_pPlayerCommander->DetactKeyState();
	//�����������Ϣ��
	//UpdateCamera(gt);
	//�ӳ����е�0�������λ�ø����������ͷ��
	UpdateCameraFromScence(gt, 0);
	//���µƹ�λ�á�
	UpdateLights(gt);

	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResource[mCurrFrameResourceIndex].get();

	if ((mCurrFrameResource->Fence != 0) 
		&& 
		(mFence->GetCompletedValue() < mCurrFrameResource->Fence))
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateObjectCBs(gt);
	UpdateMainPassCB(gt);
	UpdateMaterialCB(gt);

	m_pPlayerCommander->Executee(gt);
	//m_AICommander->Executee();
	m_pBoneCommander->Update();
	//m_collideCommander->Executee();

	//������������������Ϸ�����е�ָ�
	m_pScence->UpdateData(gt, mCurrFrameResource);
	//ִ������pawn��ָ�ʹ��Scence������һ���������
	m_pPawnMaster->Executee();
}

void MyShapesApp::Draw(const GameTimer& gt)
{
	auto cmdListAlloc = mCurrFrameResource->CmdListAllocator;

	//����CommandAllocator�е��ڴ档
	ThrowIfFailed(cmdListAlloc->Reset());

	if (mIsWireframe)
	{
		ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque_wireframe"].Get() ));
	}
	else
	{
		ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));
	}

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	mCommandList->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(), 
			D3D12_RESOURCE_STATE_PRESENT, 
			D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->ClearRenderTargetView(
		CurrentBackBufferView(), 
		Colors::LightBlue, 
		0, nullptr);
	mCommandList->ClearDepthStencilView(
		DepthStencilView(), 
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 
		1.0f, 
		0, 
		0, nullptr);

	mCommandList->OMSetRenderTargets(
		1,							//��������
		&CurrentBackBufferView(),	//BackbufferView��
		true,						//��һ��ȾĿ�ꡣ
		&DepthStencilView());		//DepthStencilBufferView��

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	/*
	int passOffset =
	mPassCbvOffset + mCurrFrameResourceIndex;
	auto passHandle =
	CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
	passHandle.Offset(
	passOffset, mCbvSrvUavDescriptorSize);

	mCommandList->SetGraphicsRootDescriptorTable(
	1, passHandle);
	*/
	
	mCommandList->SetGraphicsRootConstantBufferView(2, mCurrFrameResource->PassCB->Resource()->GetGPUVirtualAddress());

	//�������еĿ���Ⱦ���塣
	this->DrawRenderItems(
		mCommandList.Get(), 
		mOpaqueRitems);//�ڲ����������п���Ⱦ�����ָ��

	mCommandList->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(), 
			D3D12_RESOURCE_STATE_RENDER_TARGET, 
			D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	mCurrFrameResource->Fence = ++mCurrentFence;
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void MyShapesApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	SetCapture(mhMainWnd);
}

void MyShapesApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MyShapesApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		//������¡�
		float dx = XMConvertToRadians(0.25f * static_cast<float>(mLastMousePos.x - x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(mLastMousePos.y - y));

		mPhi += dy;

		mTheta += dx;

		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		int dx = mLastMousePos.x - x;

		mRadius += 0.05f * static_cast<float>(dx);

		mRadius = MathHelper::Clamp(mRadius, 3.0f, 20.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;

	//��¼�����λ�á�
	m_pPlayerCommander->mouseState.UpdateLocation(static_cast<LONG>(x), static_cast<LONG>(y));
}

void MyShapesApp::OnKeyboardInput(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();

	if (GetAsyncKeyState('1') & 0x8000) 
	{
		mIsWireframe = true;
	}
	else 
	{
		mIsWireframe = false;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		mKeyLightTheta -= 1.0f*dt;

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		mKeyLightTheta += 1.0f*dt;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		mKeyLightPhi -= 1.0f*dt;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000) 
	{
		//Ϊ���Է���һ�����·������Ҳ�������겶��
		mKeyLightPhi += 1.0f*dt;
	}


	mKeyLightPhi = MathHelper::Clamp(mKeyLightPhi, 0.1f, XM_PIDIV2);
}

//��Ҫ�Ǹ���������ռ��ת������
//���������۲����
void MyShapesApp::UpdateCamera(const GameTimer& gt)
{
	//�ҵ�����������꣬
	//mPhi��ʾ����Ķ������ײ��Ļ��ȣ�0 ~ PI��
	//mTheta��ʾ����ĳ����X�Ὺʼ�Ļ��ȡ�
	mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
	mEyePos.y = mRadius * cosf(mPhi);

	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//�����������۲����
	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void MyShapesApp::UpdateLights(const GameTimer & gt)
{
	XMFLOAT3 keyDirection;
	XMFLOAT3 fillDirection;
	XMFLOAT3 backDirection;
	//��������ֵ�Ǿ���ԭ�����1��һ��λ��ֵ����һ����ԭ��ָ���ⲿ�ĵ�λ����������
	keyDirection.x = 
		-(
			backDirection.x = 1.0f * sinf(mKeyLightPhi) * cosf(mKeyLightTheta)
			);
	keyDirection.y =
		-(
			backDirection.y = 1.0f * cosf(mKeyLightPhi)
			);
	keyDirection.z =
		-(
			backDirection.z = 1.0f * sinf(mKeyLightPhi) * sinf(mKeyLightTheta)
			);

	//��ˮƽ�Ƕȼ���90�ȱ�ʾ����Ķ��λ�á�
	fillDirection.x = 1.0f * cosf(mKeyLightTheta - XM_PIDIV2);
	fillDirection.y = 0.0f;
	fillDirection.z = 1.0f * sinf(mKeyLightTheta - XM_PIDIV2);

	//ȡ��������ΪKeyLight�ķ���
	mLights["key"]->Direction = std::move(keyDirection);
	mLights["fill"]->Direction = std::move(fillDirection);
	//KeyLight�ķ��������BackLight�ķ���
	mLights["back"]->Direction = std::move(backDirection);
}

void MyShapesApp::UpdateObjectCBs(const GameTimer& gt)
{
	auto* currentCB = mCurrFrameResource->ObjectCB.get();

	for (auto& item : mAllRitems)
	{
		if (item->NumFramesDirty > 0) {

			//��ȡ�������ꡣ
			XMMATRIX world = XMLoadFloat4x4(&item->World);
			ObjectConstants objConstants;

			//�ǵ�HLSL�еľ������о���
			//��Direct12�е����о���
			//�м���Ҫ����һ��ת�á�
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			//���µ���Ӧ��ŵ�ConstantsBuffer��
			currentCB->CopyData(item->ObjCBIndex, objConstants);


			//ʣ����Ҫ���µ�Frame������
			item->NumFramesDirty--;
		}
	}
}

void MyShapesApp::UpdateMainPassCB(const GameTimer& gt)
{
	auto passCB = mCurrFrameResource->PassCB.get();

	mMainPassCB.DeltaTime = gt.DeltaTime();
	mMainPassCB.TotalTime = gt.TotalTime();

	mMainPassCB.EyePosW = mEyePos;

	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.NearZ = 1.0f;

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);
	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	
	mMainPassCB.RenderTargetSize =
		XMFLOAT2(static_cast<float>(mClientWidth), static_cast<float>(mClientHeight));

	mMainPassCB.lights[0] = *mLights["key"].get();
	mMainPassCB.lights[1] = *mLights["fill"].get();
	mMainPassCB.lights[2] = *mLights["back"].get();

	passCB->CopyData(0, mMainPassCB);
}

void MyShapesApp::UpdateMaterialCB(const GameTimer & gt)
{
	auto& currMaterialCB = mCurrFrameResource->MaterialCB;

	for (auto& pair : mMaterials)
	{
		Material* mat = pair.second.get();
		if (mat->NumFramesDirty > 0)
		{
			MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			matConstants.MatTransform = mat->MatTransform;

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);
		}
	}
}

void MyShapesApp::UpdateCameraFromScence(const GameTimer & gt, UINT cameraIndexInScence)
{
	MyCamera* pCurrentCamera = m_pScence->GetCamera(cameraIndexInScence);
	ASSERT(pCurrentCamera && "�޷���ȡ������������Ƿ�����������һ��PlayerPawn��");

	//ע�⣺��������������Ӧ�����������꣬��Ҫ�þֲ����꣬�ֲ�����������䣬�ᵼ���������ֹ��
	XMVECTOR pos = XMVectorSet(pCurrentCamera->Pos->World._41, pCurrentCamera->Pos->World._42, pCurrentCamera->Pos->World._43, 1.0f);
	XMVECTOR target = XMVectorSet(pCurrentCamera->Target->World._41, pCurrentCamera->Target->World._42, pCurrentCamera->Target->World._43, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	static XMFLOAT4 lastValue;
	static XMFLOAT4 currValue;
	lastValue = currValue;
	XMStoreFloat4(&currValue, target);
	static int sameCount = 0;
	if (lastValue.x == currValue.x 
		&& lastValue.y == currValue.y
		&& lastValue.z == currValue.z
		&& lastValue.w == currValue.w)
	{
		++sameCount;
	}

	//�����������۲����
	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void MyShapesApp::BuildDescriptorHeap()
{
	//PassConstantsDescriptor��ƫ������
	mPassCbvOffset = mAllRitems.size() * gNumFrameResources;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	//������Descriptor����������FrameResource��Ҫʹ�õ����е�ObjectConstants��PassConstants���ܺ͡�
	cbvHeapDesc.NumDescriptors = (UINT)(mPassCbvOffset + gNumFrameResources);
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap)));
}

void MyShapesApp::BuildConstantsBufferView()
{
	
	//��ȡ����ObjectConstantsBuffer�Ĵ�С��
	//ע�����������256����������
	UINT objCBSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT passCBSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
	//��Ⱦ�����������
	UINT allRitemCount = mAllRitems.size();

	auto pbHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
	pbHandle.Offset(mPassCbvOffset, mCbvSrvUavDescriptorSize);

	//��������ѭ��
	//��һ��gNumFrameResource�Σ�
	//		�ò�����ƫ�Ƶ�ָ��FrameResource��ObjectConstants��λ��
	//		����Ϊÿһ��FrameResource��PassConstants����view
	//�ڶ���ѭ����ÿһFrame�Ļ�����ѭ��allRitemCount�Σ�
	//		ΪÿһFrame�е�ÿ�������Buffer����view��
	for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
	{
		//���������View��
		//��ȡ��Դ���׵�ַ��
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = 
			mFrameResource[frameIndex]->ObjectCB->Resource()->GetGPUVirtualAddress();

		//��ȡDescriptor���׵�ַ��
		auto cbHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
		//ƫ�Ƶ���frameIndex��ָ��ĵ�һ��Descriptor��
		cbHandle.Offset(frameIndex * allRitemCount, mCbvSrvUavDescriptorSize);

		for (UINT bufferIndex = 0; bufferIndex < mAllRitems.size(); ++bufferIndex)
		{
			//GPU��Դ��ַ����ObjectConstants����С��ָ����һ��ObjectConstants��
			//ע�����ӵ�ֵ��256����������
			D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {cbAddress + bufferIndex * objCBSize, objCBSize};

			//����Object��View��
			md3dDevice->CreateConstantBufferView(&viewDesc, cbHandle);

			//descriptorƫ��һλ��
			cbHandle.Offset(mCbvSrvUavDescriptorSize);
		}

		//����Pass��View
		D3D12_GPU_VIRTUAL_ADDRESS pbAddress =
			mFrameResource[frameIndex]->PassCB->Resource()->GetGPUVirtualAddress();
		D3D12_CONSTANT_BUFFER_VIEW_DESC passViewDesc = 
		{ 
			pbAddress, 
			passCBSize
		};
		//����Pass��View
		md3dDevice->CreateConstantBufferView(&passViewDesc, pbHandle);

		//descriptorƫ��һλ��
		pbHandle.Offset(mCbvSrvUavDescriptorSize);
	}
	
		/*
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	UINT objCount = (UINT)mOpaqueRitems.size();

	// Need a CBV descriptor for each object for each frame resource.
	for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
	{
		auto objectCB = mFrameResource[frameIndex]->ObjectCB->Resource();
		for (UINT i = 0; i < objCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();

			// Offset to the ith object constant buffer in the buffer.
			cbAddress += i*objCBByteSize;

			// Offset to the object cbv in the descriptor heap.
			int heapIndex = frameIndex*objCount + i;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, mCbvSrvUavDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objCBByteSize;

			md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

	// Last three descriptors are the pass CBVs for each frame resource.
	for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
	{
		auto passCB = mFrameResource[frameIndex]->PassCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

		// Offset to the pass cbv in the descriptor heap.
		int heapIndex = mPassCbvOffset + frameIndex;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, mCbvSrvUavDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = passCBByteSize;

		md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
	}
	*/
}

void MyShapesApp::BuildRootSignature()
{
	CD3DX12_ROOT_PARAMETER parameter[3];

	//ObjectConstants
	parameter[0].InitAsConstantBufferView(0);
	//MaterialConstants
	parameter[1].InitAsConstantBufferView(1);
	//PassConstants
	parameter[2].InitAsConstantBufferView(2);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(parameter), parameter, 0, nullptr, 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	//���л���
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr) 
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);


	//ThrowIfFailed(md3dDevice->GetDeviceRemovedReason());

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0, 
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(), 
		IID_PPV_ARGS(mRootSignature.GetAddressOf())));


}

void MyShapesApp::BuildShadersAndInputLayout()
{
	mShaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["lightingPS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");

	mInputLayout = 
	{
		{
		"POSITION",		//����
		0,				//������š�Ĭ��Ϊ0
		DXGI_FORMAT_R32G32B32_FLOAT,	//�����ʽ
		0,				//inputSlot,�����������۸�Ϊ�˱�ģ�����Ҫ��CommandList����������Ե�ʱ��ı���ֵ��
		0,				//�������֣������ǵ�һ�����ԣ�������룬���ע�����ֵ�Ͳ����أ�����ı��˲�ۣ�����ֵҲҪ�ı䡣
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
		0
		},

		{
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
		}
	};
}

void MyShapesApp::BuildLights()
{
	auto keyLight = std::make_unique<Light>();
	keyLight->Strength = { 1.0f, 1.0f, 1.0f };

	auto fillLight = std::make_unique<Light>();
	keyLight->Strength = { 1.0f, 1.0f, 1.0f };

	auto backLight = std::make_unique<Light>();
	keyLight->Strength = { 1.0f, 1.0f, 1.0f };

	mLights["key"] = std::move(keyLight);
	mLights["fill"] = std::move(fillLight);
	mLights["back"] = std::move(backLight);
}

void MyShapesApp::BuildMaterials()
{

	auto boxMat = std::make_unique<Material>();
	boxMat->Name = "box";
	boxMat->MatCBIndex = 0;
	boxMat->DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	boxMat->FresnelR0 = HelpCalculateFresnelR0(8);
	boxMat->Roughness = 0.1f;

	auto gridMat = std::make_unique<Material>();
	gridMat->Name = "grid";
	gridMat->MatCBIndex = 1;
	gridMat->DiffuseAlbedo = { 0.5f, 0.5f, 1.0f, 1.0f };
	gridMat->FresnelR0 = HelpCalculateFresnelR0(2);
	gridMat->Roughness = 0.8f;

	auto sphereMat = std::make_unique<Material>();
	sphereMat->Name = "sphere";
	sphereMat->MatCBIndex = 2;
	sphereMat->DiffuseAlbedo = { 1.0f, 0.5f, 0.5f, 1.0f};
	sphereMat->FresnelR0 = HelpCalculateFresnelR0(50);
	sphereMat->Roughness = 0.15f;

	auto cylinderMat = std::make_unique<Material>();
	cylinderMat->Name = "cylinder";
	cylinderMat->MatCBIndex = 3;
	cylinderMat->DiffuseAlbedo = { 0.5f, 1.0f, 0.5f, 1.0f };
	cylinderMat->FresnelR0 = HelpCalculateFresnelR0(1);
	cylinderMat->Roughness = 0.3f;

	mMaterials[boxMat->Name] = std::move(boxMat);
	mMaterials[gridMat->Name] = std::move(gridMat);
	mMaterials[sphereMat->Name] = std::move(sphereMat);
	mMaterials[cylinderMat->Name] = std::move(cylinderMat);
}

void MyShapesApp::BuildShapeGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	//GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 2);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = (UINT)grid.Vertices.size() + gridVertexOffset;
	UINT cylinderVertexOffset = (UINT)sphere.Vertices.size() + sphereVertexOffset;

	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = (UINT)grid.Indices32.size() + gridIndexOffset;
	UINT cylinderIndexOffset = (UINT)sphere.Indices32.size() + sphereIndexOffset;

	SubmeshGeometry boxSubmesh;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();

	SubmeshGeometry gridSubmesh;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();

	//�ܵĶ���������
	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	//�ܵ�����������
	UINT totalIndexCount =
		box.Indices32.size() +
		grid.Indices32.size() +
		sphere.Indices32.size() +
		cylinder.Indices32.size();

	//���ж��㶨�塣
	std::vector<Vertex> totalVertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		totalVertices[k].Pos = box.Vertices[i].Position;
		totalVertices[k].Normal = box.Vertices[i].Normal;
		//Box����ɫ����Ϊ��ɫ��
		//totalVertices[k].Color = XMFLOAT4(DirectX::Colors::Black);
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		totalVertices[k].Pos = grid.Vertices[i].Position;
		totalVertices[k].Normal = grid.Vertices[i].Normal;
		//Grid����ɫ����Ϊ��ɫ��
		//totalVertices[k].Color = XMFLOAT4(DirectX::Colors::Brown);
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		totalVertices[k].Pos = sphere.Vertices[i].Position;
		totalVertices[k].Normal = sphere.Vertices[i].Normal;
		//Grid����ɫ����Ϊ��ɫ��
		//totalVertices[k].Color = XMFLOAT4(DirectX::Colors::Blue);
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		totalVertices[k].Pos = cylinder.Vertices[i].Position;
		totalVertices[k].Normal = cylinder.Vertices[i].Normal;
		//Grid����ɫ����Ϊ��ɫ��
		//totalVertices[k].Color = XMFLOAT4(DirectX::Colors::Red);
	}

	//�����������塣
	std::vector<uint16_t> totalIndices;
	//����������
	totalIndices.insert(totalIndices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	totalIndices.insert(totalIndices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	totalIndices.insert(totalIndices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	totalIndices.insert(totalIndices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	//���㻺����ܴ�С��
	const UINT vbByteSize = totalVertexCount * sizeof(Vertex);
	//����������ܴ�С��
	const UINT ibByteSize = totalIndexCount * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "shapeGeo";

	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexBufferByteSize = ibByteSize;
	geo->VertexByteStride = (UINT)sizeof(Vertex);
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));

	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), totalVertices.data(), vbByteSize);
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), totalIndices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(),
		totalVertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(),
		totalIndices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void MyShapesApp::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opDesc = { 0 };
	opDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	opDesc.pRootSignature = mRootSignature.Get();
	opDesc.VS = 
	{
		reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
		mShaders["standardVS"]->GetBufferSize()
	};

	opDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["lightingPS"]->GetBufferPointer()),
		mShaders["lightingPS"]->GetBufferSize()
	};

	opDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	opDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opDesc.SampleMask = UINT_MAX;
	opDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opDesc.NumRenderTargets = 1;
	opDesc.RTVFormats[0] = mBackBufferFormat;
	opDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	opDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	opDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&opDesc, IID_PPV_ARGS(&mPSOs["opaque"])));


	//�����й��߿���ʾ��PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opWireframeDesc = opDesc;
	opWireframeDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&opWireframeDesc, IID_PPV_ARGS(&mPSOs["opaque_wireframe"])));
}

void MyShapesApp::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i) 
	{
		mFrameResource.push_back(std::make_unique<FrameResource>(
			md3dDevice.Get(),		
			1,					//PassConstant������
			1000,
			//TODO
			//(UINT)mAllRitems.size(),//ObjectConstants��������
			(UINT)mMaterials.size()));
	}
}

void MyShapesApp::BuildRenderItems()
{
	MeshGeometry* geo = mGeometries["shapeGeo"].get();

	UINT objCBIndex = 0;

	auto boxRitem = std::make_unique<RenderItem>();
	boxRitem->Geo = geo;
	boxRitem->BaseVertexLocation = geo->DrawArgs["box"].BaseVertexLocation;
	boxRitem->IndexCount = geo->DrawArgs["box"].IndexCount;
	boxRitem->NumFramesDirty = gNumFrameResources;
	boxRitem->ObjCBIndex = objCBIndex++;
	boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->StartIndexLocation = geo->DrawArgs["box"].StartIndexLocation;
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0, 2.0, 2.0) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRitem->Mat = mMaterials["box"].get();
	mAllRitems.push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->Geo = geo;
	gridRitem->BaseVertexLocation = geo->DrawArgs["grid"].BaseVertexLocation;
	gridRitem->IndexCount = geo->DrawArgs["grid"].IndexCount;
	gridRitem->NumFramesDirty = gNumFrameResources;
	gridRitem->ObjCBIndex = objCBIndex++;
	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->StartIndexLocation = geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->Mat = mMaterials["grid"].get();
	mAllRitems.push_back(std::move(gridRitem));

	for (int i = 0; i < 5; ++i)
	{
		auto leftCylRitem = std::make_unique<RenderItem>();
		auto rightCylRitem = std::make_unique<RenderItem>();
		auto leftSphereRitem = std::make_unique<RenderItem>();
		auto rightSphereRitem = std::make_unique<RenderItem>();

		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f);

		XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
		leftCylRitem->ObjCBIndex = objCBIndex++;
		leftCylRitem->Geo = mGeometries["shapeGeo"].get();
		leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation; gridRitem;
		leftCylRitem->Mat = mMaterials["cylinder"].get();

		XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
		rightCylRitem->ObjCBIndex = objCBIndex++;
		rightCylRitem->Geo = mGeometries["shapeGeo"].get();
		rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;
		rightCylRitem->Mat = mMaterials["cylinder"].get();

		XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
		leftSphereRitem->ObjCBIndex = objCBIndex++;
		leftSphereRitem->Geo = mGeometries["shapeGeo"].get();
		leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;
		leftSphereRitem->Mat = mMaterials["sphere"].get();

		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		rightSphereRitem->ObjCBIndex = objCBIndex++;
		rightSphereRitem->Geo = mGeometries["shapeGeo"].get();
		rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;
		rightSphereRitem->Mat = mMaterials["sphere"].get();

		mAllRitems.push_back(std::move(leftCylRitem));
		mAllRitems.push_back(std::move(rightCylRitem));
		mAllRitems.push_back(std::move(leftSphereRitem));
		mAllRitems.push_back(std::move(rightSphereRitem));
	}

	for (auto& item : mAllRitems)
	{
		mOpaqueRitems.push_back(item.get());
	}
}

void MyShapesApp::BuildScence()
{
	m_pScence = std::make_unique<Scence>(
		totalRitemInScence, totalCameraInScence, &mMaterials, &mGeometries);

	//����PawnMaster�������Զ�������Pawn��
	BuildPawnMaster();
	//�������ָ��١�AIָ��١����Ƹ���ָ��١�
	BuildPlayerCommander();
	BuildAICommander();
	BuildBoneCommander();
	//BuildCollideCommander();

	//ע��Pawn��
	RegisterPawnClass();
	//������ʼ��pawn���󣬿��������ﴴ����ҽ�ɫ����ʼ��������
	//����ͨ��PawnMaster��������
	BuildInitPawn();

}

void MyShapesApp::BuildPawnMaster()
{
	m_pPawnMaster = std::make_unique<PawnMaster>(PAWN_MASTER_COMMAND_MAX_NUM, m_pScence.get());
}

void MyShapesApp::BuildPlayerCommander()
{
	m_pPlayerCommander = std::make_unique<PlayerCommander>(COMMANDER_PLAYER_MAX_COMMANDS);
}

void MyShapesApp::BuildAICommander()
{
	//m_AICommander = std::make_unique<AICommander>(COMMANDER_AI_UNIT_MAX_NUM);
}

void MyShapesApp::BuildBoneCommander()
{
	m_pBoneCommander = std::make_unique<BoneCommander>(COMMANDER_FOLLOW_MAX_COMMANDS);
}

//void MyShapesApp::BuildCollideCommander()
//{
//	m_collideCommander = std::make_unique<CollideCommander>(COMMANDER_COLLIDE_MAX_NUM);
//}

void MyShapesApp::RegisterPawnClass()
{
	//PlayerPawnע������Ҫ�õ���Master��Commander��
	PlayerPawn::RegisterAll(m_pPawnMaster.get(), m_pPlayerCommander.get(), m_pBoneCommander.get());

}

void MyShapesApp::BuildInitPawn()
{
	ASSERT(PlayerPawn::m_pawnType);
	//������ԡ�
	PlayerProperty* pPProperty;
	pPProperty = PlayerPawn::NewProperty();
	pPProperty->MoveSpeed = 10.0f;
	pPProperty->RotationSpeed = 10.0f;
	//��¼һ���������ָ�
	m_pPawnMaster->CreatePawn(PlayerPawn::m_pawnType, pPProperty);

	//Ϊ�˱�֤�����һ�����ڣ����ھ�ִ������pawn��ָ�ʹ��Scence������һ���������
	m_pPawnMaster->Executee();
}

void MyShapesApp::DrawRenderItems(ID3D12GraphicsCommandList * cmdList, const std::vector<RenderItem*>& ritems)
{
	
	cmdList->IASetVertexBuffers(0, 1, &ritems[0]->Geo->VertexBufferView());
	cmdList->IASetIndexBuffer(&ritems[0]->Geo->IndexBufferView());

	/*
	int itemHandleOffset = mCurrFrameResourceIndex * mAllRitems.size();
	auto heapHandle =
	CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
	//ƫ�Ƶ�ָ��Frame�����������Descriptor����
	heapHandle.Offset(itemHandleOffset, mCbvSrvUavDescriptorSize);
	*/
	UINT ObjectConstantsSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT MaterialConstantsSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
	

	for (auto& ritem : ritems)
	{
		/*
		cmdList->IASetPrimitiveTopology(ritem->PrimitiveType);
		auto thisHandle = heapHandle;
		thisHandle.Offset(ritem->ObjCBIndex, mCbvSrvUavDescriptorSize);
		//cmdList->SetGraphicsRootDescriptorTable(0, thisHandle);
		XMMATRIX world = XMLoadFloat4x4(&ritem->World);
		cmdList->SetGraphicsRoot32BitConstants(0, 16, &XMMatrixTranspose(world), (UINT)0);
		*/

		cmdList->IASetPrimitiveTopology(ritem->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objectBufferAddress = mCurrFrameResource->ObjectCB->Resource()->GetGPUVirtualAddress();
		objectBufferAddress += ritem->ObjCBIndex * ObjectConstantsSize;

		//����ObjectConstants��
		cmdList->SetGraphicsRootConstantBufferView(0, objectBufferAddress);
		//����MaterialConstants��
		cmdList->SetGraphicsRootConstantBufferView(1, mCurrFrameResource->MaterialCB->Resource()->GetGPUVirtualAddress() + ritem->Mat->MatCBIndex * MaterialConstantsSize);

		cmdList->DrawIndexedInstanced(ritem->IndexCount, 1, ritem->StartIndexLocation, ritem->BaseVertexLocation, 0);
	}

	/*
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();

	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		// Offset to the CBV in the descriptor heap for this object and for this frame resource.
		UINT cbvIndex = mCurrFrameResourceIndex*(UINT)mOpaqueRitems.size() + ri->ObjCBIndex;
		auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
		cbvHandle.Offset(cbvIndex, mCbvSrvUavDescriptorSize);

		cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}*/
}

XMFLOAT3 MyShapesApp::HelpCalculateFresnelR0(float refractionIndex)
{
	float R0_X_or_Y_or_Z = (refractionIndex - 1) / (refractionIndex + 1);
	return XMFLOAT3(R0_X_or_Y_or_Z, R0_X_or_Y_or_Z, R0_X_or_Y_or_Z);
}


