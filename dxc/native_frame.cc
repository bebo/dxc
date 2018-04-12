#include "native_frame.h"

NativeFrame::NativeFrame(ID3D11Texture2D* texture)
  : texture_(texture) {
}

NativeFrame::~NativeFrame() {
  if (texture_) {
    delete texture_;
  }
}

ID3D11Texture2D* NativeFrame::GetTexture() {
  return texture_;
}
