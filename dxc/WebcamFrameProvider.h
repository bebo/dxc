#pragma once
#include "FrameProvider.h"
#include "MediaFoundation.h"
#include "BGRAFrame.h"

class WebcamFrameProvider : public FrameProvider<BGRAFrame>
{
private:
	// enter the main loop:
	IMFMediaSource *source;
	IMFSourceReader *reader;
	ID3D11Texture2D *tex;
	void GetTextureDimensions(UINT *width, UINT *height);
	void MakeTexture(UINT width, UINT height);
	
public:
	WebcamFrameProvider();
	~WebcamFrameProvider();

	// Inherited via FrameProvider
	virtual BGRAFrame * GetFrame(ID3D11DeviceContext * devcon) override;

	// Inherited via FrameProvider
	virtual UINT GetWidth() override;
	virtual UINT GetHeight() override;
};

