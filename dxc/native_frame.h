#ifndef NATIVE_FRAME_H_
#define NATIVE_FRAME_H_

#include <d3d11.h>
#include <wrl.h>
#include "frame.h"

using Microsoft::WRL::ComPtr;

class NativeFrame : public Frame {
public:
    NativeFrame(ID3D11Texture2D* texture);
    ~NativeFrame();

    ID3D11Texture2D* GetTexture();

protected:
    ID3D11Texture2D* texture_;
};

#endif /* NATIVE_FRAME_H_ */
