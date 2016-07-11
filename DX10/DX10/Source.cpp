#include <stdio.h>
#include <crtdbg.h>
#include <tchar.h>

#include <windows.h>
#include <tchar.h>
#include <D3D10.h>
#include <d3dx10.h>
#include <dxerr.h>

#define CLIENT_WIDTH  1280
#define CLIENT_HEIGHT  720

#define GAME_FPS (1000/60)

struct MyVertex {
	D3DXVECTOR3 Pos;      // 頂点位置
	D3DXVECTOR3 Color;    // 頂点カラー
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

//メイン関数
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	HWND hWnd = NULL;
	MSG msg;
	// ウィンドウの初期化
	static TCHAR* szAppName = _T("DragonKingdom");
	WNDCLASSEX  wndclass;



	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);


	hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		GetSystemMetrics(SM_CXSCREEN) / 2 - CLIENT_WIDTH / 2,
		GetSystemMetrics(SM_CYSCREEN) / 2 - CLIENT_HEIGHT / 2,
		CLIENT_WIDTH, CLIENT_HEIGHT, NULL, NULL, hInst, NULL);


	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	SetWindowText(hWnd, _T("DX10"));

	ID3D10Device* pDevice = NULL;
	IDXGISwapChain* pSwapChain = NULL;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	swapChainDesc.BufferDesc.Width = CLIENT_WIDTH;
	swapChainDesc.BufferDesc.Height = CLIENT_HEIGHT;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = true;      // ウィンドウモード
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, NULL, D3D10_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice);
	if (FAILED(hr))
	{
		return 0;
	}

	//描画ターゲットビューの生成
	ID3D10Texture2D *pBackBuffer;
	ID3D10RenderTargetView *pRenderTargetView;
	pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(pDevice->CreateRenderTargetView(pBackBuffer, 0, &pRenderTargetView))) {
		pSwapChain->Release(); pDevice->Release();
		return 0;
	};	pDevice->OMSetRenderTargets(1, &pRenderTargetView, NULL);
	pBackBuffer->Release();

	//ビューポートの設定
	D3D10_VIEWPORT viewPort;
	viewPort.Width = CLIENT_WIDTH;
	viewPort.Height = CLIENT_HEIGHT;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	pDevice->RSSetViewports(1, &viewPort);

	//頂点を定義
	MyVertex vtx[] = {
		D3DXVECTOR3(0.0f, 0.5f, 0.5f), D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(0.5f, -0.5f, 0.5f), D3DXVECTOR3(1.0f, 1.0f, 1.0f),
		D3DXVECTOR3(-0.5f, -0.5f, 0.5f), D3DXVECTOR3(1.0f, 1.0f, 1.0f)
	};
	
	//SetFVF的なやつ
	D3D10_INPUT_ELEMENT_DESC MyVertexDesc[] = {
		{ "IN_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "IN_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = 3 * sizeof(MyVertex);
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA subresourceData;
	subresourceData.pSysMem = vtx;

	ID3D10Buffer *pBuffer;
	if (FAILED(pDevice->CreateBuffer(&bufferDesc, &subresourceData, &pBuffer)))
		return false;

	DWORD NowTime = timeGetTime();
	DWORD OldTime = timeGetTime();

	// メッセージループ
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			NowTime = timeGetTime();
			if (NowTime - OldTime >= GAME_FPS)
			{
				float ClearColor[4] = { 0.0f, 0.125f, 0.6f, 1.0f }; // RGBA
				pDevice->ClearRenderTargetView(pRenderTargetView, ClearColor);
				pSwapChain->Present(0, 0);
				OldTime = timeGetTime();
			}
		}
	}

	pDevice->ClearState();
	pRenderTargetView->Release();
	pDevice->Release();
	pSwapChain->Release();
	return (INT)msg.wParam;
}

/*  ウインドウプロシージャ  */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return  0;
		break;
	case WM_KEYDOWN:
		switch ((CHAR)wparam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
			break;
		}
		break;
	case WM_ACTIVATE:
		switch ((CHAR)wparam)
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:

			break;
		}
	}
	return  DefWindowProc(hwnd, message, wparam, lparam);
}