#include "DX9MapEditor.h"

void DX9MapEditor::Create(int Width, int Height)
{
	// Set data that will be shared in many sub-classes
	m_WindowData.WindowWidth = Width;
	m_WindowData.WindowHeight = Height;
	m_WindowData.WindowHalfWidth = static_cast<float>(Width / 2.0f);
	m_WindowData.WindowHalfHeight = static_cast<float>(Height / 2.0f);
	GetCurrentDirectoryW(255, m_WindowData.AppDir);
}

void DX9MapEditor::Destroy()
{

}