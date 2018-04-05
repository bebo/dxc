#pragma once

#include <d3d11.h>
#include <d3d9types.h>
#include <type_traits>
#include "Frame.h"

template <class T>
class FrameProvider
{
	static_assert(std::is_base_of<Frame, T>::value, "T must derive from Frame");

public:
	virtual T * GetFrame(ID3D11DeviceContext *devcon) = 0;
	virtual UINT GetWidth() = 0;
	virtual UINT GetHeight() = 0;
};

