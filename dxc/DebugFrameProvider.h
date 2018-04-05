#pragma once
#include "FrameProvider.h"
#include "BGRAFrame.h"

class DebugFrameProvider: public FrameProvider<BGRAFrame>
{
private:
	UINT x = 5, y = 5, vx = 1, vy = 1;
	UINT width, height;
	ID3D11Texture2D *tex;
public:
	DebugFrameProvider(UINT width, UINT height);
	~DebugFrameProvider() {};
	// Inherited via FrameProvider
	virtual BGRAFrame * GetFrame(ID3D11DeviceContext *devcon) override;

	// Inherited via FrameProvider
	virtual UINT GetWidth() override;
	virtual UINT GetHeight() override;
};

