#pragma once

#include "Effect.h"
#include "ShaderLibrary.h"
#include "native_frame.h"

class DebugEffect : public Effect <NativeFrame, NativeFrame>
{
  private:
    FrameProvider<NativeFrame> *inputFP;
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
    virtual void Prepare() override;
    virtual NativeFrame* DrawFrame() override;
    virtual void Unset() override;
    virtual void AddInputFrameProvider(FrameProvider<NativeFrame>* input) override;

    // Inherited via Effect
    virtual void Initialize() override;
};

