#include "stdafx.h"
#include "WebcamFrameProvider.h"
#include "Direct3D.h"
#include <sstream>
#include <cmath>

//TODO make utils for this
template <class T> void SafeRelease(T **ppT)
{
  if (*ppT)
  {
    (*ppT)->Release();
    *ppT = NULL;
  }
}

void WebcamFrameProvider::GetTextureDimensions(UINT * width, UINT * height)
{
  D3D11_TEXTURE2D_DESC desc;

  if (!tex) return;

  tex->GetDesc(&desc);

  if (width) {
    *width = desc.Width;
  }
  if (height) {
    *height = desc.Height;
  }
}

void WebcamFrameProvider::MakeTexture(UINT width, UINT height)
{
  if (tex) {
    tex->Release();
  }
  tex = Direct3D::GetInstance().CreateDynamicTexture(width, height, DXGI_FORMAT_B8G8R8A8_UNORM);
}

WebcamFrameProvider::WebcamFrameProvider()
    : FrameProvider(480, 480) {
  //TODO: doesn't even matter
  MakeTexture(480, 480);
  MediaFoundation::EnumerateVideoDevices(&source);
  MediaFoundation::SetDeviceFormat(source);
  MediaFoundation::CreateSourceReader(source, &reader);
}

WebcamFrameProvider::~WebcamFrameProvider()
{
}

NativeFrame* WebcamFrameProvider::GetFrame(ID3D11DeviceContext * devcon)
{
  UINT current_texture_width = 0;
  UINT current_texture_height = 0;
  HRESULT hr = S_OK;
  IMFSample *pSample = NULL;
  IMFMediaBuffer * mediaBuffer = NULL;
  IMF2DBuffer * buffer = NULL;

  size_t  cSamples = 0;

  DWORD streamIndex, flags;
  LONGLONG llTimeStamp;

  GetTextureDimensions(&width_, &height_);

  //TODO: read sample blocks, needs to happen not on the render thread
  hr = reader->ReadSample(
      MF_SOURCE_READER_ANY_STREAM,    // Stream index.
      0,                              // Flags.
      &streamIndex,                   // Receives the actual stream index. 
      &flags,                         // Receives status flags.
      &llTimeStamp,                   // Receives the time stamp.
      &pSample                        // Receives the sample or NULL.
      );

  if (FAILED(hr)) {
    OutputDebugString(L"Failed to read sample");
  } else {
    std::wstringstream ss;
    ss << llTimeStamp;
  }

  if(pSample) {
    pSample->GetBufferByIndex(0, &mediaBuffer);
    mediaBuffer->QueryInterface(IID_IMF2DBuffer, (void **)&buffer);
    D3D11_MAPPED_SUBRESOURCE resource;
    if (buffer) {
      BYTE *rawBuffer = NULL;
      LONG stride;
      DWORD length;
      buffer->Lock2D(&rawBuffer, &stride);
      buffer->GetContiguousLength(&length);

      if (current_texture_width < (ULONG)std::abs(stride) || current_texture_height < (ULONG)(length / std::abs(stride))) {
        //MakeTexture(stride, (length / std::abs(stride)));
      }

      devcon->Map(tex, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &resource);

      for (UINT i = 0; i < (length / std::abs(stride)); i++) {
        memcpy((BYTE *)resource.pData + (i * resource.RowPitch), rawBuffer + (i * stride), stride);
      }
      buffer->Unlock2D();
      devcon->Unmap(tex, 0);
    }
    SafeRelease(&mediaBuffer);
    SafeRelease(&buffer);
    SafeRelease(&pSample);
  }

  return new NativeFrame(tex);
}
