#include "Core/DX9Image.h"
#include "JWWindow.h"
#include "DX9Base.h"
#include "DX9BaseME.h"
#include "DX9Map.h"
#include "DX9MapEditor.h"

const wchar_t *g_szCaption = L"DX9 2D 맵 에디터";
const wchar_t *g_szHelp = L"DirectX 9 2D 게임을 위한 맵 에디터입니다. <개발자: 김장원>";
const wchar_t *g_szMoveFN32 = L"move32.png";
const wchar_t *g_szMoveFN64 = L"move64.png";
const wchar_t *g_szTileSelFN = L"tilesel.png";
const int ALPHA_TILESEL = 150;
const int WNDSEP_X = 320;
const int WINDOW_W = 800;
const int WINDOW_H = 600;

JWWindow* g_myWND;
HWND g_hWnd;
HWND g_hChildL;
HWND g_hChildR;
HWND g_hScrLH;
HWND g_hScrLV;
HWND g_hScrRH;
HWND g_hScrRV;

int g_nLScrollXPos;
int g_nLScrollYPos;
int g_nRScrollXPos;
int g_nRScrollYPos;

bool g_MapMouseLBDown;
bool g_MapMouseRBDown;
bool g_TileMouseLBDown;
bool g_TileMouseRBDown;
int g_nMouseXStart;
int g_nMouseYStart;

DX9MapEditor* myEditor;
DX9BaseME* g_DX9Left;
DX9BaseME* g_DX9Right;
DX9Image* g_ImgTile;
DX9Image* g_ImgTileSel;
DX9Map* g_DX9Map;
DX9Image* g_ImgMapSel;
DX9Image* g_ImgMapBG;

std::wstring g_strMapName;
std::wstring g_strTileName;
int g_nTileCols = 0;
int g_nTileRows = 0;
int g_nCurrTileID = 0;
int g_nCurrTileX = 0;
int g_nCurrTileY = 0;
bool g_bMultiSel = false;
int g_nMSRangeX = 0;
int g_nMSRangeY = 0;
DX9Common::MapMode g_nMode = DX9Common::MapMode::TileMode;

