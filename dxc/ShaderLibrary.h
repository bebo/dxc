#pragma once

#include <d3dcompiler.h>
#include <d3d11.h>
#include <d3d9types.h>
#include <string>
#include <map>
#include <array>

typedef struct ShaderInfo {
	ID3D11PixelShader *pixelShader;
	ID3D11VertexShader *vertexShader;
	ID3D11InputLayout *inputLayout;
} ShaderInfo;

class ShaderLibrary
{
private:
	ID3D11Device *dev = NULL;
	ID3D11DeviceContext *devcon = NULL;
	std::map<std::wstring, ShaderInfo> shaderInfoMap;
	void ShaderLibrary::CompileShader(LPWSTR name, LPSTR vertexShaderName, LPSTR pixelShaderName, ID3D11VertexShader **vertexShader, ID3D11PixelShader **pixelShader, D3D11_INPUT_ELEMENT_DESC * inputDesc, ID3D11InputLayout **inputLayout);
public:
	ShaderInfo GetShaderInfo(std::wstring name);

	void PopulateLibrary();

	ShaderLibrary(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	~ShaderLibrary();
};

