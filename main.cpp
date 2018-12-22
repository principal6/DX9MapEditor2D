#include "JWWindow.h"
#include "DX9Base.h"
#include "DX9Image.h"
#include "DX9Map.h"

const wchar_t *g_Caption = L"DX9 2D 맵 에디터";
const wchar_t *g_szHelp = L"DirectX 9 2D 게임을 위한 맵 에디터입니다. <개발자: 김장원>";
const int g_TileW = 32;
const int g_TileH = 32;
const int g_Alpha = 150;
JWWindow g_myWND;
HWND g_hWnd;
HWND g_hChildL;
HWND g_hChildR;
HWND g_hScrLH;
HWND g_hScrLV;
HWND g_hScrRH;
HWND g_hScrRV;
int g_WndSepX = 224;
DX9Base* g_DX9Left;
DX9Base* g_DX9Right;
DX9Image* g_ImgTile;
DX9Image* g_ImgTileSel;
DX9Image* g_ImgMapSel;
DX9Map* g_DX9Map;

// 마우스
bool g_MapMouseLBDown;
bool g_MapMouseRBDown;
bool g_TileMouseLBDown;
bool g_TileMouseRBDown;
int g_nTileXStart;
int g_nTileYStart;

// 맵, 타일 정보
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

// 스크롤바
int g_nLScrollXPos;
int g_nLScrollYPos;
int g_nRScrollXPos;
int g_nRScrollYPos;


int MapSetter(int TileID, int MouseX, int MouseY);
int TileSetter(int MouseX, int MouseY, int RangeX = 0, int RangeY = 0);
int LoadTile(std::wstring TileName);