int MapSetter(int ID, int MouseX, int MouseY);
int TileSetter(int MouseX, int MouseY, int RangeX = 0, int RangeY = 0);
int LoadTile(std::wstring TileName);
void SetToTileMode();
void SetToMoveMode();
void MainLoop();
int OnScrollbarChanged();
int AdjustScrollbars();
int HandleAccelAndMenu(WPARAM wParam);
int UpdateWindowCaption(int MapCols, int MapRows);
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcL(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcR(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcNewMap(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

int main()
{
	myEditor = new DX9MapEditor;
	myEditor->Create(WINDOW_W, WINDOW_H);

	g_myWND = new JWWindow;
	g_hWnd = g_myWND->Create(g_szCaption, 50, 50, WINDOW_W, WINDOW_H, RGB(255, 255, 255), WndProc, WS_OVERLAPPEDWINDOW);
	RECT tRect;
	GetClientRect(g_hWnd, &tRect);
	g_hChildL = g_myWND->AddChild(g_hWnd, L"ChL", 0, 0, WNDSEP_X, tRect.bottom, RGB(230, 230, 230), WndProcL);
	g_hChildR = g_myWND->AddChild(g_hWnd, L"ChR", WNDSEP_X, 0, tRect.right - WNDSEP_X, tRect.bottom, RGB(150, 150, 150), WndProcR);
	g_hScrLH = g_myWND->AddScrollbarH(g_hChildL, 0, 10);
	g_hScrLV = g_myWND->AddScrollbarV(g_hChildL, 0, 10);
	g_hScrRH = g_myWND->AddScrollbarH(g_hChildR, 0, 10);
	g_hScrRV = g_myWND->AddScrollbarV(g_hChildR, 0, 10);

	g_DX9Left = new DX9BaseME;
	g_DX9Left->CreateOnWindow(g_hChildL);
	g_DX9Left->SetBackgroundColor(D3DCOLOR_XRGB(150, 50, 250));

	g_ImgTile = new DX9Image;
	g_ImgTile->Create(g_DX9Left->GetDevice());
	g_ImgTile->SetSize(0, 0);

	g_ImgTileSel = new DX9Image;
	g_ImgTileSel->Create(g_DX9Left->GetDevice());
	g_ImgTileSel->SetTexture(g_szTileSelFN);
	g_ImgTileSel->SetAlpha(ALPHA_TILESEL);
	g_ImgTileSel->SetSize(myEditor->GetTileSize(), myEditor->GetTileSize());

	g_DX9Right = new DX9BaseME;
	g_DX9Right->CreateOnWindow(g_hChildR);
	g_DX9Right->SetBackgroundColor(D3DCOLOR_XRGB(50, 50, 50));

	g_DX9Map = new DX9Map;
	g_DX9Map->Create(g_DX9Right->GetDevice());
	g_DX9Map->SetMoveTexture(g_szMoveFN32);

	g_ImgMapSel = new DX9Image;
	g_ImgMapSel->Create(g_DX9Right->GetDevice());
	g_ImgMapSel->SetAlpha(ALPHA_TILESEL);

	g_ImgMapBG = new DX9Image;
	g_ImgMapBG->Create(g_DX9Right->GetDevice());
	g_ImgMapBG->SetTexture(g_szTileSelFN);

	// 프로그램 실행
	g_DX9Left->SetMainLoop(MainLoop);
	g_DX9Left->RunWithAccel(g_myWND->GethAccel());

	// 종료
	g_DX9Left->Destroy();
	g_DX9Right->Destroy();
	g_ImgTile->Destroy();
	g_ImgTileSel->Destroy();
	g_DX9Map->Destroy();
	g_ImgMapSel->Destroy();

	delete g_DX9Left;
	delete g_DX9Right;
	delete g_ImgTile;
	delete g_ImgTileSel;
	delete g_DX9Map;
	delete g_ImgMapSel;

	return 0;
}

void MainLoop()
{
	g_DX9Left->BeginRender();

		g_ImgTile->Draw();

		if (g_ImgTile->IsTextureLoaded())
			g_ImgTileSel->Draw();

	g_DX9Left->EndRender();

	g_DX9Right->BeginRender();

		if (g_DX9Map->IsMapCreated())
		{
			if (g_nMode == DX9Common::MapMode::TileMode)
				g_ImgMapBG->Draw();

			g_DX9Map->Draw();
			g_ImgMapSel->Draw();
		}			

	g_DX9Right->EndRender();
	return;
}

int LoadTile(std::wstring TileName)
{
	// Load tile texture
	g_ImgTile->SetTexture(TileName);

	// Set tile selector size
	g_ImgTileSel->SetSize(myEditor->GetTileSize(), myEditor->GetTileSize());

	// Set tile columns and rows count
	g_nTileCols = (int)(g_ImgTile->GetWidth() / myEditor->GetTileSize());
	g_nTileRows = (int)(g_ImgTile->GetHeight() / myEditor->GetTileSize());
	float u2 = 1.0f / (float)g_nTileCols;
	float v2 = 1.0f / (float)g_nTileRows;

	// Set tile selector position
	TileSetter(0, 0);

	// Set map selector
	g_ImgMapSel->SetTexture(TileName);
	g_ImgMapSel->SetSize(myEditor->GetTileSize(), myEditor->GetTileSize());
	g_ImgMapSel->SetUVRange(0, u2, 0, v2);
	g_ImgMapSel->SetAlpha(ALPHA_TILESEL);

	return 0;
}

void SetToTileMode()
{
	if ((g_DX9Map->IsMapCreated()) && (g_nMode != DX9Common::MapMode::TileMode))
	{
		g_nMode = DX9Common::MapMode::TileMode;
		LoadTile(g_strTileName);
		g_DX9Map->SetMode(g_nMode);
		AdjustScrollbars();
		TileSetter(0, 0);
	}
}

void SetToMoveMode()
{
	if ((g_DX9Map->IsMapCreated()) && (g_nMode != DX9Common::MapMode::MoveMode))
	{
		g_nMode = DX9Common::MapMode::MoveMode;
		switch (myEditor->GetTileSize())
		{
		case 32:
			LoadTile(g_szMoveFN32);
			g_DX9Map->SetMoveTexture(g_szMoveFN32);
			break;
		case 64:
			LoadTile(g_szMoveFN64);
			g_DX9Map->SetMoveTexture(g_szMoveFN64);
			break;
		}
		g_DX9Map->SetMode(g_nMode);
		AdjustScrollbars();
		TileSetter(0, 0);
	}
}

int HandleAccelAndMenu(WPARAM wParam)
{
	std::wstring tStr;
	wchar_t tWC[255] = { 0 };

	switch (LOWORD(wParam)) { // wParam의 low word에 resource id값이 날라온다.
	case ID_ACCELERATOR40007:
	case ID_FILE_NEW:
		// 맵 만들기
		DialogBox(g_myWND->GethInstance(), MAKEINTRESOURCE(IDD_DIALOG1), g_hWnd, DlgProcNewMap);
		SetToTileMode();
		break;
	case ID_ACCELERATOR40009:
	case ID_FILE_OPEN:
		// 맵 불러오기★
		if (g_myWND->OpenFileDlg(L"맵 파일\0*.jwm\0") == TRUE)
		{
			g_DX9Map->LoadMapFromFile(g_myWND->GetDlgFileTitle());

			g_DX9Map->GetTileName(&g_strTileName);
			g_DX9Map->GetMapName(&g_strMapName);
			LoadTile(g_strTileName);
			
			g_ImgMapBG->SetSize(g_DX9Map->GetWidth(), g_DX9Map->GetHeight());
			UpdateWindowCaption(g_DX9Map->GetMapCols(), g_DX9Map->GetMapRows());
			AdjustScrollbars();

			SetToTileMode();
		}
		break;
	case ID_ACCELERATOR40013:
	case ID_FILE_SAVE:
		// 맵 저장하기
		if (g_myWND->SaveFileDlg(L"모든 파일\0*.*\0") == TRUE)
		{
			if (g_DX9Map->IsMapCreated())
			{
				g_DX9Map->GetMapData(&tStr);
				g_myWND->SetFileText(tStr);
				g_myWND->SaveFileText(g_myWND->GetDlgFileName());
			}
		}
		break;
	case ID_ACCELERATOR40021:
	case ID_MODE_TILEMODE:
		// 타일 모드
		SetToTileMode();
		break;
	case ID_ACCELERATOR40023:
	case ID_MODE_MOVEMODE:
		// 무브 모드
		SetToMoveMode();
		break;
	case ID_ACCELERATOR40011:
	case ID_HELP_INFO:
		MessageBox(g_hWnd, g_szHelp, TEXT("프로그램 정보"), MB_OK);
		break;
	}

	return 0;
}

int UpdateWindowCaption(int MapCols, int MapRows)
{
	std::wstring tStr;
	wchar_t tWC[255] = { 0 };

	tStr = g_szCaption;
	tStr += L" <맵 이름: ";
	tStr += g_strMapName;
	tStr += L"> <크기: ";
	_itow_s(MapCols, tWC, 10);
	tStr += tWC;
	tStr += L"x";
	_itow_s(MapRows, tWC, 10);
	tStr += tWC;
	tStr += L">";
	SetWindowText(g_hWnd, tStr.c_str());

	return 0;
}

int TileSetter(int MouseX, int MouseY, int RangeX, int RangeY)
{
	if (g_ImgTile->IsTextureLoaded())
	{
		int tTileX = (int)(MouseX / myEditor->GetTileSize());
		int tTileY = (int)(MouseY / myEditor->GetTileSize());

		int tCols = g_nTileCols;
		int tRows = g_nTileRows;

		tTileX = min(tTileX, tCols - 1);
		tTileY = min(tTileY, tRows - 1);

		g_ImgTileSel->SetSize(myEditor->GetTileSize(), myEditor->GetTileSize());
		g_ImgTileSel->SetPosition(D3DXVECTOR2((float)(tTileX * myEditor->GetTileSize()), (float)(tTileY * myEditor->GetTileSize())));

		if ((RangeX > 1) || (RangeY > 1))
		{
			g_bMultiSel = true;
			g_ImgTileSel->SetSize(myEditor->GetTileSize() * RangeX, myEditor->GetTileSize() * RangeY);
		}

		tTileX = tTileX + g_nLScrollXPos;
		tTileY = tTileY + g_nLScrollYPos;

		tTileX = min(tTileX, tCols - 1);
		tTileY = min(tTileY, tRows - 1);

		g_nCurrTileID = tTileX + (tTileY * tCols);
		g_nCurrTileX = tTileX;
		g_nCurrTileY = tTileY;

		if (RangeX == 0) RangeX = 1;
		if (RangeY == 0) RangeY = 1;

		int sizeX = RangeX * myEditor->GetTileSize();
		int sizeY = RangeY * myEditor->GetTileSize();
		float u1 = (float)tTileX / (float)tCols;
		float u2 = (float)(tTileX + RangeX) / (float)tCols;
		float v1 = (float)tTileY / (float)tRows;
		float v2 = (float)(tTileY + RangeY) / (float)tRows;

		g_ImgMapSel->SetSize(sizeX, sizeY);
		g_ImgMapSel->SetUVRange(u1, u2, v1, v2);

		return 0;
	}

	return -1;
}

int MapSetter(int ID, int MouseX, int MouseY)
{
	if (g_ImgTile->IsTextureLoaded())
	{
		int tMapX = (int)(MouseX / myEditor->GetTileSize()) + g_nRScrollXPos;
		int tMapY = (int)(MouseY / myEditor->GetTileSize()) + g_nRScrollYPos;

		int NewID = ID;

		if (g_bMultiSel)
		{	
			for (int i = 0; i < g_nMSRangeX; i++)
			{
				for (int j = 0; j < g_nMSRangeY; j++)
				{
					switch (g_nMode)
					{
					case DX9Common::MapMode::TileMode:
						if (NewID != -1)
							NewID = ID + i + (j * g_nTileCols);

						g_DX9Map->SetMapFragmentTile(NewID, tMapX + i, tMapY + j);
						break;
					case DX9Common::MapMode::MoveMode:
						if (NewID == -1)
						{
							NewID = 0;
						}
						else
						{
							NewID = ID + i + (j * g_nTileCols);
						}
						g_DX9Map->SetMapFragmentMove(NewID, tMapX + i, tMapY + j);
						break;
					default:
						return -1;
					}
				}
			}
		}
		else
		{
			switch (g_nMode)
			{
			case DX9Common::MapMode::TileMode:
				g_DX9Map->SetMapFragmentTile(NewID, tMapX, tMapY);
				break;
			case DX9Common::MapMode::MoveMode:
				if (NewID == -1)
					NewID = 0;
				g_DX9Map->SetMapFragmentMove(NewID, tMapX, tMapY);
				break;
			default:
				return -1;
			}
		}

		return 0;
	}

	return -1;
}

int AdjustScrollbars()
{
	RECT tRect;
	g_myWND->MoveScrollbarH(g_hChildL, g_hScrLH);
	g_myWND->MoveScrollbarV(g_hChildL, g_hScrLV);
	g_myWND->MoveScrollbarH(g_hChildR, g_hScrRH);
	g_myWND->MoveScrollbarV(g_hChildR, g_hScrRV);

	if (g_DX9Map)
	{
		if (g_DX9Map->IsMapCreated())
		{
			// 타일
			GetClientRect(g_hChildL, &tRect);

			int nCurrTileMaxRows = (int)(tRect.bottom / myEditor->GetTileSize());
			int nTileRestRows = g_nTileRows - nCurrTileMaxRows;
			nTileRestRows = max(0, nTileRestRows);
			g_myWND->SetScrollbar(g_hScrLV, 0, nTileRestRows, g_nTileRows);

			int nCurrTileMaxCols = (int)(tRect.right / myEditor->GetTileSize()) - 1;
			int nTileRestCols = g_nTileCols - nCurrTileMaxCols;
			nTileRestCols = max(0, nTileRestCols);
			g_myWND->SetScrollbar(g_hScrLH, 0, nTileRestCols, g_nTileCols);

			// 맵
			GetClientRect(g_hChildR, &tRect);

			int nCurrMapMaxRows = (int)(tRect.bottom / myEditor->GetTileSize()) - 1;
			int nMapRows = g_DX9Map->GetMapRows();
			int nMapRestRows = nMapRows - nCurrMapMaxRows;
			nMapRestRows = max(0, nMapRestRows);
			g_myWND->SetScrollbar(g_hScrRV, 0, nMapRestRows, nMapRows);

			int nCurrMapMaxCols = (int)(tRect.right / myEditor->GetTileSize()) - 1;
			int nMapCols = g_DX9Map->GetMapCols();
			int nMapRestCols = nMapCols - nCurrMapMaxCols;
			nMapRestCols = max(0, nMapRestCols);
			g_myWND->SetScrollbar(g_hScrRH, 0, nMapRestCols, nMapCols);
		}
	}
	
	OnScrollbarChanged();

	return 0;
}

int OnScrollbarChanged()
{
	g_nLScrollXPos = GetScrollPos(g_hScrLH, SB_CTL);
	g_nLScrollYPos = GetScrollPos(g_hScrLV, SB_CTL);

	D3DXVECTOR2 tOffset = D3DXVECTOR2(0, 0);

	if (g_ImgTile)
	{
		if (g_ImgTile->IsTextureLoaded())
		{
			tOffset.x = (float)(-g_nLScrollXPos * myEditor->GetTileSize());
			tOffset.y = (float)(-g_nLScrollYPos * myEditor->GetTileSize());
			g_ImgTile->SetPosition(tOffset);

			tOffset.x = (float)((g_nCurrTileX - g_nLScrollXPos) * myEditor->GetTileSize());
			tOffset.y = (float)((g_nCurrTileY - g_nLScrollYPos) * myEditor->GetTileSize());
			g_ImgTileSel->SetPosition(tOffset);

			g_nRScrollXPos = GetScrollPos(g_hScrRH, SB_CTL);
			g_nRScrollYPos = GetScrollPos(g_hScrRV, SB_CTL);

			if (g_DX9Map)
			{
				if (g_DX9Map->IsMapCreated())
				{
					tOffset.x = (float)(-g_nRScrollXPos * myEditor->GetTileSize());
					tOffset.y = (float)(-g_nRScrollYPos * myEditor->GetTileSize());
					g_DX9Map->SetPosition(tOffset);
					g_ImgMapBG->SetPosition(tOffset);
				}
			}
		}
	}
	
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	g_myWND->BaseProc(hWnd, Message, wParam, lParam, OnScrollbarChanged);

	RECT tRect;
	
	switch (Message)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		HandleAccelAndMenu(wParam);
		break;
	case WM_SIZE:
		// 윈도우 크기 조절 시!
		GetClientRect(hWnd, &tRect);
		MoveWindow(g_hChildR, WNDSEP_X, 0, tRect.right - WNDSEP_X, tRect.bottom, TRUE);
		MoveWindow(g_hChildL, 0, 0, WNDSEP_X, tRect.bottom, TRUE);

		if (g_DX9Left)
			g_DX9Left->Resize(g_hChildL);

		if (g_DX9Right)
			g_DX9Right->Resize(g_hChildR);
		
		AdjustScrollbars();

		break;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

LRESULT CALLBACK WndProcL(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	g_myWND->BaseProc(hWnd, Message, wParam, lParam, OnScrollbarChanged);

	int tMouseX = LOWORD(lParam);
	int tMouseY = HIWORD(lParam);

	switch (Message)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		HandleAccelAndMenu(wParam);
		break;
	case WM_MOUSEWHEEL:
		if ((short)HIWORD(wParam) == WHEEL_DELTA) {
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, (LPARAM)g_hScrLV);
		}
		else {
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, (LPARAM)g_hScrLV);
		}
		break;
	case WM_LBUTTONDOWN:
		g_bMultiSel = false;
		g_TileMouseLBDown = true;
		TileSetter(tMouseX, tMouseY);
		g_nMouseXStart = tMouseX;
		g_nMouseYStart = tMouseY;
		break;
	case WM_LBUTTONUP:
		g_TileMouseLBDown = false;
		break;
	case WM_RBUTTONDOWN:
		g_TileMouseRBDown = true;
		break;
	case WM_RBUTTONUP:
		g_TileMouseRBDown = false;
		break;
	case WM_MOUSEMOVE:
		if (g_ImgTile)
		{
			if (g_ImgTile->IsTextureLoaded())
			{
				if (g_TileMouseLBDown)
				{
					// 타일 다중 선택
					g_nMSRangeX = (int)((tMouseX - g_nMouseXStart) / myEditor->GetTileSize()) + 1;
					g_nMSRangeY = (int)((tMouseY - g_nMouseYStart) / myEditor->GetTileSize()) + 1;

					TileSetter(g_nMouseXStart, g_nMouseYStart, g_nMSRangeX, g_nMSRangeY);
				}
			}
		}
		if (g_ImgMapSel)
		{
			g_ImgMapSel->SetAlpha(0);
		}
		break;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

LRESULT CALLBACK WndProcR(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	g_myWND->BaseProc(hWnd, Message, wParam, lParam, OnScrollbarChanged);

	int tMouseX = LOWORD(lParam);
	int tMouseY = HIWORD(lParam);
	
	float tMapSelX = 0.0f;
	float tMapSelY = 0.0f;

	switch (Message)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		HandleAccelAndMenu(wParam);
		break;
	case WM_MOUSEWHEEL:
		if ((short)HIWORD(wParam) == WHEEL_DELTA) {
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, (LPARAM)g_hScrRV);
		}
		else {
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, (LPARAM)g_hScrRV);
		}
		break;
	case WM_LBUTTONDOWN:
		g_MapMouseLBDown = true;
		MapSetter(g_nCurrTileID, tMouseX, tMouseY);
		break;
	case WM_LBUTTONUP:
		g_MapMouseLBDown = false;
		break;
	case WM_RBUTTONDOWN:
		g_MapMouseRBDown = true;
		MapSetter(-1, tMouseX, tMouseY);
		break;
	case WM_RBUTTONUP:
		g_MapMouseRBDown = false;
		break;
	case WM_MOUSEMOVE:
		if (g_MapMouseLBDown)
			MapSetter(g_nCurrTileID, tMouseX, tMouseY);
		if (g_MapMouseRBDown)
			MapSetter(-1, tMouseX, tMouseY);
		if (g_ImgMapSel)
		{
			g_ImgMapSel->SetAlpha(ALPHA_TILESEL);
			D3DXVECTOR2 tMapSel;
			tMapSel.x = (float)((tMouseX / myEditor->GetTileSize()) * myEditor->GetTileSize());
			tMapSel.y = (float)((tMouseY / myEditor->GetTileSize()) * myEditor->GetTileSize());
			g_ImgMapSel->SetPosition(tMapSel);
		}
		break;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

LRESULT CALLBACK DlgProcNewMap(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	wchar_t tWC[255] = { 0 };
	WSTRING tStr;
	size_t tFind;
	int tMapCols = 0;
	int tMapRows = 0;
	int tTileSize = 0;

	WSTRING Ratio[4] = { L"x32", L"x64" };
	HWND tempHWND;

	switch (iMessage)
	{
	case WM_INITDIALOG:
		tempHWND = (HWND)GetDlgItem(hDlg, IDC_COMBO1);

		for (int i = 0; i < 4; i ++)
		{
			// Send a CB_ADDSTRING message to load each item (Add string to combobox.)
			SendMessage(tempHWND, CB_ADDSTRING, (WPARAM)0, (LPARAM)Ratio[i].c_str());
		}

		// Send the CB_SETCURSEL message to display an initial item in the selection field
		SendMessage(tempHWND, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		PostMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hDlg, IDC_EDIT1), TRUE);
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_BUTTON1:
			if (g_myWND->OpenFileDlg(L"모든 파일\0*.*\0") == TRUE)
			{
				// 타일 이름 얻기
				g_strTileName = g_myWND->GetDlgFileName().c_str();
				tFind = g_strTileName.find_last_of('\\');
				if (tFind)
					g_strTileName = g_strTileName.substr(tFind + 1);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT4), g_strTileName.c_str());
			}
			break;
		case IDC_COMBO1:

			break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT4, tWC, 255);
			tFind = wcslen(tWC);
			
			if (tFind)
			{
				GetDlgItemText(hDlg, IDC_EDIT1, tWC, 255);
				g_strMapName = tWC;

				tMapCols = GetDlgItemInt(hDlg, IDC_EDIT2, FALSE, FALSE);
				tMapRows = GetDlgItemInt(hDlg, IDC_EDIT3, FALSE, FALSE);

				GetDlgItemText(hDlg, IDC_COMBO1, tWC, 255);
				tStr = tWC;
				tStr = tStr.substr(1);
				tTileSize = _wtoi(tStr.c_str());
				myEditor->SetTileSize(tTileSize);
				
				if (tMapCols && tMapRows)
				{
					// 타일 불러오기
					LoadTile(g_strTileName);
					g_DX9Map->SetTileTexture(g_strTileName);
					switch (tTileSize)
					{
					case 32:
						g_DX9Map->SetMoveTexture(g_szMoveFN32);
						break;
					case 64:
						g_DX9Map->SetMoveTexture(g_szMoveFN64);
						break;
					}

					// 맵 만들기
					g_DX9Map->CreateNewMap(tWC, tMapCols, tMapRows, tTileSize);

					g_ImgMapBG->SetSize(g_DX9Map->GetWidth(), g_DX9Map->GetHeight());
					UpdateWindowCaption(tMapCols, tMapRows);
					AdjustScrollbars();
				}
			}
		case IDCANCEL:
			EndDialog(hDlg, 0);
			break;
		}
		return TRUE;
	}
	return FALSE;
}