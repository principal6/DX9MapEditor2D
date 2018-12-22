#pragma once

#ifndef DX9MAP_H
#define DX9MAP_H

#include "DX9Image.h"

class DX9Map : protected DX9Image {
private:
	int	m_nTileCols;
	int	m_nTileRows;
	int	m_nTileSheetWidth;
	int	m_nTileSheetHeight;
	int m_nMapCols;
	int m_nMapRows;
	std::wstring m_strMapName;
	std::wstring m_strTileName;
	std::wstring m_strLoadedMapTiles;
	std::vector<int> m_arrMap;
	bool m_bMapCreated;

private:
	int DX9Map::GetMapDataPart(int DataID, wchar_t *WC, int size);

public:
	DX9Map();
	~DX9Map() {};

	int DX9Map::Create(LPDIRECT3DDEVICE9 pD3DDev, std::wstring BaseDir);
	int DX9Map::Destroy();
	int DX9Map::SetTexture(std::wstring FileName);
	int DX9Map::SetTileInfo(std::wstring Name, int TileW, int TileH);
	int DX9Map::SetPosition(float OffsetX, float OffsetY);

	int DX9Map::CreateMap(std::wstring Name, int MapCols, int MapRows);
	int DX9Map::CreateMapWithData();
	int DX9Map::AddMapFragment(int TileID, int X, int Y);
	int DX9Map::AddEnd();
	int DX9Map::SetMapFragment(int TileID, int X, int Y);

	int DX9Map::Draw();
	bool DX9Map::IsMapCreated() { return m_bMapCreated; };

	int DX9Map::GetMapData(std::wstring *pStr);
	int DX9Map::SetMapData(std::wstring Str);

	int DX9Map::GetMapName(std::wstring *pStr);
	int DX9Map::GetTileName(std::wstring *pStr);
	int DX9Map::GetMapCols() { return m_nMapCols; };
	int DX9Map::GetMapRows() { return m_nMapRows; };
};


#endif