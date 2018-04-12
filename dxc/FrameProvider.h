#pragma once

#include <d3d11.h>
#include <type_traits>

template <class T>
class FrameProvider
{
  public:
    FrameProvider(UINT width, UINT height) : width_(width), height_(height) {};
    FrameProvider() : FrameProvider(0, 0) {};

    UINT GetWidth() {
      return width_;
    };

    UINT GetHeight() {
      return height_;
    };

    virtual T* GetFrame(ID3D11DeviceContext* devcon) = 0;

  protected:
    UINT width_;
    UINT height_;
};

