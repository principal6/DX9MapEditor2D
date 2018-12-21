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
int g_WndSepX = 200;
DX9Base* g_DX9Left;
DX9Base* g_DX9Right;
DX9Image* g_ImgTile;
DX9Map* g_myMap;
bool g_MouseLBDown;
bool g_MouseRBDown;
int g_nTileWidth;
int g_nTileHeight;

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

	g_DX9Right = new DX9Base;
	g_DX9Right->CreateOnWindow(g_hChildR);
	g_DX9Right->SetBGColor(D3DCOLOR_XRGB(50, 50, 50));

	g_ImgTile = new DX9Image;
	g_ImgTile->Create(g_DX9Left->GetDevice());

	g_myMap = new DX9Map;
	g_myMap->Create(g_DX9Right->GetDevice());

	g_DX9Left->RunWithAccel(MainLoop, g_myWND.GethAccel());

	g_DX9Left->Destroy();
	g_DX9Right->Destroy();
	g_ImgTile->Destroy();
	g_myMap->Destroy();

	delete g_DX9Left;
	delete g_DX9Right;
	delete g_ImgTile;
	delete g_myMap;
}

int MainLoop() {
	g_DX9Left->BeginRender();

		g_ImgTile->Draw();

	g_DX9Left->EndRender();

	g_DX9Right->BeginRender();

		g_myMap->Draw();

	g_DX9Right->EndRender();
	return 0;
}

int HandleAccelAndMenu(WPARAM wParam) {
	switch (LOWORD(wParam)) { // wParam�� low word�� resource id���� ����´�.
	case ID_ACCELERATOR40016:
	case ID_TILE_OPEN:
		if (g_myWND.OpenFileDlg(L"��� ����\0*.*\0") == TRUE)
		{
			// Ÿ�� ����
			g_ImgTile->SetTexture(g_myWND.GetDlgFileName().c_str());
			g_myMap->SetTexture(g_myWND.GetDlgFileName().c_str());
			g_nTileWidth = 32;
			g_nTileHeight = 32;
			g_myMap->SetTileInfo(g_nTileWidth, g_nTileHeight);
		}
		break;
	case ID_ACCELERATOR40007:
	case ID_FILE_NEW:
		DialogBox(g_myWND.GethInstance(), MAKEINTRESOURCE(IDD_DIALOG1), g_hWnd, DlgProcNewMap);
		break;
	case ID_ACCELERATOR40009:
	case ID_FILE_OPEN:
		if (g_myWND.OpenFileDlg(L"��� ����\0*.*\0") == TRUE)
		{
			g_myWND.OpenFileText(g_myWND.GetDlgFileName());
		}
		break;
	case ID_ACCELERATOR40013:
	case ID_FILE_SAVE:
		if (g_myWND.SaveFileDlg(L"��� ����\0*.*\0") == TRUE)
		{
			g_myWND.SaveFileText(g_myWND.GetDlgFileName());
		}
		break;
	case ID_ACCELERATOR40011:
	case ID_HELP_INFO:
		MessageBox(g_hWnd, TEXT("�������� �� ������ ���� ��� ���Դϴ�."), TEXT("����"), MB_OK);
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
		GetClientRect(hWnd, &tRect);
		MoveWindow(g_hChildR, g_WndSepX, 0, tRect.right - g_WndSepX, tRect.bottom, TRUE);
		MoveWindow(g_hChildL, 0, 0, g_WndSepX, tRect.bottom, TRUE);
		g_myWND.MoveScrollbarH(g_hChildL, g_hScrLH);
		g_myWND.MoveScrollbarV(g_hChildL, g_hScrLV);
		g_myWND.MoveScrollbarH(g_hChildR, g_hScrRH);
		g_myWND.MoveScrollbarV(g_hChildR, g_hScrRV);
		if (g_DX9Left)
			g_DX9Left->Resize(g_hChildL);
		break;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

LRESULT CALLBACK WndProcL(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	g_myWND.BaseProc(hWnd, Message, wParam, lParam);

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
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

int MapSetter(int TileID, int MouseX, int MouseY) {
	int tMapX = (int)(MouseX / g_nTileWidth);
	int tMapY = (int)(MouseY / g_nTileHeight);

	g_myMap->SetMapFragment(TileID, tMapX, tMapY);
	std::cout << tMapX << " / " << tMapY << std::endl;
	
	return 0;
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
		g_MouseLBDown = true;
		MapSetter(0, tMouseX, tMouseY);
		break;
	case WM_LBUTTONUP:
		g_MouseLBDown = false;
		break;
	case WM_RBUTTONDOWN:
		g_MouseRBDown = true;
		MapSetter(-1, tMouseX, tMouseY);
		break;
	case WM_RBUTTONUP:
		g_MouseRBDown = false;
		break;
	case WM_MOUSEMOVE:
		if (g_MouseLBDown)
			MapSetter(0, tMouseX, tMouseY);
		if (g_MouseRBDown)
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