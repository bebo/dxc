#include "stdafx.h"
#include "Direct3D.h"
#include "GraphicsTypes.h"
#include <d3dcompiler.h>
#include <time.h>

void Direct3D::Initialize(HWND hWnd, long width, long height)
{
  this->width = width;
  this->height = height;

  // create a struct to hold information about the swap chain
  DXGI_SWAP_CHAIN_DESC scd;

  // clear out the struct for use
  ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

  // fill the swap chain description struct
  scd.BufferCount = 1;                                   // one back buffer
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
  scd.BufferDesc.Width = width;                          // set the back buffer width
  scd.BufferDesc.Height = height;                        // set the back buffer height
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
  scd.OutputWindow = hWnd;                               // the window to be used
  scd.SampleDesc.Count = 4;                              // how many multisamples
  scd.Windowed = TRUE;                                   // windowed/full-screen mode
  scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

  // create a device, device context and swap chain using the information in the scd struct
  D3D11CreateDeviceAndSwapChain(NULL,
      D3D_DRIVER_TYPE_HARDWARE,
      NULL,
      D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG,
      NULL,
      NULL,
      D3D11_SDK_VERSION,
      &scd,
      &swapchain,
      &dev,
      NULL,
      &devcon);


  // get the address of the back buffer
  ID3D11Texture2D *pBackBuffer;
  swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

  // use the back buffer address to create the render target
  dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
  pBackBuffer->Release();

  // set the render target as the back buffer
  devcon->OMSetRenderTargets(1, &backbuffer, NULL);

  // Set the viewport
  D3D11_VIEWPORT viewport;
  ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = float(width);
  viewport.Height = float(height);

  devcon->RSSetViewports(1, &viewport);

  shaderLibrary = new ShaderLibrary(dev, devcon);
  shaderLibrary->PopulateLibrary();

  mixerShaderInfo = shaderLibrary->GetShaderInfo(L"basic_mixer");
}

void Direct3D::Start()
{
  DWORD myThreadId;
  renderThread = CreateThread(0, 0, Direct3D::RenderLoop, this, 0, &myThreadId);
}

void Direct3D::Cleanup()
{
  swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

  pVBuffer->Release();
  swapchain->Release();
  backbuffer->Release();
  dev->Release();
  devcon->Release();
  //TODO cleanup shader library
}

ID3D11Texture2D* Direct3D::CreateDynamicTexture(UINT width, UINT height, DXGI_FORMAT format)
{
  D3D11_TEXTURE2D_DESC desc;

  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = desc.ArraySize = 1;
  desc.Format = format;
  desc.SampleDesc.Quality = 0;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DYNAMIC;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  desc.MiscFlags = 0;

  ID3D11Texture2D *tex = NULL;
  dev->CreateTexture2D(&desc, NULL, &tex);
  return tex;
}

ID3D11Texture2D* Direct3D::CreateTextureTarget(UINT width, UINT height)
{
  // Initialize the render target texture description.
  D3D11_TEXTURE2D_DESC textureDesc;

  // Setup the render target texture description.
  textureDesc.Width = width;
  textureDesc.Height = height;
  textureDesc.MipLevels = 1;
  textureDesc.ArraySize = 1;
  textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  textureDesc.SampleDesc.Quality = 0;
  textureDesc.SampleDesc.Count = 1;
  textureDesc.Usage = D3D11_USAGE_DEFAULT;
  textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  textureDesc.CPUAccessFlags = 0;
  textureDesc.MiscFlags = 0;

  ID3D11Texture2D *tex = NULL;
  dev->CreateTexture2D(&textureDesc, NULL, &tex);
  return tex;
}

void Direct3D::AddFrameProvider(FrameProvider<NativeFrame> * fp, UINT x, UINT y)
{
  FrameProviderInformation info;
  info.x = x;
  info.y = y;
  info.fp = fp;
  frameProviders.push_back(info);
}

DWORD WINAPI Direct3D::RenderLoop(void *klass)
{
  clock_t last_time = clock();
  clock_t sixtyFPSClocks = (clock_t)(CLOCKS_PER_SEC / 60.0);
  while (TRUE) {
    auto d3d = (Direct3D *)klass;
    d3d->DoRender();

    clock_t current_time = clock();
    clock_t dt = current_time - last_time;
    clock_t sleep_time = (sixtyFPSClocks - dt) * 1000 / (CLOCKS_PER_SEC) ;
    last_time = current_time;
    if (sleep_time > 0) {
      Sleep((DWORD)sleep_time);
    }
  }
  return 0;
}

