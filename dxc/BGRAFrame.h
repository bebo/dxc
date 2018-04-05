#pragma once
#include "Frame.h"
class BGRAFrame : public Frame
{
private:
	UINT width, height;
public:
	ID3D11Texture2D *texture;
	BGRAFrame(ID3D11Texture2D *texture);
	~BGRAFrame();

	// Inherited via Frame
	virtual UINT getWidth() override;
	virtual UINT getHeight() override;
};


