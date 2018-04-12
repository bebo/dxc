#include "stdafx.h"
#include "DebugEffect.h"
#include "ShaderLibrary.h"
#include "Direct3D.h"
#include "GraphicsTypes.h"

DebugEffect::DebugEffect()
{
}
    

DebugEffect::~DebugEffect()
{
}

void DebugEffect::Prepare()
{
  D3D11_BUFFER_DESC bd;
  ID3D11ShaderResourceView *svr;
  ID3D11Buffer *pVBuffer;

  NativeFrame* frame = inputFP->GetFrame(devcon);
  ID3D11Texture2D *fpTex = frame->GetTexture();
  delete frame;

  FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  devcon->ClearRenderTargetView(outputTextureView, color);

  //set render target to output
  devcon->OMSetRenderTargets(1, &outputTextureView, NULL);

  //set shader
  devcon->VSSetShader(shaderInfo.vertexShader, 0, 0);
  devcon->PSSetShader(shaderInfo.pixelShader, 0, 0);
  devcon->IASetInputLayout(shaderInfo.inputLayout);

  //set shader inputs
  TEXTURECOORD coord1 = { 0.0f, 0.0f }; //top left
  TEXTURECOORD coord2 = { 1.0f, 0.0f }; //top right
  TEXTURECOORD coord3 = { 1.0f, 1.0f }; //bottom right
  TEXTURECOORD coord4 = { 0.0f, 1.0f }; //bottom left

  float convertedX = -1.0f;
  float convertedY = 1.0f;
  float convertedWidth = 2.0f * (float(inputFP->GetWidth()) / screen_width);
  float convertedHeight = 2.0f * (float(inputFP->GetHeight()) / screen_height);

  VERTEX vertices[] = {
    { convertedX, convertedY, 0.0f, coord1 }, //top left
    { convertedX + convertedWidth, convertedY, 0.0f, coord2 }, //top right
    { convertedX + convertedWidth, convertedY - convertedHeight, 0.0f, coord3 }, //bottom right

    { convertedX + convertedWidth, convertedY - convertedHeight, 0.0f, coord3 }, //bottom right
    { convertedX, convertedY - convertedHeight, 0.0f, coord4 }, //bottom left
    { convertedX, convertedY, 0.0f, coord1 }, //top left
  };

  dev->CreateShaderResourceView(fpTex, NULL, &svr);
  devcon->PSSetShaderResources(0, 1, &svr);

  ZeroMemory(&bd, sizeof(bd));

  bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
  bd.ByteWidth = sizeof(VERTEX) * 6;             // size is the VERTEX struct * 3
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

  dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer

  D3D11_MAPPED_SUBRESOURCE ms;
  devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
  memcpy(ms.pData, vertices, sizeof(vertices));                 // copy the data
  devcon->Unmap(pVBuffer, NULL);                                      // unmap the buffer

  // select which vertex buffer to display
  UINT stride = sizeof(VERTEX);
  UINT offset = 0;
  devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
  // select which primtive type we are using
  devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  svr->Release();
  pVBuffer->Release();
}

NativeFrame* DebugEffect::DrawFrame()
{
  devcon->Draw(6, 0);
  return new NativeFrame(outputTexture);
}

void DebugEffect::Unset()
{
  auto devcon = Direct3D::GetInstance().GetDeviceContext();
  ID3D11ShaderResourceView *nullSvr[1] = { NULL };
  ID3D11RenderTargetView *nullRT[1] = { NULL };
  devcon->PSSetShaderResources(0, 1, nullSvr);
  devcon->OMSetRenderTargets(1, nullRT, NULL);
}

void DebugEffect::AddInputFrameProvider(FrameProvider<NativeFrame>* input)
{
  inputFP = input;
}

void DebugEffect::Initialize()
{
  D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

  dev = Direct3D::GetInstance().GetDevice();
  devcon = Direct3D::GetInstance().GetDeviceContext();

  screen_width = Direct3D::GetInstance().width;
  screen_height = Direct3D::GetInstance().height;

  outputTexture = Direct3D::GetInstance().CreateTextureTarget(GetWidth(), GetHeight());
  renderTargetViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  renderTargetViewDesc.Texture2D.MipSlice = 0;

  // Create the render target view.
  dev->CreateRenderTargetView(outputTexture, &renderTargetViewDesc, &outputTextureView);

  shaderInfo = Direct3D::GetInstance().shaderLibrary->GetShaderInfo(L"debug_blue");
}
