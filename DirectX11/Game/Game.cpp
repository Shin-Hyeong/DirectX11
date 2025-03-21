#include "pch.h"
#include "Game.h"

Game::Game()
{

}

Game::~Game()
{

}

// Client.cpp에서 한번 실행
void Game::BeginPlay(HWND hwnd)
{
	_hwnd = hwnd;
	_width = GWinSizeX;
	_height = GWinSizeY;

	// device와 SwapChain 생성
	CreateDeviceAndSwapChain();
	// RTV 생성
	CreateRenderTargetView();
	// Viewport 설정
	SetViewport();

	// 도형 생성(그리기)
	CreateGeometry();
	// VertexShader 생성
	CreateVS();
	// VertexShader에 대한 설명
	CreateInputLayout();
	// PixelShader 생성
	CreatePS();
}

void Game::Tick()
{
	
}

void Game::Render()
{
	// Render 준비 작업
	RenderBegin();

	// 렌더링 파이프라인에 연결하기
	// IA - VS - RS - PS - OM
	{
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;
		// IA(코딩 불가) : 사용할 값들을 설정
		// 사용할 VertexBuffer(정점에 대한 정보) 설정
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(),&stride, &offset);
		// VertexBuffer에 대한 설명을 연결
		_deviceContext->IASetInputLayout(_inputLayout.Get());
		// 각 정점들을 어떻게 연결할것인지 설정
		// https://vitacpp.tistory.com/37
		// 삼각형 형태로 연결할것
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS(코딩 가능)
		// 사용할 vertexShader 연결
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);

		// RS(코딩 불가)

		// PS(코딩 가능)
		// 사용할 pixelShader 연결
		_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);

		// OM(코딩 불가)
		// 입력한 값을 그려달라고 요청
		_deviceContext->Draw(_vertices.size(), 0);
	}

	// Render 실행
	RenderEnd();
}

void Game::RenderBegin()
{
	// OM : Output Merger, 렌더링 파이프라인의 마지막 단계
	// 후면 Buffer에 연산해서 Render하길 요청함
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	// 초기 색상으로 한번 밀어줌
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
	// RS : Rasterizer Stage : 렌더링 파이프라인의 단계
	// 화면의 크기 정보를 설정함
	_deviceContext->RSSetViewports(1, &_viewport);
}

void Game::RenderEnd()
{
	// https://learn.microsoft.com/ko-kr/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-present
	// 후면 Buffer에 Render된 것을 전면 Buffer에 복사해서 화면을 출력해달라 요청
	HRESULT hr = _swapChain->Present(1, 0);
	// 후면 Buffer의 데이터를 전면 Buffer에 복사 실패하면 프로그램 중단
	assert(SUCCEEDED(hr));
}

