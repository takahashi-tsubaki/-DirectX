#include <Windows.h>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <wrl.h>

using namespace DirectX;

const float PI = 3.141592f;

//�����N�̐ݒ�
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")//directInput�̃o�[�W����
#pragma comment(lib,"dxguid.lib")

//�֐��̃v���g�^�C�v�錾
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#pragma region ���͊֐�
//�L�[�̏����������֐�
void keyInitialize(BYTE* key, BYTE* oldkey, int array);
//�L�[��������Ă鎞
bool pushKey(BYTE key, int keyNum);
//�L�[��������ĂȂ���
bool notPushKey(BYTE* key, int keyNum);
//�L�[����������
bool pressKey(BYTE* key, BYTE* oldkey, int keyNum);
//�L�[���������Ă鎞
bool triggerKey(BYTE* key, BYTE* oldkey, int keyNum);
//�L�[�𗣂�����
bool releaseKey(BYTE* key, BYTE* oldkey, int keyNum);
#pragma endregion ���͊֐�


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	OutputDebugStringA("Hello DirectX!!\n");

	const int window_width = 960;
	const int window_height = 640;

	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;//�R�[���o�b�N�֐��̎w��
	w.lpszClassName = _T("DirectXTest");//�A�v���P�[�V�����N���X��(�K���ł����ł�)
	w.hInstance = GetModuleHandle(0);//�n���h���̎擾
	w.hCursor = LoadCursor(NULL, IDC_ARROW);


	RegisterClassEx(&w);//�A�v���P�[�V�����N���X(���������̍�邩���낵������OS�ɗ\������)

	RECT wrc = { 0,0,window_width, window_height };//{0,0,window_width,window_height}

	//�֐����g���ăE�B���h�E�̃T�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(w.lpszClassName,//�N���X���w��
		_T("DX12Test"),//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,//�^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,//�\��x���W��OS�ɂ��C��
		CW_USEDEFAULT,//�\��y���W��OS�ɂ��C��
		wrc.right - wrc.left,//�E�B���h�E��
		wrc.bottom - wrc.top,//�E�B���h�E��
		nullptr,//�e�E�B���h�E�n���h��
		nullptr,//���j���[�n���h��
		w.hInstance,//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);//�ǉ��p�����[�^�[

	ShowWindow(hwnd, SW_SHOW);

	MSG msg{};

	//��������DirectX����������
