#pragma once
#include "Frame.h"
class YUVFrame :Frame
{
public:
	ID3D11Texture2D *textureY;
	ID3D11Texture2D *textureU;
	ID3D11Texture2D *textureV;

	YUVFrame();
	~YUVFrame();
};

