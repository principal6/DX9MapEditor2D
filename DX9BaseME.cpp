#include "DX9BaseME.h"

void DX9BaseME::SetMainLoop(void(*MainLoop)())
{
	m_pMainLoop = MainLoop;
}

void DX9BaseME::Run()
{
	while (m_MSG.message != WM_QUIT)
	{
		if (PeekMessage(&m_MSG, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&m_MSG);
			DispatchMessage(&m_MSG);
		}
		else
		{
			m_pMainLoop();
		}
	}
}

int DX9BaseME::RunWithAccel(HACCEL hAccel)
{
	while (GetMessage(&m_MSG, 0, 0, 0)) {
		if (!TranslateAccelerator(m_hWnd, hAccel, &m_MSG)) {
			TranslateMessage(&m_MSG);
			DispatchMessage(&m_MSG);
			m_pMainLoop();
		}
	}

	return (int)m_MSG.wParam;
}