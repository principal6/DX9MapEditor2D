#include "JWWindow.h"
#include "DX9Base.h"
#include "DX9Image.h"
#include "DX9Map.h"

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
DX9Map* g_myMap;
bool g_MapMouseLBDown;
bool g_MapMouseRBDown;
bool g_TileMouseLBDown;
bool g_TileMouseRBDown;
int g_nTileWidth;
int g_nTileHeight;
int g_nTileCols;
int g_nTileRows;
int g_nCurrTileID = 0;

int MainLoop();
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcL(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcR(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcNewMap(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

int main() {
	JWWindow g_myWND;
	g_hWnd = g_myWND.Create(L"Program", 50, 50, 800, 600, RGB(255, 255, 255), WndProc, WS_OVERLAPPEDWINDOW);
	RECT tempRect;
	GetClientRect(g_hWnd, &tempRect);
	g_hChildL = g_myWND.AddChild(g_hWnd, L"ChL", 0, 0, g_WndSepX, tempRect.bottom, RGB(230, 230, 230), WndProcL);
	g_hChildR = g_myWND.AddChild(g_hWnd, L"ChR", g_WndSepX, 0, tempRect.right - g_WndSepX, tempRect.bottom, RGB(150, 150, 150), WndProcR);
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
	g_ImgTile->Create(g_DX9Left->GetDevice());

	g_ImgTileSel = new DX9Image;
	g_ImgTileSel->Create(g_DX9Left->GetDevice());
	g_ImgTileSel->SetTexture(L"tilesel32x32.png");

	g_myMap = new DX9Map;
	g_myMap->Create(g_DX9Right->GetDevice());

	g_DX9Left->RunWithAccel(MainLoop, g_myWND.GethAccel());

	g_DX9Left->Destroy();
	g_DX9Right->Destroy();
	g_ImgTile->Destroy();
	g_ImgTileSel->Destroy();
	g_myMap->Destroy();

	delete g_DX9Left;
	delete g_DX9Right;
	delete g_ImgTile;
	delete g_ImgTileSel;
	delete g_myMap;
}

int MainLoop() {
	g_DX9Left->BeginRender();

		g_ImgTile->Draw();

		if (g_ImgTile->IsTextureLoaded())
			g_ImgTileSel->Draw();

	g_DX9Left->EndRender();

	g_DX9Right->BeginRender();

		g_myMap->Draw();

	g_DX9Right->EndRender();
	return 0;
}

int HandleAccelAndMenu(WPARAM wParam) {
	switch (LOWORD(wParam)) { // wParam의 low word에 resource id값이 날라온다.
	case ID_ACCELERATOR40016:
	case ID_TILE_OPEN:
		if (g_myWND.OpenFileDlg(L"모든 파일\0*.*\0") == TRUE)
		{
			// 타일 열기
			g_ImgTile->SetTexture(g_myWND.GetDlgFileName().c_str());
			g_myMap->SetTexture(g_myWND.GetDlgFileName().c_str());
			g_nTileWidth = 32;
			g_nTileHeight = 32;
			g_myMap->SetTileInfo(g_nTileWidth, g_nTileHeight);
			g_nTileCols = (int)(g_ImgTile->GetWidth() / g_nTileWidth);
			g_nTileRows = (int)(g_ImgTile->GetHeight() / g_nTileHeight);
		}
		break;
	case ID_ACCELERATOR40007:
	case ID_FILE_NEW:
		DialogBox(g_myWND.GethInstance(), MAKEINTRESOURCE(IDD_DIALOG1), g_hWnd, DlgProcNewMap);
		break;
	case ID_ACCELERATOR40009:
	case ID_FILE_OPEN:
		if (g_myWND.OpenFileDlg(L"모든 파일\0*.*\0") == TRUE)
		{
			g_myWND.OpenFileText(g_myWND.GetDlgFileName());
		}
		break;
	case ID_ACCELERATOR40013:
	case ID_FILE_SAVE:
		if (g_myWND.SaveFileDlg(L"모든 파일\0*.*\0") == TRUE)
		{
			g_myWND.SaveFileText(g_myWND.GetDlgFileName());
		}
		break;
	case ID_ACCELERATOR40011:
	case ID_HELP_INFO:
		MessageBox(g_hWnd, TEXT("윈도우즈 앱 개발을 위한 기반 앱입니다."), TEXT("정보"), MB_OK);
		break;
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	g_myWND.BaseProc(hWnd, Message, wParam, lParam);

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
		g_myWND.MoveScrollbarH(g_hChildL, g_hScrLH);
		g_myWND.MoveScrollbarV(g_hChildL, g_hScrLV);
		g_myWND.MoveScrollbarH(g_hChildR, g_hScrRH);
		g_myWND.MoveScrollbarV(g_hChildR, g_hScrRV);
		if (g_DX9Left)
			g_DX9Left->Resize(g_hChildL);

		if (g_nTileHeight)
		{
			GetClientRect(g_hChildL, &tRect);
			int nCurrMaxRows = (int)(tRect.bottom / g_nTileHeight);
			std::cout << nCurrMaxRows << std::endl;
			int nRestRows = g_nTileRows - nCurrMaxRows;
			nRestRows = max(0, nRestRows);
			g_myWND.SetScrollbar(g_hScrLV, 0, nRestRows, g_nTileRows);
		}
		
		break;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

int TileSetter(int MouseX, int MouseY) {
	if ((g_nTileWidth) && (g_nTileHeight))
	{
		int tTileX = (int)(MouseX / g_nTileWidth);
		int tTileY = (int)(MouseY / g_nTileHeight);

		tTileX = min(tTileX, g_nTileCols - 1);
		tTileY = min(tTileY, g_nTileRows - 1);

		g_nCurrTileID = tTileX + (tTileY * g_nTileCols);
		g_ImgTileSel->SetPosition((float)(tTileX * g_nTileWidth), (float)(tTileY * g_nTileHeight));

		return 0;
	}

	return -1;
}

LRESULT CALLBACK WndProcL(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	g_myWND.BaseProc(hWnd, Message, wParam, lParam);

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
		g_TileMouseLBDown = true;
		TileSetter(tMouseX, tMouseY);
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

		break;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

int MapSetter(int TileID, int MouseX, int MouseY) {
	if ((g_nTileWidth) && (g_nTileHeight))
	{
		int tMapX = (int)(MouseX / g_nTileWidth);
		int tMapY = (int)(MouseY / g_nTileHeight);

		g_myMap->SetMapFragment(TileID, tMapX, tMapY);

		return 0;
	}
	
	return -1;
}

LRESULT CALLBACK WndProcR(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	g_myWND.BaseProc(hWnd, Message, wParam, lParam);

	int tMouseX = LOWORD(lParam);
	int tMouseY = HIWORD(lParam);
	
	switch (Message)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		HandleAccelAndMenu(wParam);
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
		break;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

LRESULT CALLBACK DlgProcNewMap(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	wchar_t tempStr[255] = { 0 };
	int tMapSizeX, tMapSizeY;

	switch (iMessage)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT1, tempStr, 255);
			tMapSizeX = GetDlgItemInt(hDlg, IDC_EDIT2, FALSE, FALSE);
			tMapSizeY = GetDlgItemInt(hDlg, IDC_EDIT3, FALSE, FALSE);

			if (tMapSizeX && tMapSizeY)
				g_myMap->CreateMap(tMapSizeX, tMapSizeY);

			g_myMap->SetMapFragment(0, 0, 0);

		case IDCANCEL:
			EndDialog(hDlg, 0);
			break;
		}
		return TRUE;
	}
	return FALSE;
}