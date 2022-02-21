// 라이브러리 링크
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

#include <Windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma warning ( disable : 4996 )
#include <strsafe.h>
#pragma warning ( disable : 4996 )

// dx 글로벌 변수 선언
LPDIRECT3D9					g_pD3D = NULL;
LPDIRECT3DDEVICE9			g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9		g_pVB = NULL;
LPDIRECT3DINDEXBUFFER9		g_pIB = NULL;
LPDIRECT3DTEXTURE9			g_pTexture = NULL;

// 사용할 버텍스 구조체
struct CUSTOMVERTEX
{
	// 위치와 노말벡터
	// 기본 버텍스 구성 ( 위치, 노말벡터 )
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;

	// 텍스쳐에 사용될 구성
	FLOAT u, v;
}; // struct CUSTOMVERTEX

// FVF : flexible vertex format, 제작하는 프로그램에 알맞게 새로운 정점 포맷을 정할 수 있도록 유연성 제공
// 즉, 프로그래머가 직접 정점의 구조를 정의해서 사용할 수 있도록 DX에서 제공하는 기능
// 프로그래머는 필요로 하는 정점 포맷을 선택하며 조합할 수 있다.
#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )

HRESULT InitD3D(HWND hWnd)
{
	// d3d 오브젝트 생성
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return E_FAIL;
	} // if

	// d3d장치 셋팅
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// d3d장치 생성
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	} // if

	// 컬링모드
	// 컬링모드 기본으로 none으로 설정하면 CCW로 그려진다
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// z버퍼 끄기
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	// Turn off D3D lighting
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
} // HRESULT InitD3D

// 두 방향벡터를 받아 노말값을 구합니다.
D3DXVECTOR3 GetVector3Normal(D3DXVECTOR3 dirVec1, D3DXVECTOR3 dirVec2)
{
	// 순서를 바꾸면 안그려짐. 순서가 바뀌면 그리는 방향이 바뀐다. 그리는 방향에 따라 오른손, 왼손 법칙이 바뀐다.
	// 두 벡터도 행렬처럼 계산된다. 따라서 외적으로구할 땐 순서도 고려를 해줘야한다.
	// 내적은 상관없음.
	// 외적으로 위 두 벡터를 낀 평면에 수직인 벡터를 구합니다.
	// 
	// 방향벡터의 정규화된 값을 구합니다.
	D3DXVECTOR3 vec1;
	D3DXVec3Normalize(&vec1, &dirVec1);

	// 방향벡터의 정규화된 값을 구합니다.
	D3DXVECTOR3 vec2;
	D3DXVec3Normalize(&vec2, &dirVec2);

	// 외적으로 노말벡터를 구합니다.
	D3DXVECTOR3 normalVec;
	D3DXVec3Cross(&normalVec, &vec1, &vec2);

	return normalVec;
} // D3DXVECTOR3 Normal

// GetAsyncKeyState를 사용해서 키입렵이 있을 때마다 처리하도록 할 수있다.

HRESULT RenderTerrain(int grid, int polygon, int vertex, int index)
{
	// 버텍스 버퍼를 생성합니다.
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(vertex * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT,
		&g_pVB, NULL)))
	{
		return E_FAIL;
	} // if

	// 버텍스를 생성합니다.
	CUSTOMVERTEX* pVertex;
	if (FAILED(g_pVB->Lock(0, 0, (void**)&pVertex, 0)))
	{
		return E_FAIL;
	} // if

	int x = 0;
	int z = 0;
	int v = 0;
	float uvOffset = (1.0f / (float)grid);

	// 버텍스 셋팅
	for (z = 0; z <= grid; z++)
	{
		for (x = 0; x <= grid; x++)
		{
			// 위치값
			pVertex[v].position.z = (float)z;
			// sin값으로 y값을 준다.
			float sf = sinf((float)x);
			float cf = cosf((float)z);
			//cout << "sinf(%f) : %f * cosf(%f) : %f = %f\n", x, sf, z, cf, sf*cf << endl;
			pVertex[v].position.y = sf * cf;
			pVertex[v].position.x = (float)x;
			
			// 노말값을 일단 y+방향으로 주도록합니다.
			const D3DXVECTOR3 vec = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			D3DXVECTOR3 nv;
			D3DXVec3Normalize( &nv, &vec );

			pVertex[v].normal = nv;

			// u, v를 채웁니다.
			// 모든 버텍스에 대해서 uv값을 설정해주어야 합니다.
			pVertex[v].u = x * uvOffset;
			pVertex[v].v = z * uvOffset;

			v++;
		} // for
	} // for
	
	g_pVB->Unlock();

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(g_pd3dDevice->CreateIndexBuffer(index * sizeof(vertex), D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL)))
	{
		return E_FAIL;
	}

	void* buffer = NULL;

	if (FAILED(g_pIB->Lock(0, 0, &buffer, 0)))
	{
		return E_FAIL;
	}

	WORD* ib = (WORD*)buffer;

	int cnt = 0;
	int b = 0;
	v = 0;

	// 인덱스 셋팅
	for (z = 0; z < grid; z++)
	{
		for (x = 0; x < grid; x++)
		{
			b = v + (grid + 1);

			ib[cnt++] = v;
			ib[cnt++] = b + 1;
			ib[cnt++] = b;

			ib[cnt++] = v;
			ib[cnt++] = v + 1;
			ib[cnt++] = b + 1;

			v++;
		} // for

		v++;

	} // for

	g_pIB->Unlock();

	return S_OK;
} // HRESULT RenderTerrain

