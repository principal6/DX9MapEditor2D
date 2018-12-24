#pragma once

#ifndef DX9MAP_H
#define DX9MAP_H

#include "DX9Image.h"

const int MAX_TILEID_LEN = 3;
const int MAX_MOVEID_LEN = 2;

const int TILE_W = 32;
const int TILE_H = 32;

const int MOVE_ALPHA = 100;

struct DXUV {
	float u1;
	float u2;
	float v1;
	float v2;
	DXUV() : u1(0), u2(0), v1(0), v2(0) {};
	DXUV(float U1, float U2, float V1, float V2) : u1(U1), u2(U2), v1(V1), v2(V2) {};
};

struct DXMAPDATA {
	int TileID;
	int MoveID;
	DXMAPDATA(int TILEID, int MOVEID) : TileID(TILEID), MoveID(MOVEID) {};
};

class DX9Map : protected DX9Image {
private:
	int m_nMapCols;
	int m_nMapRows;
	int	m_nTileSheetWidth;
	int	m_nTileSheetHeight;
	int	m_nMoveSheetWidth;
	int	m_nMoveSheetHeight;
	std::wstring m_strMapName;
	std::wstring m_strTileName;
	std::wstring m_strLoadedMapTiles;
	std::vector<DXMAPDATA> m_MapData;
	bool m_bMapCreated;

	LPDIRECT3DTEXTURE9 m_pTextureMove;
	LPDIRECT3DVERTEXBUFFER9 m_pVBMove;
	std::vector<DX9VERTEX>	m_VertMove;
	int						m_nVertMoveCount;

	DX9MAPMODE m_CurrMapMode;

private:
	int DX9Map::GetMapDataPart(int DataID, wchar_t *WC, int size);
	DXUV DX9Map::ConvertIDtoUV(int ID, int SheetW, int SheetH);
	int DX9Map::CreateVBMove();
	int DX9Map::UpdateVBMove();

public:
	DX9Map();
	~DX9Map() {};

	int DX9Map::Create(LPDIRECT3DDEVICE9 pD3DDev, std::wstring BaseDir);
	int DX9Map::Destroy();
	int DX9Map::SetTileTexture(std::wstring FileName);
	int DX9Map::SetMoveTexture(std::wstring FileName);

	int DX9Map::CreateMap(std::wstring Name, int MapCols, int MapRows);
	int DX9Map::CreateMapWithData();
	int DX9Map::AddMapFragmentTile(int ID, int X, int Y);
	int DX9Map::AddMapFragmentMove(int ID, int X, int Y);
	int DX9Map::AddEnd();
	int DX9Map::SetMode(DX9MAPMODE Mode);
	int DX9Map::SetMapFragmentTile(int TileID, int X, int Y);
	int DX9Map::SetMapFragmentMove(int MoveID, int X, int Y);
	int DX9Map::SetPosition(float OffsetX, float OffsetY);

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