// Window 관련 함수
int MainLoop();
int OnScrollbarChanged();
int AdjustScrollbars();
int HandleAccelAndMenu(WPARAM wParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcL(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcR(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcNewMap(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

int main() {
	// 초기화
	wchar_t tBaseDir[255] = { 0 };
	GetCurrentDirectory(255, tBaseDir);

	JWWindow g_myWND;
	g_hWnd = g_myWND.Create(g_Caption, 50, 50, 800, 600, RGB(255, 255, 255), WndProc, WS_OVERLAPPEDWINDOW);
	RECT tRect;
	GetClientRect(g_hWnd, &tRect);
	g_hChildL = g_myWND.AddChild(g_hWnd, L"ChL", 0, 0, g_WndSepX, tRect.bottom, RGB(230, 230, 230), WndProcL);
	g_hChildR = g_myWND.AddChild(g_hWnd, L"ChR", g_WndSepX, 0, tRect.right - g_WndSepX, tRect.bottom, RGB(150, 150, 150), WndProcR);
	g_hScrLH = g_myWND.AddScrollbarH(g_hChildL, 0, 10);
	g_hScrLV = g_myWND.AddScrollbarV(g_hChildL, 0, 10);
	g_hScrRH = g_myWND.AddScrollbarH(g_hChildR, 0, 10);
	g_hScrRV = g_myWND.AddScrollbarV(g_hChildR, 0, 10);

	g_DX9Left = new DX9Base;
	g_DX9Left->CreateOnWindow(g_hChildL);
	g_DX9Left->SetBGColor(D3DCOLOR_XRGB(50, 50, 250));

	g_DX9Right = new DX9Base;
	g_DX9Right->CreateOnWindow(g_hChildR);
	g_DX9Right->SetBGColor(D3DCOLOR_XRGB(50, 50, 50));

	g_ImgTile = new DX9Image;
	g_ImgTile->Create(g_DX9Left->GetDevice(), tBaseDir);

	g_ImgTileSel = new DX9Image;
	g_ImgTileSel->Create(g_DX9Left->GetDevice(), tBaseDir);
	g_ImgTileSel->SetTexture(L"tilesel32x32.png");
	g_ImgTileSel->SetAlpha(g_Alpha);

	g_DX9Map = new DX9Map;
	g_DX9Map->Create(g_DX9Right->GetDevice(), tBaseDir);

	g_ImgMapSel = new DX9Image;
	g_ImgMapSel->Create(g_DX9Right->GetDevice(), tBaseDir);
	g_ImgMapSel->SetAlpha(g_Alpha);

	// 프로그램 실행
	g_DX9Left->RunWithAccel(MainLoop, g_myWND.GethAccel());

	// 종료
	g_DX9Left->Destroy();
	g_DX9Right->Destroy();
	g_ImgTile->Destroy();
	g_ImgTileSel->Destroy();
	g_DX9Map->Destroy();

	delete g_DX9Left;
	delete g_DX9Right;
	delete g_ImgTile;
	delete g_ImgTileSel;
	delete g_DX9Map;
}

int MainLoop() {
	g_DX9Left->BeginRender();

		g_ImgTile->Draw();

		if (g_ImgTile->IsTextureLoaded())
			g_ImgTileSel->Draw();

	g_DX9Left->EndRender();

	g_DX9Right->BeginRender();

		if (g_DX9Map->IsMapCreated())
		{
			g_DX9Map->Draw();
			g_ImgMapSel->Draw();
		}			

	g_DX9Right->EndRender();
	return 0;
}

int LoadTile(std::wstring TileName) {
	int tTW, tTH;
	g_ImgTile->SetTexture(TileName);
	g_DX9Map->SetTexture(TileName);
	g_DX9Map->SetTileInfo(TileName, g_TileW, g_TileH);
	g_ImgMapSel->SetTexture(TileName);
	g_ImgMapSel->SetSize(0, 0);
	g_ImgMapSel->SetAlpha(g_Alpha);

	tTW = g_ImgTile->GetWidth();
	tTH = g_ImgTile->GetHeight();
	g_nTileCols = (int)(tTW / g_TileW);
	g_nTileRows = (int)(tTH / g_TileH);

	return 0;
}

int HandleAccelAndMenu(WPARAM wParam) {
	std::wstring tStr;
	wchar_t tWC[255] = { 0 };

	switch (LOWORD(wParam)) { // wParam의 low word에 resource id값이 날라온다.
	case ID_ACCELERATOR40007:
	case ID_FILE_NEW:
		// 맵 만들기
		DialogBox(g_myWND.GethInstance(), MAKEINTRESOURCE(IDD_DIALOG1), g_hWnd, DlgProcNewMap);
		break;
	case ID_ACCELERATOR40009:
	case ID_FILE_OPEN:
		// 맵 불러오기★
		if (g_myWND.OpenFileDlg(L"맵 파일\0*.jwm\0") == TRUE)
		{
			g_myWND.OpenFileText(g_myWND.GetDlgFileName());
			g_myWND.GetFileText(&tStr);

			g_DX9Map->SetMapData(tStr);
			g_DX9Map->GetTileName(&g_strTileName);
			g_DX9Map->GetMapName(&g_strMapName);

			LoadTile(g_strTileName);
			g_DX9Map->CreateMapWithData();

			tStr = g_Caption;
			tStr += L" <맵 이름: ";
			tStr += g_strMapName;
			tStr += L"> <크기: ";
			_itow_s(g_DX9Map->GetMapCols(), tWC, 10);
			tStr += tWC;
			tStr += L"x";
			_itow_s(g_DX9Map->GetMapRows(), tWC, 10);
			tStr += tWC;
			tStr += L">";
			SetWindowText(g_hWnd, tStr.c_str());

			AdjustScrollbars();
		}
		break;
	case ID_ACCELERATOR40013:
	case ID_FILE_SAVE:
		// 맵 저장하기
		if (g_myWND.SaveFileDlg(L"모든 파일\0*.*\0") == TRUE)
		{
			if (g_DX9Map->IsMapCreated())
			{
				g_DX9Map->GetMapData(&tStr);
				g_myWND.SetFileText(tStr);
				g_myWND.SaveFileText(g_myWND.GetDlgFileName());
			}
		}
		break;
	case ID_ACCELERATOR40011:
	case ID_HELP_INFO:
		MessageBox(g_hWnd, g_szHelp, TEXT("프로그램 정보"), MB_OK);
		break;
	}

	return 0;
}

int TileSetter(int MouseX, int MouseY, int RangeX, int RangeY) {
	if (g_ImgTile->IsTextureLoaded())
	{
		int tTileX = (int)(MouseX / g_TileW);
		int tTileY = (int)(MouseY / g_TileH);

		tTileX = min(tTileX, g_nTileCols - 1);
		tTileY = min(tTileY, g_nTileRows - 1);

		g_ImgTileSel->SetSize(g_TileW, g_TileH);
		g_ImgTileSel->SetPosition((float)(tTileX * g_TileW), (float)(tTileY * g_TileH));

		if ((RangeX > 1) || (RangeY > 1))
		{
			g_bMultiSel = true;
			g_ImgTileSel->SetSize(g_TileW * RangeX, g_TileH * RangeY);
		}

		tTileX = tTileX + g_nLScrollXPos;
		tTileY = tTileY + g_nLScrollYPos;

		tTileX = min(tTileX, g_nTileCols - 1);
		tTileY = min(tTileY, g_nTileRows - 1);

		g_nCurrTileID = tTileX + (tTileY * g_nTileCols);
		g_nCurrTileX = tTileX;
		g_nCurrTileY = tTileY;

		if (RangeX == 0) RangeX = 1;
		if (RangeY == 0) RangeY = 1;

		int sizeX = RangeX * g_TileW;
		int sizeY = RangeY * g_TileH;
		float u1 = (float)tTileX / (float)g_nTileCols;
		float u2 = (float)(tTileX + RangeX) / (float)g_nTileCols;
		float v1 = (float)tTileY / (float)g_nTileRows;
		float v2 = (float)(tTileY + RangeY) / (float)g_nTileRows;

		g_ImgMapSel->SetSize(sizeX, sizeY);
		g_ImgMapSel->SetRange(u1, u2, v1, v2);

		return 0;
	}

	return -1;
}

int MapSetter(int TileID, int MouseX, int MouseY) {
	if (g_ImgTile->IsTextureLoaded())
	{
		int tMapX = (int)(MouseX / g_TileW) + g_nRScrollXPos;
		int tMapY = (int)(MouseY / g_TileH) + g_nRScrollYPos;

		if (g_bMultiSel)
		{
			int NewTileID = TileID;
			for (int i = 0; i < g_nMSRangeX; i++)
			{
				for (int j = 0; j < g_nMSRangeY; j++)
				{
					if (TileID == -1)
					{
						NewTileID = -1;
					}
					else
					{
						NewTileID = TileID + i + (j * g_nTileCols);
					}
					g_DX9Map->SetMapFragment(NewTileID, tMapX + i, tMapY + j);
				}
			}
		}
		else
		{
			g_DX9Map->SetMapFragment(TileID, tMapX, tMapY);
		}

		return 0;
	}

	return -1;
}

int AdjustScrollbars() {
	RECT tRect;
	g_myWND.MoveScrollbarH(g_hChildL, g_hScrLH);
	g_myWND.MoveScrollbarV(g_hChildL, g_hScrLV);
	g_myWND.MoveScrollbarH(g_hChildR, g_hScrRH);
	g_myWND.MoveScrollbarV(g_hChildR, g_hScrRV);

	if (g_DX9Map)
	{
		if (g_DX9Map->IsMapCreated())
		{
			// 타일
			GetClientRect(g_hChildL, &tRect);

			int nCurrTileMaxRows = (int)(tRect.bottom / g_TileH);
			int nTileRestRows = g_nTileRows - nCurrTileMaxRows;
			nTileRestRows = max(0, nTileRestRows);
			g_myWND.SetScrollbar(g_hScrLV, 0, nTileRestRows, g_nTileRows);

			int nCurrTileMaxCols = (int)(tRect.right / g_TileW);
			int nTileRestCols = g_nTileCols - nCurrTileMaxCols;
			nTileRestCols = max(0, nTileRestCols);
			g_myWND.SetScrollbar(g_hScrLH, 0, nTileRestCols, g_nTileCols);

			// 맵
			GetClientRect(g_hChildR, &tRect);

			int nCurrMapMaxRows = (int)(tRect.bottom / g_TileH);
			int nMapRows = g_DX9Map->GetMapRows();
			int nMapRestRows = nMapRows - nCurrMapMaxRows;
			nMapRestRows = max(0, nMapRestRows);
			g_myWND.SetScrollbar(g_hScrRV, 0, nMapRestRows, nMapRows);

			int nCurrMapMaxCols = (int)(tRect.right / g_TileW);
			int nMapCols = g_DX9Map->GetMapCols();
			int nMapRestCols = nMapCols - nCurrMapMaxCols;
			nMapRestCols = max(0, nMapRestCols);
			g_myWND.SetScrollbar(g_hScrRH, 0, nMapRestCols, nMapCols);
		}
	}
	
	OnScrollbarChanged();

	return 0;
}

int OnScrollbarChanged() {
	g_nLScrollXPos = GetScrollPos(g_hScrLH, SB_CTL);
	g_nLScrollYPos = GetScrollPos(g_hScrLV, SB_CTL);

	int tOffsetX = 0;
	int tOffsetY = 0;

	if (g_ImgTile)
	{
		if (g_ImgTile->IsTextureLoaded())
		{
			tOffsetX = -g_nLScrollXPos * g_TileW;
			tOffsetY = -g_nLScrollYPos * g_TileH;
			g_ImgTile->SetPosition((float)tOffsetX, (float)tOffsetY);

			tOffsetX = (g_nCurrTileX - g_nLScrollXPos) * g_TileW;
			tOffsetY = (g_nCurrTileY - g_nLScrollYPos) * g_TileH;
			g_ImgTileSel->SetPosition((float)tOffsetX, (float)tOffsetY);

			g_nRScrollXPos = GetScrollPos(g_hScrRH, SB_CTL);
			g_nRScrollYPos = GetScrollPos(g_hScrRV, SB_CTL);

			if (g_DX9Map)
			{
				if (g_DX9Map->IsMapCreated())
				{
					tOffsetX = -g_nRScrollXPos * g_TileW;
					tOffsetY = -g_nRScrollYPos * g_TileH;
					g_DX9Map->SetPosition((float)tOffsetX, (float)tOffsetY);
				}
			}
		}
	}
	

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	g_myWND.BaseProc(hWnd, Message, wParam, lParam, OnScrollbarChanged);

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
		MoveWindow(g_hChildR, g_WndSepX, 0, tRect.right - g_WndSepX, tRect.bottom, TRUE);
		MoveWindow(g_hChildL, 0, 0, g_WndSepX, tRect.bottom, TRUE);

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
	g_myWND.BaseProc(hWnd, Message, wParam, lParam, OnScrollbarChanged);

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
		g_nTileXStart = tMouseX;
		g_nTileYStart = tMouseY;
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
					g_nMSRangeX = (int)((tMouseX - g_nTileXStart) / g_TileW) + 1;
					g_nMSRangeY = (int)((tMouseY - g_nTileYStart) / g_TileH) + 1;

					TileSetter(g_nTileXStart, g_nTileYStart, g_nMSRangeX, g_nMSRangeY);
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
	g_myWND.BaseProc(hWnd, Message, wParam, lParam, OnScrollbarChanged);

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
			g_ImgMapSel->SetAlpha(g_Alpha);
			tMapSelX = (tMouseX / g_TileW) * g_TileW;
			tMapSelY = (tMouseY / g_TileH) * g_TileH;
			g_ImgMapSel->SetPosition(tMapSelX, tMapSelY);
		}
		break;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

LRESULT CALLBACK DlgProcNewMap(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	wchar_t tWC[255] = { 0 };
	std::wstring tStr;
	size_t tFind;
	int tMapCols = 0;
	int tMapRows = 0;

	switch (iMessage)
	{
	case WM_INITDIALOG:
		PostMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hDlg, IDC_EDIT1), TRUE);
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_BUTTON1:
			if (g_myWND.OpenFileDlg(L"모든 파일\0*.*\0") == TRUE)
			{
				// 타일 이름 얻기
				g_strTileName = g_myWND.GetDlgFileName().c_str();
				tFind = g_strTileName.find_last_of('\\');
				if (tFind)
					g_strTileName = g_strTileName.substr(tFind + 1);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT4), g_strTileName.c_str());
			}
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

				if (tMapCols && tMapRows)
				{
					// 타일 불러오기
					LoadTile(g_strTileName);

					// 맵 만들기
					g_DX9Map->CreateMap(tWC, tMapCols, tMapRows);

					tStr = g_Caption;
					tStr += L" <맵 이름: ";
					tStr += g_strMapName;
					tStr += L"> <크기: ";
					_itow_s(tMapCols, tWC, 10);
					tStr += tWC;
					tStr += L"x";
					_itow_s(tMapRows, tWC, 10);
					tStr += tWC;
					tStr += L">";
					SetWindowText(g_hWnd, tStr.c_str());

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