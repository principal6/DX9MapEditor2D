#pragma once

#include "Core/DX9Common.h"

class DX9MapEditor final : public DX9Common
{
public:
	static int s_TileSize;

public:
	DX9MapEditor() {};
	~DX9MapEditor() {};

	void DX9MapEditor::Create(int Width, int Height);
	void DX9MapEditor::Destroy();

	void DX9MapEditor::SetTileSize(int Size);
	int DX9MapEditor::GetTileSize() const;
};