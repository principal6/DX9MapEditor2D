#include "DX9Common.h"

const float DX9Common::UV_OFFSET = 0.005f;
const wchar_t DX9Common::ASSET_DIR[] = L"\\Asset\\";
HINSTANCE DX9Common::m_hInstance = nullptr;
HWND DX9Common::m_hWnd = nullptr;
WindowData DX9Common::m_WindowData;

void DX9Common::ConvertFrameIDIntoUV(int FrameID, int NumCols, int NumRows, FloatUV* UV)
{
	UV->u1 = static_cast<float>(FrameID % NumCols) / static_cast<float>(NumCols);
	UV->u2 = UV->u1 + (1.0f / static_cast<float>(NumCols));
	UV->v1 = static_cast<float>(FrameID / NumCols) / static_cast<float>(NumRows);
	UV->v2 = UV->v1 + (1.0f / static_cast<float>(NumRows));
}

void DX9Common::GetTileCols(int SheetWidth, int TileWidth, int* TileCols)
{
	*TileCols = static_cast<int>(SheetWidth / TileWidth);
}

void DX9Common::GetTileRows(int SheetHeight, int TileHeight, int* TileRows)
{
	*TileRows = static_cast<int>(SheetHeight / TileHeight);
}