#include "native_frame.h"

NativeFrame::NativeFrame(ID3D11Texture2D* texture)
  : texture_(texture), shader_view_(NULL) {
}

NativeFrame::NativeFrame(ID3D11Texture2D* texture, ID3D11ShaderResourceView* shader)
  : texture_(texture), shader_view_(shader) {
}


NativeFrame::~NativeFrame() {
  if (texture_) {
    delete texture_;
  }
  if (shader_view_) {
    delete shader_view_;
  }
}

ID3D11Texture2D* NativeFrame::GetTexture() {
  return texture_;
}

ID3D11ShaderResourceView* NativeFrame::GetShaderResourceView() {
  return shader_view_;
}
