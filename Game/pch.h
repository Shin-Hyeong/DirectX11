#pragma once

#include <vector>
#include <list>
#include <map>
#include <unordered_map>

// Window
#include <Windows.h>
#include <assert.h>

// DX11
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>		// ComPtr
#include <DirectXMath.h>

// DX Tex (외부 라이브러리)
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
using namespace DirectX;
using namespace Microsoft::WRL; // ComPtr 관련

// DX11 라이브러리 링크
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG // 디버그 모드이면 debug.lib사용
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else	// 디버그 모드가 아니면 일반 lib사용
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif



// Define
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

using Vec2 = DirectX::XMFLOAT2;
using Vec3 = DirectX::XMFLOAT3;
using Vec4 = DirectX::XMFLOAT4;
using Color = DirectX::XMFLOAT4;

// Window
const int32 GWinSizeX = 800;
const int32 GWinSizeY = 600;