// device와 SwapChain 생성
void Game::CreateDeviceAndSwapChain()
{
	// https://m.blog.naver.com/danhana123/221969051284
	// SwapChain의 설정값을 저장하는 구조체
	DXGI_SWAP_CHAIN_DESC desc;
	// desc를 0으로 밀어버림. 필요한 정보만 채울수 있음
	ZeroMemory(&desc, sizeof(desc)); // == ::memset(&desc, 0, sizeof(desc))

	// SwapChain 기본 설정
	{
		desc.BufferDesc.Width = _width;		// Buffer의 가로 크기 (윈도우 창 크기랑 같게 함)
		desc.BufferDesc.Height = _height;	// Buffer의 세로 크기 (윈도우 창 크기랑 같게 함)
		desc.BufferDesc.RefreshRate.Numerator = 60; // 화면 주사율 (hz), 분자
		desc.BufferDesc.RefreshRate.Denominator = 1; // 분모
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA가 각각 8bit = 32bit, UNORM : Unsigned Normalize (정규화)
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 스캔라인 그리기 모드 설정 (기본값, 스캔 라인 순서를 지정하지 않음)
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 스케일링 모드 설정 (기본값, 지정되지 않은 크기 조정)
		desc.SampleDesc.Count = 1; // MSAA(안티 앨리어싱)할 때 사용될 픽셀 샘플링의 갯수, 값이 클수록 보간 값이 커짐, 1 = 안티앨리어싱 사용 안함 
		desc.SampleDesc.Quality = 0; // 이미지 품질 수준. 값이 높아질 수록 품질이 높아지지만 성능은 떨어짐
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 출력용 버퍼로 쓸 것이다
		desc.BufferCount = 1; // 후면 Buffer 개수
		desc.OutputWindow = _hwnd; // 윈도우 핸들
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // SwapChain의 기본 정보
		desc.Windowed = true; // 윈도우창 모드
	}

	// D3D11CreateDeviceAndSwapChain 인자 설명 : https://pub-repository.tistory.com/96
	// https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
	// HRESULT : 일종의 bool과 비슷함
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr, // nullptr이면 IDXGIFactoryl::EnumAdpters의 첫번째인 기본 어댑터를 사용하게 됨.
		D3D_DRIVER_TYPE_HARDWARE, // GPU를 사용하겠다를 의미, 그래픽카드가 없으면 다른 걸 사용해야함
		nullptr, // 그래픽카드를 사용하기 때문에 nullptr 사용
		0, // 사용할 DX11의 API레이어를 설정
		nullptr, //지원하는 DX11의 Feature_Level을 설정함. nullptr이면 지원가능한 최상의 버전을 선택함
		0,	// 위에서 설정한 Feature_Level의 배열의 개수
		D3D11_SDK_VERSION, // 단순 매크로, 사용하는 DX SDK 버전을 입력함
		&desc, // SwapChain의 설정값을 저장한 구조체의 주소

		// _device.Get(); -> ID3D11Device*을 반환
		// _device.GetAddressOf(); -> ID3D11Device**을 반환,_device.Get()의 주소값
		_swapChain.GetAddressOf(), // 생성된 SwapChain를 저장함
		_device.GetAddressOf(), // 생성된 Device를 저장함
		nullptr,
		_deviceContext.GetAddressOf() // 생성된 deviceContext를 저장함
	);

	// D3D11CreateDeviceAndSwapChain가 실패하면 프로그램을 중단시킴
	assert(SUCCEEDED(hr));
}

// RenderTargetView(RTV) 생성
void Game::CreateRenderTargetView()
{
	HRESULT hr;

	// ID3D11Texture2D : 다용도 사용 가능. ex) png파일
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	// ID3D11Texture2D타입으로 0번 Buffer(후면 Buffer)를 backBuffer에 저장
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	// GetBuffer()를 실패하면 프로그램 중단
	assert(SUCCEEDED(hr));

	// RenderTargetView 생성
	// 후면 Buffer를 설명하는 RTV를 생성해서 _renderTargetView에 저장
	hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	// CreateRenderTargetViewt를 실패하면 프로그램 중단
	assert(SUCCEEDED(hr));
}

void Game::SetViewport()
{
	_viewport.TopLeftX = 0.f;
	_viewport.TopLeftY = 0.f;
	_viewport.Width = static_cast<float>(_width);
	_viewport.Height = static_cast<float>(_height);
	_viewport.MinDepth = 0.f;
	_viewport.MaxDepth = 1.f;
}

