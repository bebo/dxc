#pragma once

#include <d3d11.h>
#include "FrameProvider.h"
#include "MediaFoundation.h"
#include "native_frame.h"

class WebcamFrameProvider : public FrameProvider<NativeFrame>
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
    NativeFrame* GetFrame(ID3D11DeviceContext* devcon) override;
};

