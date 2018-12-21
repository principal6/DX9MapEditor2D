#pragma once

#ifndef DX9MAP_H
#define DX9MAP_H

#include "DX9Image.h"

class DX9Map : protected DX9Image {
private:
	int	m_nRows;
	int	m_nCols;
	int	m_nSheetWidth;
	int	m_nSheetHeight;

public:
	DX9Map();
	~DX9Map() {};

	int DX9Map::Create(LPDIRECT3DDEVICE9 pD3DDev);
	int DX9Map::Destroy();
	int DX9Map::SetTexture(std::wstring FileName);
	int DX9Map::SetTileInfo(int TileW, int TileH);
	int DX9Map::AddMapFragment(int TileID, float X, float Y);
	int DX9Map::AddEnd();
	int DX9Map::Draw();

	// 나중에 LoadTileMap, DrawTileMap 추가하자!
};


#endif