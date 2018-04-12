#pragma once

#include <d3d11.h>
#include <type_traits>

template <class T>
class FrameProvider
{
  public:
  UINT width_;
  UINT height_;

  UINT GetWidth() {
    return width_;
  };

  UINT GetHeight() {
    return height_;
  };

  virtual T* GetFrame(ID3D11DeviceContext* devcon) = 0;
};

