#include "stdafx.h"
#include "ShaderLibrary.h"


void ShaderLibrary::PopulateLibrary()
{
	ShaderInfo debugBlueShaderInfo;
	ShaderInfo basicMixerShaderInfo;

	D3D11_INPUT_ELEMENT_DESC positionTexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	CompileShader(L"debug_blue.shader", "VShader", "PShader", &debugBlueShaderInfo.vertexShader, &debugBlueShaderInfo.pixelShader, positionTexDesc, &debugBlueShaderInfo.inputLayout);
	CompileShader(L"basic_mixer.shader", "VShader", "PShader", &basicMixerShaderInfo.vertexShader, &basicMixerShaderInfo.pixelShader, positionTexDesc, &basicMixerShaderInfo.inputLayout);

	shaderInfoMap[L"debug_blue"] = debugBlueShaderInfo;
	shaderInfoMap[L"basic_mixer"] = basicMixerShaderInfo;
}

ShaderLibrary::ShaderLibrary(ID3D11Device * device, ID3D11DeviceContext * deviceContext): dev(device), devcon(deviceContext)
{}

ShaderLibrary::~ShaderLibrary()
{}

void ShaderLibrary::CompileShader(LPWSTR name, LPSTR vertexShaderName, LPSTR pixelShaderName, ID3D11VertexShader ** vertexShader, ID3D11PixelShader ** pixelShader, D3D11_INPUT_ELEMENT_DESC * inputDesc, ID3D11InputLayout ** inputLayout)
{
	// load and compile the two shaders
	ID3D10Blob *VS, *PS;
	ID3DBlob *errorMsg;
	D3DCompileFromFile(name, 0, 0, vertexShaderName, "vs_5_0", 0, 0, &VS, &errorMsg);
	if (errorMsg) {
		OutputDebugStringA((char*)errorMsg->GetBufferPointer());
		errorMsg->Release();
		errorMsg = NULL;
	}
	D3DCompileFromFile(name, 0, 0, pixelShaderName, "ps_5_0", 0, 0, &PS, &errorMsg);
	if (errorMsg) {
		OutputDebugStringA((char*)errorMsg->GetBufferPointer());
		errorMsg->Release();
		errorMsg = NULL;
	}

	// encapsulate both shaders into shader objects
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, vertexShader);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, pixelShader);

	// set the shader objects
	//devcon->VSSetShader(pVS, 0, 0);
	//devcon->PSSetShader(pPS, 0, 0);

	// create the input layout object
	//D3D11_INPUT_ELEMENT_DESC ied[] =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};

	//dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	//devcon->IASetInputLayout(pLayout);

	dev->CreateInputLayout(inputDesc, 2, VS->GetBufferPointer(), VS->GetBufferSize(), inputLayout);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;
	ID3D11SamplerState* myLinearWrapSampler;
	HRESULT hr = dev->CreateSamplerState(&samplerDesc, &myLinearWrapSampler);
	devcon->PSSetSamplers(0, 1, &myLinearWrapSampler);
}

ShaderInfo ShaderLibrary::GetShaderInfo(std::wstring name)
{
	return shaderInfoMap[name];
}

