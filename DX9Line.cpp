#include "DX9Line.h"

// Static member declaration
LPDIRECT3DDEVICE9 DX9Line::m_pDevice;

void DX9Line::Create(LPDIRECT3DDEVICE9 pD3DDev)
{
	m_pDevice = pD3DDev;
	m_pVB = nullptr;
	m_pIB = nullptr;
	Clear();
}

void DX9Line::Clear()
{
	m_Vert.clear();
	m_Ind.clear();
	m_VertCount = 0;
	m_IndCount = 0;
}

void DX9Line::CreateMax(LPDIRECT3DDEVICE9 pD3DDev)
{
	Create(pD3DDev);
	CreateVBMax();
	CreateIBMax();
}

void DX9Line::Destroy()
{
	m_pDevice = nullptr; // Just set to nullptr cuz it's newed in <DX9Base> class

	m_Vert.clear();
	m_Ind.clear();

	if (m_pIB)
	{
		m_pIB->Release();
		m_pIB = nullptr;
	}
		
	if (m_pVB)
	{
		m_pVB->Release();
		m_pVB = nullptr;
	}
}

void DX9Line::AddLine(D3DXVECTOR2 StartPos, D3DXVECTOR2 Length, DWORD Color)
{
	m_Vert.push_back(DX9VERTEX_LINE(StartPos.x, StartPos.y, Color));
	m_Vert.push_back(DX9VERTEX_LINE(StartPos.x + Length.x, StartPos.y + Length.y, Color));
	m_VertCount = (int)m_Vert.size();

	m_Ind.push_back(DX9INDEX2(m_IndCount * 2, m_IndCount * 2 + 1));
	m_IndCount = (int)m_Ind.size();
}

void DX9Line::AddBox(D3DXVECTOR2 StartPos, D3DXVECTOR2 Size, DWORD Color)
{
	AddLine(StartPos, D3DXVECTOR2(Size.x, 0), Color);
	AddLine(StartPos, D3DXVECTOR2(0, Size.y), Color);
	AddLine(D3DXVECTOR2(StartPos.x + Size.x, StartPos.y), D3DXVECTOR2(0, Size.y), Color);
	AddLine(D3DXVECTOR2(StartPos.x, StartPos.y + Size.y), D3DXVECTOR2(Size.x, 0), Color);
}

void DX9Line::AddEnd()
{
	CreateVB();
	CreateIB();
	UpdateVB();
	UpdateIB();
}

void DX9Line::SetBoxPosition(D3DXVECTOR2 StartPos, D3DXVECTOR2 Size)
{
	m_Vert[0].x = StartPos.x;
	m_Vert[0].y = StartPos.y;
	m_Vert[1].x = StartPos.x + Size.x;
	m_Vert[1].y = StartPos.y;

	m_Vert[2].x = StartPos.x;
	m_Vert[2].y = StartPos.y;
	m_Vert[3].x = StartPos.x;
	m_Vert[3].y = StartPos.y + Size.y;

	m_Vert[4].x = StartPos.x + Size.x;
	m_Vert[4].y = StartPos.y;
	m_Vert[5].x = StartPos.x + Size.x;
	m_Vert[5].y = StartPos.y + Size.y;

	m_Vert[6].x = StartPos.x;
	m_Vert[6].y = StartPos.y + Size.y;
	m_Vert[7].x = StartPos.x + Size.x;
	m_Vert[7].y = StartPos.y + Size.y;
	
	UpdateVB();
}

void DX9Line::CreateVB()
{
	if (m_pVB)
	{
		m_pVB->Release();
		m_pVB = nullptr;
	}
	int rVertSize = sizeof(DX9VERTEX_LINE) * m_VertCount;
	if (FAILED(m_pDevice->CreateVertexBuffer(rVertSize, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return;
}

void DX9Line::CreateIB()
{
	if (m_pIB)
	{
		m_pIB->Release();
		m_pIB = nullptr;
	}
	int rIndSize = sizeof(DX9INDEX2) * m_IndCount;
	if (FAILED(m_pDevice->CreateIndexBuffer(rIndSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return;
}

void DX9Line::CreateVBMax()
{
	if (m_pVB)
	{
		m_pVB->Release();
		m_pVB = nullptr;
	}
	int rVertSize = sizeof(DX9VERTEX_LINE) * MAX_UNIT_COUNT * 8;
	if (FAILED(m_pDevice->CreateVertexBuffer(rVertSize, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return;
}

void DX9Line::CreateIBMax()
{
	if (m_pIB)
	{
		m_pIB->Release();
		m_pIB = nullptr;
	}
	int rIndSize = sizeof(DX9INDEX2) * MAX_UNIT_COUNT * 4;
	if (FAILED(m_pDevice->CreateIndexBuffer(rIndSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return;
}

void DX9Line::UpdateVB()
{
	if (m_Vert.size() > 0)
	{
		int rVertSize = sizeof(DX9VERTEX_LINE) * m_VertCount;
		VOID* pVertices;
		if (FAILED(m_pVB->Lock(0, rVertSize, (void**)&pVertices, 0)))
			return;
		memcpy(pVertices, &m_Vert[0], rVertSize);
		m_pVB->Unlock();
	}
}

void DX9Line::UpdateIB()
{
	if (m_Ind.size() > 0)
	{
		int rIndSize = sizeof(DX9INDEX2) * m_IndCount;
		VOID* pIndices;
		if (FAILED(m_pIB->Lock(0, rIndSize, (void **)&pIndices, 0)))
			return;
		memcpy(pIndices, &m_Ind[0], rIndSize);
		m_pIB->Unlock();
	}
}

void DX9Line::Draw() const
{
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pDevice->SetTexture(0, nullptr);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(DX9VERTEX_LINE));
	m_pDevice->SetFVF(D3DFVF_LINE);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, m_VertCount, 0, m_IndCount);
}