// 도형 생성
void Game::CreateGeometry()
{
	// 정점 정보
	{
		_vertices.resize(3);

		_vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
		_vertices[0].color = Color(1.0f, 0.f, 0.f, 1.0f);

		_vertices[1].position = Vec3(0.f, 0.5f, 0.f);
		_vertices[1].color = Color(0.f, 1.0f, 0.f, 1.0f);

		_vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
		_vertices[2].color = Color(0.f, 0.f, 1.f, 1.0f);
	}

	// GPU의 VRAM에 정점 정보를 가지는 Buffer를 생성함
	{
		D3D11_BUFFER_DESC desc;
		// desc를 전체 값을 0으로 초기화 -> 필요한 값만 설정해서 사용함
		// 모든 값을 설정할 것 아니면 0으로 초기화 이후 사용
		ZeroMemory(&desc, sizeof(desc));
		// Buffer의 사용방식 설정
		// https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/ne-d3d11-d3d11_usage
		// https://myoung-min.tistory.com/10
		// D3D11_USAGE_IMMUTABLE : GPU는 읽기전용으로 사용, CPU는 접근 못함
		desc.Usage = D3D11_USAGE_IMMUTABLE; 
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // VertexBuffer를 만드는데 사용할 것이다
		desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * _vertices.size()); // Vertex 구조체 크기 * 배열 갯수

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		// CPU에서 GPU에게 전달할 데이터 시작 주소
		data.pSysMem = _vertices.data(); // .data() 배열의 첫번째 값 주소

		// _vertices배열 정보를 가진 VertexBuffer 타입의 Buffer 생성
		_device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
	}
}

// vertexShader의 입력된 정보를 어떻게 사용하는지 설명
void Game::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// Shader에서 POSITION은 32bit float가 3개(12byte)로 이루어짐
		// Vertex구조체에서 position은 구조체 시작 0byte 이후 자리에 있음
		// Vertex Data로 이루어짐
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		// Shader COLOR은 32bit float가 4개로 이루어짐
		// Vertex구조체의 Color은 시작지점에서 12byte 이후 자리에 있음
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// layout의 배열 개수
	const int32 count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	// VertexBuffer의 멤버와 VS함수인 VS_INPUT(구조체) 멤버와 연결 및 설명
	// 전달하는 값의 정보, 전달하는 값의 개수, 
	// 셰이더에 대한 정보를 가진 주소, 셰이더에 대한 정보크기, 셰이더에 대한 정보를 저장할 위치
	_device->CreateInputLayout(layout, count,
		_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
}

// 셰이더 불러오기
// 파일 경로, 함수 이름, 셰이더 버전, 저장할 Blob
void Game::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
{
	// 디버그 용도 , 최적화는 건너뜀
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	// d3dcompiler.h
	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		blob.GetAddressOf(), // 불러온 셰이더 정보를 blob에 저장함,
		nullptr
	);

	// 셰이더 파일을 불러오지 못하면 프로그램 종료
	assert(SUCCEEDED(hr));
}

void Game::CreateVS()
{
	// L"" : 유니코드(UTF-16)으로 처리됨(2Byte, wchar)
	// Default.hlsl의 VS함수 내용을 _vsBlob이 저장하게 됨
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0", _vsBlob);

	// _vsBlob이 가지고 있는 데이터를 _vertexBuffer를 생성함
	// GPU에서 _vertexBuffer를 사용하여 데이터를 읽을 수 있음
	HRESULT hr =_device->CreateVertexShader(
		_vsBlob->GetBufferPointer(),
		_vsBlob->GetBufferSize(),
		nullptr,
		_vertexShader.GetAddressOf()
	);

	// VertexShader를 생성하지 못하면 프로그램 중단
	assert(SUCCEEDED(hr));
}

void Game::CreatePS()
{
	// L"" : 유니코드(UTF-16)으로 처리됨(2Byte, wchar)
	// Default.hlsl의 VS함수 내용을 _vsBlob이 저장하게 됨
	LoadShaderFromFile(L"Default.hlsl", "PS", "ps_5_0", _psBlob);

	// _psBlob이 가지고 있는 데이터를 _pixelShader를 생성함
	// GPU에서 _pixelShader를 사용하여 데이터를 읽을 수 있음
	HRESULT hr = _device->CreatePixelShader(
		_psBlob->GetBufferPointer(),
		_psBlob->GetBufferSize(),
		nullptr,
		_pixelShader.GetAddressOf()
	);

	// PixelShader를 생성하지 못하면 프로그램 중단
	assert(SUCCEEDED(hr));
}


