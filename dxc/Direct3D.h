#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3d9types.h>
#include <vector>
#include "FrameProvider.h"
#include "BGRAFrame.h"
#include "ShaderLibrary.h"

typedef struct FrameProviderInformation {
	FrameProvider<BGRAFrame> *fp;
	UINT x, y;
} FrameProviderInformation;

class Direct3D
{
private:

	Direct3D() {}

	IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
	ID3D11Device *dev;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
	ID3D11RenderTargetView *backbuffer;    // the pointer to our back buffer
	ID3D11InputLayout *pLayout;            // the pointer to the input layout
	ShaderInfo mixerShaderInfo;
	ID3D11Buffer *pVBuffer;                // the pointer to the vertex buffe
	static Direct3D instance;
	HANDLE renderThread;
	FrameProvider<BGRAFrame> *frameProvider;
	ID3D11Texture2D *drawTexture;
	std::vector<FrameProviderInformation> frameProviders;

	void DoRender();
	static DWORD WINAPI RenderLoop(void *klass);

public:
	long width, height;
	ShaderLibrary *shaderLibrary;

	static Direct3D& GetInstance()
	{
		static Direct3D    instance; // Guaranteed to be destroyed.
							  // Instantiated on first use.
		return instance;
	}
	void Initialize(HWND hWnd, long width, long height);
	void Start();
	void Cleanup();
	ID3D11Texture2D *CreateDynamicTexture(UINT width, UINT height, DXGI_FORMAT format);
	ID3D11Texture2D * Direct3D::CreateTextureTarget(UINT width, UINT height);
	void AddFrameProvider(FrameProvider<BGRAFrame> *fp, UINT x, UINT y);
	Direct3D(Direct3D const&) = delete;
	void operator=(Direct3D const&) = delete;

	ID3D11Device *GetDevice() { return dev; }
	ID3D11DeviceContext *GetDeviceContext() { return devcon; }
};

