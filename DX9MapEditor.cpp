#include "DX9MapEditor.h"

// Static member variable declaration
int DX9MapEditor::s_TileSize;

void DX9MapEditor::Create(int Width, int Height)
{
	// Set default tile size
	s_TileSize = DEF_TILE_SIZE;

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

void DX9MapEditor::SetTileSize(int Size)
{
	s_TileSize = Size;
}

int DX9MapEditor::GetTileSize() const
{
	return s_TileSize;
}