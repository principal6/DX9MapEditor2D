#include "DX9Map.h"

DX9Map::DX9Map() {
	m_nRows = 0;
	m_nCols = 0;
	m_nSheetWidth = 0;
	m_nSheetHeight = 0;
}

int DX9Map::Create(LPDIRECT3DDEVICE9 pD3DDev) {
	// 멤버 변수 초기화
	m_pDevice = pD3DDev;

	m_pVB = NULL;
	m_pIB = NULL;
	m_pTexture = NULL;

	m_Vert.clear();
	m_Ind.clear();

	m_fX = 0.0f;
	m_fY = 0.0f;
	m_fScaleX = 1.0f;
	m_fScaleY = 1.0f;

	m_nWidth = 32;
	m_nHeight = 32;

	return 0;
}


int DX9Map::Destroy() {
	DX9Image::Destroy();
	return 0;
}

int DX9Map::SetTexture(std::wstring FileName) {
	DX9Image::SetTexture(FileName);
	
	return 0;
}

int DX9Map::SetTileInfo(int TileW, int TileH) {

	m_nSheetWidth = m_nWidth;
	m_nSheetHeight = m_nHeight;

	m_nWidth = TileW;
	m_nHeight = TileH;

	m_nRows = (int)(m_nSheetWidth / m_nWidth);
	m_nCols = (int)(m_nSheetHeight / m_nHeight);

	return 0;
}

int DX9Map::AddMapFragment(int TileID, float X, float Y) {
	int TileX = (TileID % m_nCols);
	int TileY = (TileID / m_nCols);

	float u1 = (float)(TileX * m_nWidth) / (float)m_nSheetWidth;
	float u2 = u1 + (float)m_nWidth / (float)m_nSheetWidth;
	float v1 = (float)(TileY * m_nHeight) / (float)m_nSheetHeight;
	float v2 = v1 + (float)m_nHeight / (float)m_nSheetHeight;

	m_Vert.push_back(DX9VERTEX(X, Y, 0, 1, 0xffffffff, u1, v1));
	m_Vert.push_back(DX9VERTEX(X + m_nWidth, Y, 0, 1, 0xffffffff, u2, v1));
	m_Vert.push_back(DX9VERTEX(X, Y + m_nHeight, 0, 1, 0xffffffff, u1, v2));
	m_Vert.push_back(DX9VERTEX(X + m_nWidth, Y + m_nHeight, 0, 1, 0xffffffff, u2, v2));
	m_nVertCount = (int)m_Vert.size();

	m_Ind.push_back(DX9INDEX(m_nVertCount - 4, m_nVertCount - 3, m_nVertCount - 1));
	m_Ind.push_back(DX9INDEX(m_nVertCount - 4, m_nVertCount - 1, m_nVertCount - 2));
	m_nIndCount = (int)m_Ind.size();

	return 0;
}

int DX9Map::AddEnd() {
	DX9Image::CreateVB();
	DX9Image::CreateIB();
	DX9Image::UpdateVB();

	return 0;
}

int DX9Map::Draw() {
	DX9Image::Draw();
	return 0;
}