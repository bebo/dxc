#include "stdafx.h"
#include "BGRAFrame.h"


BGRAFrame::BGRAFrame(ID3D11Texture2D * texture) : texture(texture)
{
	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);
	width = desc.Width;
	height = desc.Height;
}

BGRAFrame::~BGRAFrame()
{
}

UINT BGRAFrame::getWidth()
{
	return width;
}

UINT BGRAFrame::getHeight()
{
	return height;
}
