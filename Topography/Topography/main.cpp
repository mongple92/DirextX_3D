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
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// z���� ����
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

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

	for (z = 0; z <= grid; z++)
	{
		for (x = 0; x <= grid; x++)
		{
			pVertex[v].position.x = (float)x;
			pVertex[v].position.z = (float)z;
			pVertex[v].position.y = 0.0f;
			v++;
		} // for
	} // for

	g_pVB->Unlock();

	// �ε��� ���۸� �����մϴ�.
	if (FAILED(g_pd3dDevice->CreateIndexBuffer(index, D3DUSAGE_WRITEONLY,
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

	int a = 0;
	int b = (grid + 1);

	return S_OK;
} // HRESULT RenderTerrain

HRESULT RenderHouse()
{
	// �ؽ��� ����
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"banana.bmp", &g_pTexture)))
	{
		// ���� ������ ���ٸ� ������������ �õ��մϴ�.
		if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"..\\banana.bmp", &g_pTexture)))
		{
			MessageBox(NULL, L"Could not find banana.bmp", L"Texture.exe", MB_OK);
			return E_FAIL;
		} // if
	} // if

	// ���ؽ� ���۸� �����մϴ�.
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(14 * 3 * sizeof(CUSTOMVERTEX),
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

	D3DXVECTOR3 normalVec;

	// ���� ��ġ�� �ι��׸��� z�������� �Ͼ
	// ��ġ�� ����, �븻���� ����, uv�� �ٸ��� �ٸ��� �׷�����մϴ�.
	// �ո�
	pVertex[0].position = D3DXVECTOR3(-1.0f, -1.0f, -2.0f);
	pVertex[0].u = 0.0f;
	pVertex[0].v = 1.0f;

	pVertex[1].position = D3DXVECTOR3(-1.0f, 1.0f, -2.0f);
	pVertex[1].u = 0.0f;
	pVertex[1].v = 0.0f;

	pVertex[2].position = D3DXVECTOR3(1.0f, 1.0f, -2.0f);
	pVertex[2].u = 1.0f;
	pVertex[2].v = 0.0f;

	pVertex[3].position = D3DXVECTOR3(1.0f, -1.0f, -2.0f);
	pVertex[3].u = 1.0f;
	pVertex[3].v = 1.0f;

	// �ո� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[1].position - pVertex[0].position, pVertex[2].position - pVertex[0].position);

	pVertex[0].normal = normalVec;
	pVertex[1].normal = normalVec;
	pVertex[2].normal = normalVec;
	pVertex[3].normal = normalVec;

	// �����ʸ�
	pVertex[4].position = D3DXVECTOR3(1.0f, -1.0f, -2.0f);
	pVertex[4].u = 0.0f;
	pVertex[4].v = 1.0f;

	pVertex[5].position = D3DXVECTOR3(1.0f, 1.0f, -2.0f);
	pVertex[5].u = 0.0f;
	pVertex[5].v = 0.0f;

	pVertex[6].position = D3DXVECTOR3(1.0f, 1.0f, 2.0f);
	pVertex[6].u = 1.0f;
	pVertex[6].v = 0.0f;

	pVertex[7].position = D3DXVECTOR3(1.0f, -1.0f, 2.0f);
	pVertex[7].u = 1.0f;
	pVertex[7].v = 1.0f;

	// �����ʸ� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[5].position - pVertex[4].position, pVertex[6].position - pVertex[4].position);

	pVertex[4].normal = normalVec;
	pVertex[5].normal = normalVec;
	pVertex[6].normal = normalVec;
	pVertex[7].normal = normalVec;

	// ���ʸ�
	pVertex[8].position = D3DXVECTOR3(-1.0f, -1.0f, 2.0f);
	pVertex[8].u = 0.0f;
	pVertex[8].v = 1.0f;

	pVertex[9].position = D3DXVECTOR3(-1.0f, 1.0f, 2.0f);
	pVertex[9].u = 0.0f;
	pVertex[9].v = 0.0f;

	pVertex[10].position = D3DXVECTOR3(-1.0f, 1.0f, -2.0f);
	pVertex[10].u = 1.0f;
	pVertex[10].v = 0.0f;

	pVertex[11].position = D3DXVECTOR3(-1.0f, -1.0f, -2.0f);
	pVertex[11].u = 1.0f;
	pVertex[11].v = 1.0f;

	// ���ʸ� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[9].position - pVertex[8].position, pVertex[10].position - pVertex[8].position);

	pVertex[8].normal = normalVec;
	pVertex[9].normal = normalVec;
	pVertex[10].normal = normalVec;
	pVertex[11].normal = normalVec;

	// ���ʸ�
	pVertex[12].position = D3DXVECTOR3(1.0f, -1.0f, 2.0f);
	pVertex[12].u = 0.0f;
	pVertex[12].v = 1.0f;

	pVertex[13].position = D3DXVECTOR3(1.0f, 1.0f, 2.0f);
	pVertex[13].u = 0.0f;
	pVertex[13].v = 0.0f;

	pVertex[14].position = D3DXVECTOR3(-1.0f, 1.0f, 2.0f);
	pVertex[14].u = 1.0f;
	pVertex[14].v = 0.0f;

	pVertex[15].position = D3DXVECTOR3(-1.0f, -1.0f, 2.0f);
	pVertex[15].u = 1.0f;
	pVertex[15].v = 1.0f;

	// ���ʸ� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[13].position - pVertex[12].position, pVertex[14].position - pVertex[12].position);

	pVertex[12].normal = normalVec;
	pVertex[13].normal = normalVec;
	pVertex[14].normal = normalVec;
	pVertex[15].normal = normalVec;

	// ���ʸ��� �ϴ� �׸� �ʿ䰡 �����Ƿ� �����մϴ�.
	// ���ʸ�
	pVertex[16].position = D3DXVECTOR3(-1.0f, 1.0f, -2.0f);
	pVertex[16].u = 0.0f;
	pVertex[16].v = 1.0f;

	pVertex[17].position = D3DXVECTOR3(-1.0f, 1.0f, 2.0f);
	pVertex[17].u = 0.0f;
	pVertex[17].v = 0.0f;

	pVertex[18].position = D3DXVECTOR3(1.0f, 1.0f, 2.0f);
	pVertex[18].u = 1.0f;
	pVertex[18].v = 0.0f;

	pVertex[19].position = D3DXVECTOR3(1.0f, 1.0f, -2.0f);
	pVertex[19].u = 1.0f;
	pVertex[19].v = 1.0f;

	// ���ʸ� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[17].position - pVertex[16].position, pVertex[18].position - pVertex[16].position);

	pVertex[16].normal = normalVec;
	pVertex[17].normal = normalVec;
	pVertex[18].normal = normalVec;
	pVertex[19].normal = normalVec;

	// �Ʒ��ʸ�
	pVertex[20].position = D3DXVECTOR3(-1.0f, -1.0f, 2.0f);
	pVertex[20].u = 0.0f;
	pVertex[20].v = 1.0f;

	pVertex[21].position = D3DXVECTOR3(-1.0f, -1.0f, -2.0f);
	pVertex[21].u = 0.0f;
	pVertex[21].v = 0.0f;

	pVertex[22].position = D3DXVECTOR3(1.0f, -1.0f, -2.0f);
	pVertex[22].u = 1.0f;
	pVertex[22].v = 0.0f;

	pVertex[23].position = D3DXVECTOR3(1.0f, -1.0f, 2.0f);
	pVertex[23].u = 1.0f;
	pVertex[23].v = 1.0f;

	// �Ʒ��ʸ� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[21].position - pVertex[20].position, pVertex[22].position - pVertex[20].position);

	pVertex[20].normal = normalVec;
	pVertex[21].normal = normalVec;
	pVertex[22].normal = normalVec;
	pVertex[23].normal = normalVec;

	// ���� ��
	pVertex[24].position = D3DXVECTOR3(-2.0f, 1.0f, -2.0f);
	pVertex[24].u = 0.0f;
	pVertex[24].v = 1.0f;

	pVertex[25].position = D3DXVECTOR3(0.0f, 3.0f, -2.0f);
	pVertex[25].u = 0.5f;
	pVertex[25].v = 0.0f;

	pVertex[26].position = D3DXVECTOR3(2.0f, 1.0f, -2.0f);
	pVertex[26].u = 1.0f;
	pVertex[26].v = 1.0f;

	// ���� �ո� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[25].position - pVertex[24].position, pVertex[26].position - pVertex[24].position);

	pVertex[24].normal = normalVec;
	pVertex[25].normal = normalVec;
	pVertex[26].normal = normalVec;

	// ���� ��
	pVertex[27].position = D3DXVECTOR3(2.0f, 1.0f, 2.0f);
	pVertex[27].u = 0.0f;
	pVertex[27].v = 1.0f;

	pVertex[28].position = D3DXVECTOR3(0.0f, 3.0f, 2.0f);
	pVertex[28].u = 0.5f;
	pVertex[28].v = 0.0f;

	pVertex[29].position = D3DXVECTOR3(-2.0f, 1.0f, 2.0f);
	pVertex[29].u = 1.0f;
	pVertex[29].v = 1.0f;

	// ���� �޸� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[28].position - pVertex[27].position, pVertex[29].position - pVertex[27].position);

	pVertex[27].normal = normalVec;
	pVertex[28].normal = normalVec;
	pVertex[29].normal = normalVec;

	// ���� �Ʒ�
	pVertex[30].position = D3DXVECTOR3(-2.0f, 1.0f, 2.0f);
	pVertex[30].u = 0.0f;
	pVertex[30].v = 1.0f;

	pVertex[31].position = D3DXVECTOR3(-2.0f, 1.0f, -2.0f);
	pVertex[31].u = 0.0f;
	pVertex[31].v = 0.0f;

	pVertex[32].position = D3DXVECTOR3(2.0f, 1.0f, -2.0f);
	pVertex[32].u = 1.0f;
	pVertex[32].v = 0.0f;

	pVertex[33].position = D3DXVECTOR3(2.0f, 1.0f, 2.0f);
	pVertex[33].u = 1.0f;
	pVertex[33].v = 1.0f;

	// ���� �Ʒ��ʸ� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[31].position - pVertex[30].position, pVertex[32].position - pVertex[30].position);

	pVertex[30].normal = normalVec;
	pVertex[31].normal = normalVec;
	pVertex[32].normal = normalVec;
	pVertex[33].normal = normalVec;

	// ���� ������
	pVertex[34].position = D3DXVECTOR3(2.0f, 1.0f, -2.0f);
	pVertex[34].u = 0.0f;
	pVertex[34].v = 1.0f;

	pVertex[35].position = D3DXVECTOR3(0.0f, 3.0f, -2.0f);
	pVertex[35].u = 0.0f;
	pVertex[35].v = 0.0f;

	pVertex[36].position = D3DXVECTOR3(0.0f, 3.0f, 2.0f);
	pVertex[36].u = 1.0f;
	pVertex[36].v = 0.0f;

	pVertex[37].position = D3DXVECTOR3(2.0f, 1.0f, 2.0f);
	pVertex[37].u = 1.0f;
	pVertex[37].v = 1.0f;

	// ���� �����ʸ� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[35].position - pVertex[34].position, pVertex[36].position - pVertex[34].position);

	pVertex[34].normal = normalVec;
	pVertex[35].normal = normalVec;
	pVertex[36].normal = normalVec;
	pVertex[37].normal = normalVec;

	// ���� ����
	pVertex[38].position = D3DXVECTOR3(-2.0f, 1.0f, 2.0f);
	pVertex[38].u = 0.0f;
	pVertex[38].v = 1.0f;

	pVertex[39].position = D3DXVECTOR3(0.0f, 3.0f, 2.0f);
	pVertex[39].u = 0.0f;
	pVertex[39].v = 0.0f;

	pVertex[40].position = D3DXVECTOR3(0.0f, 3.0f, -2.0f);
	pVertex[40].u = 1.0f;
	pVertex[40].v = 0.0f;

	pVertex[41].position = D3DXVECTOR3(-2.0f, 1.0f, -2.0f);
	pVertex[41].u = 1.0f;
	pVertex[41].v = 1.0f;

	// ���� ���ʸ� �븻���� ���մϴ�.
	normalVec = GetVector3Normal(pVertex[39].position - pVertex[38].position, pVertex[40].position - pVertex[38].position);

	pVertex[38].normal = normalVec;
	pVertex[39].normal = normalVec;
	pVertex[40].normal = normalVec;
	pVertex[41].normal = normalVec;

	g_pVB->Unlock();

	if (FAILED(g_pd3dDevice->CreateIndexBuffer(20 * 3 * 2, D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL)))
	{
		return E_FAIL;
	}

	// �ε����� �����մϴ�.
	void* buffer = NULL;

	if (FAILED(g_pIB->Lock(0, 0, &buffer, 0)))
	{
		return E_FAIL;
	}

	WORD* ib = (WORD*)buffer;

	ib[0] = 0;
	ib[1] = 1;
	ib[2] = 2;

	ib[3] = 0;
	ib[4] = 2;
	ib[5] = 3;

	ib[6] = 4;
	ib[7] = 5;
	ib[8] = 6;

	ib[9] = 4;
	ib[10] = 6;
	ib[11] = 7;

	ib[12] = 8;
	ib[13] = 9;
	ib[14] = 10;

	ib[15] = 8;
	ib[16] = 10;
	ib[17] = 11;

	ib[18] = 12;
	ib[19] = 13;
	ib[20] = 14;

	ib[21] = 12;
	ib[22] = 14;
	ib[23] = 15;

	//ib[24] = 16;
	//ib[25] = 17;
	//ib[26] = 18;

	//ib[27] = 16;
	//ib[28] = 18;
	//ib[29] = 19;

	ib[30] = 20;
	ib[31] = 21;
	ib[32] = 22;

	ib[33] = 20;
	ib[34] = 22;
	ib[35] = 23;

	ib[36] = 24;
	ib[37] = 25;
	ib[38] = 26;

	ib[39] = 27;
	ib[40] = 28;
	ib[41] = 29;

	ib[42] = 30;
	ib[43] = 31;
	ib[44] = 32;

	ib[45] = 30;
	ib[46] = 32;
	ib[47] = 33;

	ib[48] = 34;
	ib[49] = 35;
	ib[50] = 36;

	ib[51] = 34;
	ib[52] = 36;
	ib[53] = 37;

	ib[54] = 38;
	ib[55] = 39;
	ib[56] = 40;

	ib[57] = 38;
	ib[58] = 40;
	ib[59] = 41;

	return S_OK;
} // HRESULT RenderHouse

HRESULT InitGeometry(int grid, int polygon, int vertex, int index)
{
	RenderTerrain(grid, polygon, vertex, index);

	//RenderHouse();

	g_pIB->Unlock();

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

VOID SetupMatrices()
{
	// ���� ����� �����մϴ�.
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	//D3DXMatrixRotationY(&matWorld, timeGetTime() / 1500.0F);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// �� ����� �����մϴ�.
	D3DXVECTOR3 vEyePt(0.0F, 5.0F, -10.0F);
	D3DXVECTOR3 vLookAtPt(0.0F, 0.0F, 0.0F);
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
		SetupMatrices();

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
		L"D3D Study", NULL
	};
	RegisterClassEx(&wc);

	// ������ �� ����
	HWND hWnd = CreateWindow(L"D3D Study", L"D3D Study",
		WS_OVERLAPPEDWINDOW, 100, 100, 800, 800, NULL, NULL, wc.hInstance, NULL);

	// d3d �ʱ�ȭ
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		int grid = 1;
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

	UnregisterClass(L"D3D Study", wc.hInstance);
	return 0;
} // INT WINAPI wWinMain