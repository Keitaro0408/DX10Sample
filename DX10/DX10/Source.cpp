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

#define VERTEXNUM 4

#define GAME_FPS (1000/60)

struct MyVertex {
	D3DXVECTOR3 Pos;      // 頂点位置
	D3DXVECTOR2 UV;
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
		D3DXVECTOR3(-0.2f, 0.2f, 0.0f), D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR3(0.2f, 0.2f, 0.0f), D3DXVECTOR2(1.0f, 0.0f),
		D3DXVECTOR3(-0.2f, -0.2f, 0.0f), D3DXVECTOR2(0.0f, 1.0f),
		D3DXVECTOR3(0.2f, -0.2f, 0.0f), D3DXVECTOR2(1.0f, 1.0f)
	};
	
	//SetFVF的なやつ
	D3D10_INPUT_ELEMENT_DESC MyVertexDesc[] = {
		{ "IN_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = VERTEXNUM * sizeof(MyVertex);
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA subresourceData;
	subresourceData.pSysMem = vtx;

	ID3D10Buffer *pBuffer;
	if (FAILED(pDevice->CreateBuffer(&bufferDesc, &subresourceData, &pBuffer)))
	{
		return false;
	}

	UINT stride = sizeof(MyVertex);
	UINT offset = 0;
	pDevice->IASetVertexBuffers(0,1,&pBuffer,&stride,&offset);
	pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D10_PASS_DESC passDesc;
	ID3D10EffectTechnique *pTechnique;
	ID3D10Effect *pEffect;

	// シェーダファイルからエフェクトを作る
	if (FAILED(D3DX10CreateEffectFromFile("MyShader.fx", NULL, NULL,
		"fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, pDevice, NULL, NULL, &pEffect, NULL, NULL)))
		return FALSE;

	// エフェクトからテクニック情報を取得
	pTechnique = pEffect->GetTechniqueByName("SimpleRender");

	// テクニックからパス情報を取得
	pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	ID3D10InputLayout *pVertexLayout;

	// 頂点レイアウトを作成
	if (FAILED(pDevice->CreateInputLayout(
		MyVertexDesc, sizeof(MyVertexDesc) / sizeof(D3D10_INPUT_ELEMENT_DESC),
		passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &pVertexLayout)))
		return FALSE;

	// 頂点レイアウトを描画デバイスにセット
	pDevice->IASetInputLayout(pVertexLayout);

	//画像読み込み
	ID3D10ShaderResourceView*   pTextureRV = NULL;
	if (FAILED(D3DX10CreateShaderResourceViewFromFile(pDevice, "test.jpg", NULL, NULL, &pTextureRV, NULL)))
	{
		MessageBox(NULL,"","テクスチャの読み込みに失敗しました",MB_OK);
	}

	DWORD NowTime = timeGetTime();
	DWORD OldTime = timeGetTime();
	float ClearColor[4] = { 0.0f, 0.125f, 0.6f, 1.0f }; // RGBA
	float angle = 0.f;

	ID3D10EffectMatrixVariable *pWorldMatVar;
	ID3D10EffectMatrixVariable *pViewMatVar;
	ID3D10EffectMatrixVariable *pProjMatVar;
	ID3D10EffectShaderResourceVariable *pTexMatVar;

	D3DXMATRIX World;
	D3DXMATRIX View;
	D3DXMATRIX Proj;

	pWorldMatVar = pEffect->GetVariableByName("World")->AsMatrix();
	pViewMatVar = pEffect->GetVariableByName("View")->AsMatrix();
	pProjMatVar = pEffect->GetVariableByName("Proj")->AsMatrix();
	pTexMatVar = pEffect->GetVariableByName("Texture")->AsShaderResource();
	D3DXMatrixIdentity(&View);
	D3DXMatrixIdentity(&World);

	D3DXMatrixLookAtLH(&View, &D3DXVECTOR3(0, 0, -2.0f), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	D3DXMatrixPerspectiveFovLH(&Proj,
		(float)(D3DX_PI / 4.0), //視野角(ここでは45度をセットしてます)
		(float)(1280 / 720),     //画面アスペクト比
		1.0f,                 //クリッピング距離（これより近いのは描画しません）
		100.0f);          //クリッピング距離（これより遠いのは描画しません)

	pViewMatVar->SetMatrix((float*)&View);
	pProjMatVar->SetMatrix((float*)&Proj);
	pTexMatVar->SetResource(pTextureRV);
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
				
				// 回転
				D3DXMatrixRotationZ(&World, (float)D3DXToRadian(angle));
				pWorldMatVar->SetMatrix((float*)&World);

				D3D10_TECHNIQUE_DESC techDesc;
				pDevice->ClearRenderTargetView(pRenderTargetView, ClearColor);
				pTechnique->GetDesc(&techDesc);
				for (UINT p = 0; p < techDesc.Passes; ++p)
				{
					pTechnique->GetPassByIndex(p)->Apply(0);
					pDevice->Draw(VERTEXNUM, 0);
				}
				pSwapChain->Present(0, 0);				OldTime = timeGetTime();
			}
		}
	}

	pDevice->ClearState();
	pTextureRV->Release();
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