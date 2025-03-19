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

	// TODO
	// device와 SwapChain 생성
	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
}

void Game::Tick()
{
	
}

void Game::Render()
{
	// Render 준비 작업
	RenderBegin();

	// TODO
	// Render에 필요한 작업

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