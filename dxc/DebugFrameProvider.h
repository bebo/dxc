#pragma once

#include "FrameProvider.h"
#include "native_frame.h"

class DebugFrameProvider : public FrameProvider<NativeFrame>
{
  private:
    UINT x = 5, y = 5, vx = 1, vy = 1;
    ID3D11Texture2D* tex;

  public:
    DebugFrameProvider(UINT width, UINT height);
    ~DebugFrameProvider() {};

    // Inherited via FrameProvider
    NativeFrame* GetFrame(ID3D11DeviceContext *devcon) override;
};

