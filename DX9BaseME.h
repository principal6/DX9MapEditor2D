#pragma once

#include "DX9Base.h"

class DX9BaseME : public DX9Base
{
private:
	void(*m_pMainLoop)();
	HWND m_hWndME;

public:
	DX9BaseME() {};
	~DX9BaseME() {};

	void DX9BaseME::SetMainLoop(void(*MainLoop)());
	void DX9BaseME::Run();
	int DX9BaseME::RunWithAccel(HACCEL hAccel);
};