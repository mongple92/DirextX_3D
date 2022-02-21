// ���̺귯�� ��ũ
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

#include <Windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma warning ( disable : 4996 )
#include <strsafe.h>
#pragma warning ( disable : 4996 )

// dx �۷ι� ���� ����
LPDIRECT3D9					g_pD3D = NULL;
LPDIRECT3DDEVICE9			g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9		g_pVB = NULL;
LPDIRECT3DINDEXBUFFER9		g_pIB = NULL;
LPDIRECT3DTEXTURE9			g_pTexture = NULL;

// ����� ���ؽ� ����ü
struct CUSTOMVERTEX
{
	// ��ġ�� �븻����
	// �⺻ ���ؽ� ���� ( ��ġ, �븻���� )
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;

	// �ؽ��Ŀ� ���� ����
	FLOAT u, v;
}; // struct CUSTOMVERTEX

// FVF : flexible vertex format, �����ϴ� ���α׷��� �˸°� ���ο� ���� ������ ���� �� �ֵ��� ������ ����
// ��, ���α׷��Ӱ� ���� ������ ������ �����ؼ� ����� �� �ֵ��� DX���� �����ϴ� ���
// ���α׷��Ӵ� �ʿ�� �ϴ� ���� ������ �����ϸ� ������ �� �ִ�.
#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )

HRESULT InitD3D(HWND hWnd)
{
	// d3d ������Ʈ ����
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return E_FAIL;
	} // if

	// d3d��ġ ����
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// d3d��ġ ����
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	} // if

	// �ø����
	// �ø���� �⺻���� none���� �����ϸ� CCW�� �׷�����
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// z���� ����
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	// Turn off D3D lighting
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
} // HRESULT InitD3D

// �� ���⺤�͸� �޾� �븻���� ���մϴ�.
D3DXVECTOR3 GetVector3Normal(D3DXVECTOR3 dirVec1, D3DXVECTOR3 dirVec2)
{
	// ������ �ٲٸ� �ȱ׷���. ������ �ٲ�� �׸��� ������ �ٲ��. �׸��� ���⿡ ���� ������, �޼� ��Ģ�� �ٲ��.
	// �� ���͵� ���ó�� ���ȴ�. ���� �������α��� �� ������ ����� ������Ѵ�.
	// ������ �������.
	// �������� �� �� ���͸� �� ��鿡 ������ ���͸� ���մϴ�.
	// 
	// ���⺤���� ����ȭ�� ���� ���մϴ�.
	D3DXVECTOR3 vec1;
	D3DXVec3Normalize(&vec1, &dirVec1);

	// ���⺤���� ����ȭ�� ���� ���մϴ�.
	D3DXVECTOR3 vec2;
	D3DXVec3Normalize(&vec2, &dirVec2);

	// �������� �븻���͸� ���մϴ�.
	D3DXVECTOR3 normalVec;
	D3DXVec3Cross(&normalVec, &vec1, &vec2);

	return normalVec;
} // D3DXVECTOR3 Normal

// GetAsyncKeyState�� ����ؼ� Ű�Է��� ���� ������ ó���ϵ��� �� ���ִ�.

HRESULT RenderTerrain(int grid, int polygon, int vertex, int index)
{
	// ���ؽ� ���۸� �����մϴ�.
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(vertex * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT,
		&g_pVB, NULL)))
	{
		return E_FAIL;
	} // if

	// ���ؽ��� �����մϴ�.
	CUSTOMVERTEX* pVertex;
	if (FAILED(g_pVB->Lock(0, 0, (void**)&pVertex, 0)))
	{
		return E_FAIL;
	} // if

	int x = 0;
	int z = 0;
	int v = 0;
	float uvOffset = (1.0f / (float)grid);

	// ���ؽ� ����
	for (z = 0; z <= grid; z++)
	{
		for (x = 0; x <= grid; x++)
		{
			// ��ġ��
			pVertex[v].position.z = (float)z;
			// sin������ y���� �ش�.
			float sf = sinf((float)x);
			float cf = cosf((float)z);
			//cout << "sinf(%f) : %f * cosf(%f) : %f = %f\n", x, sf, z, cf, sf*cf << endl;
			pVertex[v].position.y = sf * cf;
			pVertex[v].position.x = (float)x;
			
			// �븻���� �ϴ� y+�������� �ֵ����մϴ�.
			const D3DXVECTOR3 vec = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			D3DXVECTOR3 nv;
			D3DXVec3Normalize( &nv, &vec );

			pVertex[v].normal = nv;

			// u, v�� ä��ϴ�.
			// ��� ���ؽ��� ���ؼ� uv���� �������־�� �մϴ�.
			pVertex[v].u = x * uvOffset;
			pVertex[v].v = z * uvOffset;

			v++;
		} // for
	} // for
	
	g_pVB->Unlock();

	// �ε��� ���۸� �����մϴ�.
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

	// �ε��� ����
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
	// ���׸����� �����մϴ�.
	// ��ǻ��, �ں��Ʈ�� ����
	// ��ǻ�� : 
	// ��������� ���� �����մϴ�.
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
	// ���� ����� �����մϴ�.
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	//D3DXMatrixRotationY(&matWorld, timeGetTime() / 1500.0F);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	float g = (float)grid / 2.0f;
	D3DXVECTOR3 atVec = D3DXVECTOR3(g, 0.0f, g);

	// �� ����� �����մϴ�.
	D3DXVECTOR3 vEyePt( -atVec.x/2, 30.0F, -atVec.z/2);
	D3DXVECTOR3 vLookAtPt( atVec.x, atVec.y, atVec.z );
	D3DXVECTOR3 vUpVec(0.0F, 1.0F, 0.0F);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookAtPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	// ��������� �����մϴ�.
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0F, 1.0F, 100.0F);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
} // VOID SetupMatrices

VOID Render(int grid, int polygon, int vertex, int index)
{
	// �ĸ���� Ŭ����, z���� Ŭ����
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0F, 0);

	// ���� �����մϴ�.
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// ����Ʈ�� �����մϴ�.
		SetupLight(1, 1, 1);

		// ����, ��, ���� ��Ʈ������ �����մϴ�.
		SetupMatrices( grid );

		// �ؽ��ĸ� �����մϴ�.
		g_pd3dDevice->SetTexture(0, g_pTexture);

		// ���ؽ����� ��ҵ��� �׸��ϴ�.
		// ����� ���� ���۸� ����̽��� ������ ��Ʈ���� ���� ��Ų��.
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		// ���� ������ ����̽��� �����Ѵ�.
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

	// ������ Ŭ���� �������Ϳ� ���
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"D3D Topography", NULL
	};
	RegisterClassEx(&wc);

	// ������ �� ����
	HWND hWnd = CreateWindow(L"D3D Topography", L"D3D Topography",
		WS_OVERLAPPEDWINDOW, 100, 100, 800, 800, NULL, NULL, wc.hInstance, NULL);

	// d3d �ʱ�ȭ
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		int grid = 100;
		int vertex = (grid + 1) * (grid + 1);
		int polygon = grid * grid * 2;
		int index = polygon * 3;

		// ������Ʈ�� ����
		if (SUCCEEDED(InitGeometry(grid, polygon, vertex, index)))
		{
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			// ������ �޼���
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