#ifdef _DEBUG
	//�f�o�b�N���C���[���I����
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	HRESULT result;
	//�󂯎M�ƂȂ�ϐ�
	ID3D12Device* dev = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeap = nullptr;


	//DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));

	//�A�_�v�^�[�̗񋓗p
	std::vector<IDXGIAdapter4*> adapters;

	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter4* tmpAdapter = nullptr;

	//�p�t�H�[�}���X�̍���������A�S�ẴA�_�v�^�[��񋓂���
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&tmpAdapter))
		!= DXGI_ERROR_NOT_FOUND; i++)
	{
		//���I�z��ɒǉ�����
		adapters.push_back(tmpAdapter);
	}

	//�Ó��ȃA�_�v�^�[��I�ʂ���
	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		//�A�_�v�^�[�̏����擾����
		adapters[i]->GetDesc3(&adapterDesc);

		//�\�t�g�E�F�A�f�o�C�X�����
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//�f�o�C�X���̗p���ă��[�v�𔲂���
			tmpAdapter = adapters[i];
			break;
		}
	}


	//�Ή����x���̎擾
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;
	for (size_t i = 0; i < _countof(levels); i++)
	{
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			//�f�o�C�X�𐶐��������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}

	//�R�}���h�A���P�[�^�𐶐�
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
	assert(SUCCEEDED(result));

	//�R�}���h���X�g�𐶐�
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator, nullptr, IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));

	//�R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//�R�}���h�L���[�𐶐�
	result = dev->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));

	//�X���b�v�`�F�[���̐ݒ�
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = window_width;
	swapChainDesc.Height = window_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//�F���̏���
	swapChainDesc.SampleDesc.Count = 1;//�}���`�T���v�����Ȃ�
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;//�o�b�N�o�b�t�@�p
	swapChainDesc.BufferCount = 2;//�o�b�t�@����2�ɐݒ�
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//�t���b�v��͔j��
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//
	//�X���b�v�`�F�[���̐���
	result = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&swapChain);
	assert(SUCCEEDED(result));

	//�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//�����_�[�^�[�Q�b�g�r���[
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount;//���\��2��

	//�f�X�N���v�^�q�[�v�̐���
	dev->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));//��Ƀf�X�N���v�^�q�[�v�����

	//�o�b�N�o�b�t�@
	std::vector<ID3D12Resource*>backBuffers;
	backBuffers.resize(swapChainDesc.BufferCount);//�X���b�v�`�F�[�����ɐ������ꂽ�o�b�N�o�b�t�@�̃A�h���X�����Ă������߂̃|�C���^��p�ӂ���

	//�X���b�v�`�F�[���̑S�Ẵo�b�t�@�ɂ��ď�������
	for (size_t i = 0; i < backBuffers.size(); i++)
	{
		//�X���b�v�`�F�[������o�b�t�@���擾
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		//�f�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		//�\�������ŃA�h���X�������
		rtvHandle.ptr += i * dev->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//�����_�[�^�[�Q�b�g�r���[�̐ݒ�
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//�V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//�����_�[�^�[�Q�b�g�r���[�̐���
		dev->CreateRenderTargetView(backBuffers[i], &rtvDesc, rtvHandle);
	}

	//�t�F���X�̐���
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//DirectInput�̏�����
	IDirectInput8* directInput = nullptr;
	result = DirectInput8Create(w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//�L�[�{�[�h�f�o�C�X�̐���
	IDirectInputDevice8* keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
	//DISCL_FOREGROUND   ��ʂ���O�ɂ���ꍇ�̂ݓ��͂��󂯕t����
	//DISCL_NONEXCLUSIVE �f�o�C�X�����̃A�v�������Ő�L���Ȃ�
	//DISCL_NOWINKEY     Windows�L�[�𖳌��ɂ���



	//�����܂�DirectX����������


	//��������I�u�W�F�N�g�̕`�揉��������
	//���_�f�[�^
	XMFLOAT3 vertices[] =
	{
		// x     y    z    //���W
		{-0.5f,+0.5f,0.0f},//����
		{+0.5f,+0.5f,0.0f},//�E��
		{-0.5f,-0.5f,0.0f},//����
		{+0.5f,-0.5f,0.0f},//�E��
	};

	uint16_t indices[] =
	{
		0,1,2,
		1,2,3,
	};



	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;//���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̍쐬
	ID3D12Resource* vertBuff = nullptr;
	result = dev->CreateCommittedResource(
		&heapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];//���W���R�s�[
	}

	//�q���������
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�[�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	//���_1���̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	ID3DBlob* vsBlob = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr;//�G���[�I�u�W�F�N�g

	//�萔�o�b�t�@
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;//�F
	};

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* constBuffMaterial = nullptr;
	//�萔�o�b�t�@�̐���
	result = dev->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,

		&cbResourceDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
	assert(SUCCEEDED(result));

	//�l���������ނƎ����I�ɓ]�������
	constMapMaterial->color = XMFLOAT4(1, 0, 0, 0.5f);

	//���[�g�p�����[�^
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParam.Descriptor.ShaderRegister = 0;//�萔�o�b�t�@�ԍ�
	rootParam.Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����

	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t)) * _countof(indices);

	//���\�[�X�ݒ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�C���f�b�N�X�o�b�t�@�̐���
	ID3D12Resource* indexBuff = nullptr;
	result = dev->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//�S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];
	}
	//�}�b�s���O����
	indexBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�̐���
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��(���_�V�F�[�_�͒��_�̍��W�ϊ�)
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",//�V�F�[�_�[�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h���\�ɂ���
		"main",//�G���g���[�|�C���g
		"vs_5_0",//�V�F�[�_���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
		0,
		&vsBlob, &errorBlob);

	////�R���p�C���G���[�Ȃ�
	//if (FAILED(result))
	//{
	//	//errorBlob����G���[���e��stirng�^�ɃR�s�[
	//	std::string error;
	//	error.resize(errorBlob->GetBufferSize());

	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferPointer(),
	//		error.begin());
	//	error += "\n";
	//	//�G���[���e���o�̓E�B���h�E�ɕ\��
	//	OutputDebugStringA(error.c_str());
	//	assert(0);
	//}

	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��(�s�N�Z���̖����͕`��F�̐ݒ�)
	result = D3DCompileFromFile(
		L"BasicPS.hlsl",//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

	////�R���p�C���G���[�Ȃ�
	//if (FAILED(result))
	//{
	//	//errorBlob����G���[���e��stirng�^�ɃR�s�[
	//	std::string error;
	//	error.resize(errorBlob->GetBufferSize());

	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferPointer(),
	//		error.begin());
	//	error += "\n";
	//	//�G���[���e���o�̓E�B���h�E�ɕ\��
	//	OutputDebugStringA(error.c_str());
	//	assert(0);
	//}


	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
		},
	};

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//�V�F�[�_�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//�T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�W���ݒ�

	//���X�^���C�U�̐ݒ�(���_�̃s�N�Z����)
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//�|���S�����h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true;//�[�x�N���b�s���O��L����

	//�u�����h�X�e�[�g
	/*pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;*///RGBA�S�Ẵ`�����l����`��
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//���ʐݒ�
	//blenddesc.BlendEnable    = true;//�u�����h��L���ɂ���
	//blenddesc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;//���Z
	//blenddesc.SrcBlendAlpha  = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//�\�[�X�̒l��0%�g��

	////	���Z����
	//blenddesc.BlendOp   = D3D12_BLEND_OP_ADD;//���Z
	//blenddesc.SrcBlend  = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��

	////���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//�e�X�g����\�[�X�����Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��

	////�F���]
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//���Z
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-�f�X�g�J���[�̒l
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;//�g��Ȃ�

	//������
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//���Z
	//blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//�\�[�X�̃A���t�@�l
	//blenddesc.DestBlend = D3D12_BLEND_SRC_ALPHA;//1.0f-�\�[�X�̃A���t�@�l

	//���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//�}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1;//�`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1;//1�s�N�Z���ɂ�1�񃌃��_�����O

	//���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature;

	//���[�g�V�O�l�`���̐ݒ�(����)
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootParam;
	rootSignatureDesc.NumParameters = 1;

	//���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature;

	//�p�C�v���C���X�e�[�g�̐���
	ID3D12PipelineState* pipelineState = nullptr;
	result = dev->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	//�����܂ŕ`�揉��������

	FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };//���ۂ��F

	BYTE key[256] = {};
	BYTE oldkey[256] = {};

	//�Q�[�����[�v
	while (true)
	{
		//�E�B���h�E���b�Z�[�W����
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		if (msg.message == WM_QUIT)
		{
			break;
		}

		//��������DirectX���t���[������

		//�L�[�{�[�h���̎擾�J�n
		keyboard->Acquire();
		keyInitialize(key, oldkey, sizeof(key) / sizeof(key[0]));
		//
		keyboard->GetDeviceState(sizeof(key), key);

		int angle = 0;
		if (pressKey(key, oldkey, DIK_Z))
		{
			angle = 180;
		}
		else if (pressKey(key, oldkey, DIK_C))
		{
			angle = -180;
		}
		float cosin = cos(PI / angle);
		float sain = sin(PI / angle);

		float transX = 0.0f;
		if (pressKey(key, oldkey, DIK_D))
		{
			transX = 0.01f;
		}
		else if (pressKey(key, oldkey, DIK_A))
		{
			transX = -0.01f;
		}
		float transY = 0.0f;
		if (pressKey(key, oldkey, DIK_W))
		{
			transY = 0.01f;
		}
		else if (pressKey(key, oldkey, DIK_S))
		{
			transY = -0.01f;
		}
		float transZ = 0.0f;

		float afinRotaZ[4][4] =
		{
		  {cosin,sain,0.0f,0.0f},//x=cos��-ysin��
		  {-sain,cosin,0.0f,0.0f},//y=xsin��+ycos��
		  {0.0f,0.0f,1.0f,0.0f},//z=z
		  {0.0f,0.0f,0.0f,1.0f},//
		};

		float afinScale[4][4] =
		{
			{1.01f,0.0f,0.0f,0.0f},//x��
			{0.0f,1.01f,0.0f,0.0f},//y��
			{0.0f,0.0f,1.01f,0.0f},//z��
			{0.0f,0.0f,0.0f,1.0f},//�H
		};

		float afinShrink[4][4] =
		{
			{0.99f,0.0f,0.0f,0.0f},//x��
			{0.0f,0.99f,0.0f,0.0f},//y��
			{0.0f,0.0f,0.99f,0.0f},//z��
			{0.0f,0.0f,0.0f,1.0f},//�H
		};

		float afinTranslation[4][4] =
		{
		  {1.0f, 0.0f, 0.0f, transX},//Tx
		  {0.0f, 1.0f, 0.0f, transY},//Ty
		  {0.0f, 0.0f, 1.0f, transZ},//Tz
		  {0.0f, 0.0f, 0.0f, 1.0f},//1
		};

		if (pressKey(key, oldkey, DIK_Z) || pressKey(key, oldkey, DIK_C))
		{

			for (int i = 0; i < 4/* _countof(vertices)*/; i++)
			{
				vertices[i].x = vertices[i].x * afinRotaZ[0][0] + vertices[i].y * afinRotaZ[0][1] +
					vertices[i].z * afinRotaZ[0][2] + 1 * afinRotaZ[0][3];
				vertices[i].y = vertices[i].x * afinRotaZ[1][0] + vertices[i].y * afinRotaZ[1][1] +
					vertices[i].z * afinRotaZ[1][2] + 1 * afinRotaZ[1][3];
				vertices[i].z = vertices[i].x * afinRotaZ[2][0] + vertices[i].y * afinRotaZ[2][1] +
					vertices[i].z * afinRotaZ[2][2] + 1 * afinRotaZ[2][3];
			}
		}
		if (pressKey(key, oldkey, DIK_2))
		{
			for (int i = 0; i < 4/* _countof(vertices)*/; i++)
			{
				vertices[i].x = vertices[i].x * afinScale[0][0] + vertices[i].y * afinScale[0][1] +
					vertices[i].z * afinScale[0][2] + 1 * afinScale[0][3];
				vertices[i].y = vertices[i].x * afinScale[1][0] + vertices[i].y * afinScale[1][1] +
					vertices[i].z * afinScale[1][2] + 1 * afinScale[1][3];
				vertices[i].z = vertices[i].x * afinScale[2][0] + vertices[i].y * afinScale[2][1] +
					vertices[i].z * afinScale[2][2] + 1 * afinScale[2][3];
			}

		}
		if (pressKey(key, oldkey, DIK_3))
		{
			for (int i = 0; i < 4/* _countof(vertices)*/; i++)
			{
				vertices[i].x = vertices[i].x * afinShrink[0][0] + vertices[i].y * afinShrink[0][1] +
					vertices[i].z * afinShrink[0][2] + 1 * afinShrink[0][3];
				vertices[i].y = vertices[i].x * afinShrink[1][0] + vertices[i].y * afinShrink[1][1] +
					vertices[i].z * afinShrink[1][2] + 1 * afinShrink[1][3];
				vertices[i].z = vertices[i].x * afinShrink[2][0] + vertices[i].y * afinShrink[2][1] +
					vertices[i].z * afinShrink[2][2] + 1 * afinShrink[2][3];
			}

		}
		if (pressKey(key, oldkey, DIK_W) || pressKey(key, oldkey, DIK_S)
			|| pressKey(key, oldkey, DIK_A) || pressKey(key, oldkey, DIK_D))
		{
			for (int i = 0; i < 4/* _countof(vertices)*/; i++)
			{
				vertices[i].x = vertices[i].x * afinTranslation[0][0] + vertices[i].y * afinTranslation[0][1] +
					vertices[i].z * afinTranslation[0][2] + 1 * afinTranslation[0][3];
				vertices[i].y = vertices[i].x * afinTranslation[1][0] + vertices[i].y * afinTranslation[1][1] +
					vertices[i].z * afinTranslation[1][2] + 1 * afinTranslation[1][3];
				vertices[i].z = vertices[i].x * afinTranslation[2][0] + vertices[i].y * afinTranslation[2][1] +
					vertices[i].z * afinTranslation[2][2] + 1 * afinTranslation[2][3];
			}

		}

		for (int i = 0; i < _countof(vertices); i++)
		{
			vertMap[i] = vertices[i];//���W���R�s�[
		}


		//�o�b�N�o�b�t�@�̔ԍ�������
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

		// �P�D���\�[�X�o���A�ŏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbIndex];//�o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;//�\����Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;//�`���Ԃ�
		commandList->ResourceBarrier(1, &barrierDesc);

		//�@�Q�D�`���̕ύX
		//�����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		// �R�D��ʃN���A          R     G     B     A(alpha)

		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		//�@�S�D��������`��R�}���h
		//�r���[�|�[�g�̐ݒ�R�}���h
		D3D12_VIEWPORT viewport[4]{};

		viewport[0].Width = window_width;
		viewport[0].Height = window_height;
		viewport[0].TopLeftX = 0;
		viewport[0].TopLeftY = 0;
		viewport[0].MinDepth = 0.0f;
		viewport[0].MaxDepth = 1.0f;

		/*viewport[1].Width = window_width;
		viewport[1].Height = window_height;
		viewport[1].TopLeftX = -300;
		viewport[1].TopLeftY = 200;
		viewport[1].MinDepth = 0.0f;
		viewport[1].MaxDepth = 1.0f;

		viewport[2].Width = window_width;
		viewport[2].Height = window_height;
		viewport[2].TopLeftX = 200;
		viewport[2].TopLeftY = -100;
		viewport[2].MinDepth = 0.0f;
		viewport[2].MaxDepth = 1.0f;

		viewport[3].Width = window_width;
		viewport[3].Height = window_height / 2;
		viewport[3].TopLeftX = 200;
		viewport[3].TopLeftY = 400;
		viewport[3].MinDepth = 0.0f;
		viewport[3].MaxDepth = 1.0f;*/


		//�r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		commandList->RSSetViewports(1, &viewport[0]);

		//�V�U�[��`
		D3D12_RECT scissorRect{};
		scissorRect.left = 0;//�؂蔲�����W��
		scissorRect.right = scissorRect.left + window_width;//�؂蔲�����W�E
		scissorRect.top = 0;//�؂蔲�����W��
		scissorRect.bottom = scissorRect.top + window_height;//�؂蔲�����W��

		//�V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		commandList->RSSetScissorRects(1, &scissorRect);

		//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h

		commandList->SetPipelineState(pipelineState);


		commandList->SetGraphicsRootSignature(rootSignature);

		//�v���~�e�B�u�`��̐ݒ�R�}���h
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		/*commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);*/

		//���_�o�b�t�@�[�r���[�̐ݒ�R�}���h
		commandList->IASetVertexBuffers(0, 1, &vbView);

		//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
		commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
		commandList->IASetIndexBuffer(&ibView);

		//�`��R�}���h

		commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);//�S�Ă̒��_���g���ĕ`��
		//�r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		//commandList->RSSetViewports(1, &viewport[1]);
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);//�S�Ă̒��_���g���ĕ`��

		//commandList->RSSetViewports(1, &viewport[2]);
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);//�S�Ă̒��_���g���ĕ`��

		//commandList->RSSetViewports(1, &viewport[3]);
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);//�S�Ă̒��_���g���ĕ`��
		//�@�S�D�����܂ŕ`��R�}���h

		//�@�T�D���\�[�X�o���A��߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//�`���Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;//�\����Ԃ�
		commandList->ResourceBarrier(1, &barrierDesc);

		//���߂̃N���[�Y
		result = commandList->Close();
		assert(SUCCEEDED(result));

		//�R�}���h���X�g�̎��s
		ID3D12CommandList* commandLists[] = { commandList };
		commandQueue->ExecuteCommandLists(1, commandLists);

		//��ʂɕ\������o�b�t�@���t���b�v (���\�̓���ւ�)
		result = swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		//�R�}���h�̎��s������҂�
		commandQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		//�L���[���N���A
		result = cmdAllocator->Reset();
		assert(SUCCEEDED(result));
		//�ĂуR�}���h���X�g�����߂鏀��
		result = commandList->Reset(cmdAllocator, nullptr);
		assert(SUCCEEDED(result));

		//�����܂�DirectX���t���[������
	}
	UnregisterClass(w.lpszClassName, w.hInstance);


	return 0;
}

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
		//�E�B���h�E���j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂���,�A�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}

	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void keyInitialize(BYTE* key, BYTE* oldkey, int array)
{
	for (int i = 0; i < array; i++)
	{
		oldkey[i] = key[i];
	}
};
bool pushKey(BYTE* key, int keyNum)
{
	if (key[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

bool notPushKey(BYTE* key, int keyNum)
{
	if (!key[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}

};

bool pressKey(BYTE* key, BYTE* oldkey, int keyNum)
{
	if (key[keyNum] && oldkey[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

bool triggerKey(BYTE* key, BYTE* oldkey, int keyNum)
{
	if (key[keyNum] && !oldkey[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

bool releaseKey(BYTE* key, BYTE* oldkey, int keyNum)
{
	if (!key[keyNum] && oldkey[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};
