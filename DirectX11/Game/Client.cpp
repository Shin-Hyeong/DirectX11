// Game.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "Game.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
HWND GHwnd;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // 윈도우 창 정보 등록
    MyRegisterClass(hInstance);

    // 윈도우 창 생성
    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;

    // Game 생성
    Game game;
    game.BeginPlay(GHwnd);
    MSG msg = {};

    // 기본 메시지 루프입니다:
    while (msg.message != WM_QUIT)
    {
        // PeekMessage : GetMessage 대신해서 사용함
        // = true : 어떠한 msg값이 존재함
        // - 메시지가 있는지 없는지만 확인함. -> 메시지가 없으면 if함수를 넘기고 실행시킴
        // 
        // (&메시지, Window 핸들, 메시지 필터링, 메시지 필터링)
        // - Window 핸들이 nullptr이면 현재 스레드와 관련된 모든 메시지를 받게 됨.
        // - 메시지 필터링 : 0이면 필터링 없이 모든 메시지를 받겠다
        // PM_REMOVE : 입력 Queue에서 순차적으로 꺼내도록함.
        // 마우스 입력이나 키보드 입력을 순차적으로 입력Queue에 저장하여 순차적으로 처리할 수 있도록 함
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // msg가 없으면 game의 Tick()과 Render()를 실행
        else
        {
            game.Tick();
            game.Render();
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL; // 윈도우창 메뉴
    wcex.lpszClassName  = L"Game";
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   // 윈도우 창 크기 설정
   RECT windowRect = { 0, 0, GWinSizeX, GWinSizeY };
   // 메뉴창을 제외하고 설정한 사이즈로 윈도우창 설정
   ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false); 

   GHwnd = CreateWindowW(L"Game", L"Client", WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
       nullptr, nullptr, hInstance, nullptr
   ); // 윈도우 창 설정한 크기로 생성

   if (!GHwnd)
   {
      return FALSE;
   }

   ::ShowWindow(GHwnd, nCmdShow);
   ::UpdateWindow(GHwnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}