#pragma once
#include "BGRAFrame.h"
#include "Effect.h"
#include "ShaderLibrary.h"

class DebugEffect : public Effect <BGRAFrame, BGRAFrame>
{
private:
	FrameProvider<BGRAFrame> *inputFP;
	ID3D11Texture2D *outputTexture;
	ID3D11RenderTargetView *outputTextureView;
	ShaderInfo shaderInfo;
	ID3D11Device *dev;
	ID3D11DeviceContext *devcon;
	UINT screen_width, screen_height;

public:
	DebugEffect();
	~DebugEffect();

	// Inherited via Effect
	virtual UINT GetWidth() override;
	virtual UINT GetHeight() override;
	virtual void Prepare() override;
	virtual BGRAFrame * DrawFrame() override;
	virtual void Unset() override;
	virtual void AddInputFrameProvider(FrameProvider<BGRAFrame>* input) override;

	// Inherited via Effect
	virtual void Initialize() override;
};