HRESULT InitGeometry(int grid, int polygon, int vertex, int index)
{
	// Use D3DX to create a texture from a file based image
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"terrain.png", &g_pTexture)))
	{
		// If texture is not in current folder, try parent folder
		if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"..\\terrain.png", &g_pTexture)))
		{
			MessageBox(NULL, L"Could not find terrain.png", L"Topography.exe", MB_OK);
			return E_FAIL;
		}
	}

	RenderTerrain(grid, polygon, vertex, index);

	return S_OK;
} // HRESULT InitGeometry

VOID Cleanup()
{
	if (g_pVB != NULL)
	{
		g_pVB->Release();
	} // if

	if (g_pIB != NULL)
	{
		g_pIB->Release();
	} // if

	if (g_pd3dDevice != NULL)
	{
		g_pd3dDevice->Release();
	} // if

	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
	} // if

	if (g_pTexture != NULL)
	{
		g_pTexture->Release();
	} // if

} // VOID Cleanup

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM IParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, IParam);
} // LRESULT WINAPI MsgProc

VOID SetupLight(float r, float g, float b)
{
	// 머테리얼을 셋팅합니다.
	// 디퓨즈, 앰비언트를 설정
	// 디퓨즈 : 
	// 노란색으로 색을 셋팅합니다.
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = r;
	mtrl.Diffuse.g = mtrl.Ambient.g = g;
	mtrl.Diffuse.b = mtrl.Ambient.b = b;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0F;
	g_pd3dDevice->SetMaterial(&mtrl);

	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0F;
	light.Diffuse.g = 1.0F;
	light.Diffuse.b = 1.0F;
	vecDir = D3DXVECTOR3(0.0F, -1.0F, 1.0F);
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
	light.Range = 1000.0F;
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x00202020);
} // VOID SetupLight

VOID SetupMatrices( int grid )
{
	// 월드 행렬을 셋팅합니다.
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	//D3DXMatrixRotationY(&matWorld, timeGetTime() / 1500.0F);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	float g = (float)grid / 2.0f;
	D3DXVECTOR3 atVec = D3DXVECTOR3(g, 0.0f, g);

	// 뷰 행렬을 셋팅합니다.
	D3DXVECTOR3 vEyePt( -atVec.x/2, 30.0F, -atVec.z/2);
	D3DXVECTOR3 vLookAtPt( atVec.x, atVec.y, atVec.z );
	D3DXVECTOR3 vUpVec(0.0F, 1.0F, 0.0F);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookAtPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	// 투영행렬을 셋팅합니다.
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0F, 1.0F, 100.0F);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
} // VOID SetupMatrices

VOID Render(int grid, int polygon, int vertex, int index)
{
	// 후면버퍼 클리어, z버퍼 클리어
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0F, 0);

	// 씬을 시작합니다.
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// 라이트를 셋팅합니다.
		SetupLight(1, 1, 1);

		// 월드, 뷰, 투영 매트릭스를 셋팅합니다.
		SetupMatrices( grid );

		// 텍스쳐를 셋팅합니다.
		g_pd3dDevice->SetTexture(0, g_pTexture);

		// 버텍스버퍼 요소들을 그립니다.
		// 출력할 정점 버퍼를 디바이스의 데이터 스트림과 연결 시킨다.
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		// 정점 포맷을 디바이스에 지정한다.
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->SetIndices(g_pIB);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertex, 0, polygon);
		//g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 14 * 3, 0, 20);
		g_pd3dDevice->EndScene();
	} // if

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
} // VOID Render

INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, INT)
{
	UNREFERENCED_PARAMETER(hInst);

	// 윈도우 클래스 레지스터에 등록
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"D3D Topography", NULL
	};
	RegisterClassEx(&wc);

	// 윈도우 앱 생성
	HWND hWnd = CreateWindow(L"D3D Topography", L"D3D Topography",
		WS_OVERLAPPEDWINDOW, 100, 100, 800, 800, NULL, NULL, wc.hInstance, NULL);

	// d3d 초기화
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		int grid = 100;
		int vertex = (grid + 1) * (grid + 1);
		int polygon = grid * grid * 2;
		int index = polygon * 3;

		// 지오메트리 생성
		if (SUCCEEDED(InitGeometry(grid, polygon, vertex, index)))
		{
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			// 윈도우 메세지
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				} // if
				else
				{
					Render(grid, polygon, vertex, index);
				} // if..else..
			} // while
		} // if
	} // if

	UnregisterClass(L"D3D Topography", wc.hInstance);
	return 0;
} // INT WINAPI wWinMain