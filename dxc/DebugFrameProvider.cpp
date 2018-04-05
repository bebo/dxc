#include "stdafx.h"
#include "DebugFrameProvider.h"
#include "Direct3D.h"

DebugFrameProvider::DebugFrameProvider(UINT width, UINT height) {
	this->width = width;
	this->height = height;
	tex = Direct3D::GetInstance().CreateDynamicTexture(width, height, DXGI_FORMAT_B8G8R8A8_UNORM);
	OutputDebugString(L"mapped shit");
}

BGRAFrame * DebugFrameProvider::GetFrame(ID3D11DeviceContext * devcon)
{
	int ballDim = int(height / 10);
	if (x >= (width - ballDim - 1) || x <= 1) {
		vx *= -1;
	}

	if (y >= (height - ballDim - 1) || y <= 1) {
		vy *= -1;
	}

	x += vx;
	y += vy;

	D3D11_MAPPED_SUBRESOURCE resource;
	devcon->Map(tex, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memset(resource.pData, 255, resource.DepthPitch);
	//render ball
	for (UINT i = y; i < y + ballDim; i++) {
		memset((char *)resource.pData + (i * resource.RowPitch + 4 * x), 127, 4 * ballDim);
	}
	devcon->Unmap(tex, 0);
	auto frame = new BGRAFrame(tex);
	return frame;
}

UINT DebugFrameProvider::GetWidth()
{
	return width;
}

UINT DebugFrameProvider::GetHeight()
{
	return height;
}
