#pragma once

class Game
{
public:
	Game();
	~Game();

	void BeginPlay(HWND hwnd);
	void Tick();
	void Render();

private:
	void RenderBegin();
	void RenderEnd();

	// device와 SwapChain 생성
	void CreateDeviceAndSwapChain();
	// RenderTargetView(RTV) 생성
	void CreateRenderTargetView();
	// Viewport 설정
	void SetViewport();


	// 기하학적 요소(도형, 정점) 생성
	void CreateGeometry();
	// vertexBuffer의 멤버와 셰이더에서 VS_INPUT(구조체) 멤버와 연결 및 설명, IA단계에서 사용됨
	// InputLayout : vertexBuffer가 담고 있는 데이터가 어떤 데이터인지, 
	// 셰이더에 어떻게 넘길 것인지 같은 정보를 저장해 두는 것
	void CreateInputLayout();


	// 셰이더 불러오기
	// wstring : wchar형태(영어를 제외한 문자를 사용하기 윈한 2바이트)
	void LoadShaderFromFile(const wstring& path, const string& name, 
		const string& version, ComPtr<ID3DBlob>& blob);
	// 불러온 셰이더를 GPU에게 전달하기
	void CreateVS();
	void CreatePS();
	
	// pixelShader 단계에서 사용할 수 있는 View
	// Shader Resource View
	void CreateSRV();

private:
	HWND			_hwnd;
	uint32			_width = 0;
	uint32			_height = 0;

private:
	/**
		DirectX 11
	*/
	// Com객체 & ComPtr : https://pub-repository.tistory.com/98 혹은 Reference폴더 DX11 Com & ComPtr 참조
	// I : Interface
	// ID3D11Device : Com(Component Object Model)객체
	// Com객체 : MS에서 만든 여러 GPU종류에 상관하지 않고 하드웨어적 연결되도록 하는 객체
	// ComPtr : Com객체를 관리하는 스마트 포인터 
	//	참조(AddRef)하면 자동으로 참조 횟수를 늘림
	//	어디선가 소멸(Release)을 하면 자동으로 참조 횟수를 줄임
	// ComPtr로 되어 있으면 GPU에게 무언가를 요청하는 것이고 GPU내에서 실질적 객체가 생성됨.
	
	// Device
	// device & deviceContext : https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/overviews-direct3d-11-devices-intro
	// Device & DeviceContext & DXGI : https://myoung-min.tistory.com/4 or https://dev-nicitis.tistory.com/46
	// _device : 각종 리소스를 생성과 그래픽스 파이프 라인을 구성하고 셰이더를 생성하기 위한 용도
	// 디바이스는 리소스를 만들고 디스플레이 어댑터(GPU)의 기능을 열거하는 데 사용됩니다.
	ComPtr<ID3D11Device>				_device;
	// _deviceContext : device가 사용되는 환경이나 설정을 함. 리소스와 셰이더를 파이프라인에 연결하고 렌더링을 수행
	// 디바이스 컨텍스트는 파이프라인 상태를 설정하고 디바이스가 소유한 리소스를 사용하여 렌더링 명령을 생성하는 데 사용됩니다
	ComPtr<ID3D11DeviceContext>			_deviceContext;

	// SwapChain
	// DXGI(DirectX Graphics Infrastructure) : DX10부터 사용
	// - 런타임에 독립적인 저수준의 작업(하드웨어 장치를 열거나 화면을 출력)을 관리하며
	// - DirectX 그래픽을 위한 프레임 워크 제공
	// DXGI의 목적 : 커널모드 드라이버 및 시스템 하드웨어와 통신하는 것
	// 
	// Double-Buffer 작동 2가지
	// 1. 후면 Buffer에서 연산이 되고 연산 완료되면 전면 Buffer에 고속 복사해서 출력 (해당 코드 방식)
	// 2. 후면 Buffer에서 연산하고 전면 Buffer와 교환하여 2개의 Buffer를 돌아가면서 연산&출력
	ComPtr<IDXGISwapChain>				_swapChain = nullptr;

	// RenderTargetView
	// View : 어떠한 리소스에 대해서 설명서를 붙여서 GPU에게 설명해줌
	// RTV : 후면 Buffer를 설명하기 위한 View
	ComPtr<ID3D11RenderTargetView>		_renderTargetView;
	// ShaderResourceView
	// SRV : PixelShader 단계에서 사용되는 View
	ComPtr<ID3D11ShaderResourceView>	_shaderResourceView;
	ComPtr<ID3D11ShaderResourceView>	_shaderResourceView2;


	// Mics
	// 화면를 설명하는 구조체(크기, 등)
	D3D11_VIEWPORT						_viewport = {0};
	// 초기 색상
	float _clearColor[4] = { 0.f, 0.f, 0.f, 0.f };



	/**
		Geometry
	*/
	// 정점의 정보를 담은 배열
	vector<Vertex>						_vertices;
	// CPU가 가진 정보를 GPU에게 전달해야함
	// GPU가 정보를 읽기 위한 VRAM에 저장될 데이터를 가짐.
	ComPtr<ID3D11Buffer>				_vertexBuffer = nullptr;
	// _vertexBuffer를 통해 전달되는 _vertices의 구조를 설명함
	ComPtr<ID3D11InputLayout>			_inputLayout = nullptr;
	// IndexBuffer에서 사용할 데이터
	vector<uint32>						_indices;
	// IndexBuffer
	ComPtr<ID3D11Buffer>				_indexBuffer = nullptr;



	/**
		Shader
	*/
	// Vertex Shader
	ComPtr<ID3D11VertexShader>			_vertexShader = nullptr;
	// Blob : 셰이더 파일 정보를 가져옴
	ComPtr<ID3DBlob>					_vsBlob = nullptr;

	// Pixel Shader
	ComPtr<ID3D11PixelShader>			_pixelShader = nullptr;
	// Blob : 셰이더 파일 정보를 가져옴
	ComPtr<ID3DBlob>					_psBlob = nullptr;
};