static TEXTURECOORD coord1 = { 0.0f, 0.0f }; //top left
static TEXTURECOORD coord2 = { 1.0f, 0.0f }; //top right
static TEXTURECOORD coord3 = { 1.0f, 1.0f }; //bottom right
static TEXTURECOORD coord4 = { 0.0f, 1.0f }; //bottom left

void Direct3D::DoRender(void)
{
  FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

  devcon->VSSetShader(mixerShaderInfo.vertexShader, 0, 0);
  devcon->PSSetShader(mixerShaderInfo.pixelShader, 0, 0);
  devcon->IASetInputLayout(mixerShaderInfo.inputLayout);

  devcon->ClearRenderTargetView(backbuffer, color);

  // get the address of the back buffer
  //TODO: keep this around
  ID3D11Texture2D *pBackBuffer;
  swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

  // set the render target as the back buffer
  devcon->OMSetRenderTargets(1, &backbuffer, NULL);

  for (auto it = frameProviders.begin(); it != frameProviders.end(); ++it) {
    FrameProviderInformation fpInfo = *it;
    auto *fp = fpInfo.fp;

    UINT fp_width = fp->GetWidth();
    UINT fp_height = fp->GetHeight();

    float convertedX = -1.0f + (fpInfo.x / float(width) * 2.0f);
    float convertedY = 1.0f - 2 * (fpInfo.y / float(height) * 2.0f);
    float convertedWidth = (fp_width / float(width)) * 2.0f;
    float convertedHeight = (fp_height / float(height)) * 2.0f;

    VERTEX vertices[] = {
      { convertedX, convertedY, 0.0f, coord1 }, //top left
      { convertedX + convertedWidth, convertedY, 0.0f, coord2 }, //top right
      { convertedX + convertedWidth, convertedY - convertedHeight, 0.0f, coord3 }, //bottom right

      { convertedX + convertedWidth, convertedY - convertedHeight, 0.0f, coord3 }, //bottom right
      { convertedX, convertedY - convertedHeight, 0.0f, coord4 }, //bottom left
      { convertedX, convertedY, 0.0f, coord1 }, //top left
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    bd.ByteWidth = sizeof(VERTEX) * 6;             // size is the VERTEX struct * 3
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

    dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer

    //TODO: get rid of cast
    //also smart pointers for frames
    NativeFrame *frame = fp->GetFrame(devcon);
    if (frame == NULL) continue;

    ID3D11Texture2D *fpTex = frame->GetTexture();
    ID3D11ShaderResourceView *svr;

    //unbind
    ID3D11ShaderResourceView *nullSvr[1] = { NULL };
    ID3D11RenderTargetView *nullRT[1] = { NULL };
    devcon->PSSetShaderResources(0, 1, nullSvr);
    devcon->OMSetRenderTargets(1, nullRT, NULL);

    dev->CreateShaderResourceView(fpTex, NULL, &svr);
    devcon->PSSetShaderResources(0, 1, &svr);

    devcon->OMSetRenderTargets(1, &backbuffer, NULL);

    D3D11_MAPPED_SUBRESOURCE ms;
    devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
    memcpy(ms.pData, vertices, sizeof(vertices));                       // copy the data
    devcon->Unmap(pVBuffer, NULL);                                      // unmap the buffer

    // select which vertex buffer to display
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

    // select which primtive type we are using
    devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // draw the vertex buffer to the back buffer
    devcon->Draw(6, 0);

    // delete frame;
    if (svr) {
       svr->Release();
    }

    pVBuffer->Release();
  }

  // switch the back buffer and the front buffer
  swapchain->Present(0, 0);

  //unset render target
  ID3D11RenderTargetView *nullRT[1] = { NULL };
  devcon->OMSetRenderTargets(1, nullRT, NULL);
  ID3D11ShaderResourceView *nullSvr[1] = { NULL };
  devcon->PSSetShaderResources(0, 1, nullSvr);
}
