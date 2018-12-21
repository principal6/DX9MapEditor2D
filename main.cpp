#include "JWWindow.h"
#include "DX9Base.h"
#include "DX9Image.h"

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
DX9Image* g_myImage;

int MainLoop();
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcL(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcR(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

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

	g_myImage = new DX9Image;
	g_myImage->Create(g_DX9Left->GetDevice());
	g_myImage->SetTexture(L"maptile32x32.png");

	g_DX9Left->RunWithAccel(MainLoop, g_myWND.GethAccel());
}

int MainLoop() {
	g_DX9Left->BeginRender();
	g_myImage->Draw();
	g_DX9Left->EndRender();
	return 0;
}

int HandleAccelAndMenu(WPARAM wParam) {
	switch (LOWORD(wParam)) { // wParam의 low word에 resource id값이 날라온다.
	case ID_ACCELERATOR40016:
	case ID_TILE_OPEN:
		if (g_myWND.OpenFileDlg(L"모든 파일\0*.*\0") == TRUE)
		{
			g_myImage->SetTexture(g_myWND.GetDlgFileName().c_str());
		}
		break;
	case ID_ACCELERATOR40007:
	case ID_FILE_NEW:
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

LRESULT CALLBACK WndProcR(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	g_myWND.BaseProc(hWnd, Message, wParam, lParam);

	switch (Message)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		HandleAccelAndMenu(wParam);
		break;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}