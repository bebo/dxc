#pragma once
#include "FrameProvider.h"
#include "Direct3D.h"
template <class InFrame, class OutFrame>
class Effect : public FrameProvider<OutFrame>
{
	static_assert(std::is_base_of<Frame, InFrame>::value, "InFrame must derive from Frame");
	static_assert(std::is_base_of<Frame, OutFrame>::value, "OutFrame must derive from Frame");
protected:
	virtual void Initialize() = 0;
	virtual void Prepare() = 0;
	virtual OutFrame * DrawFrame() = 0;
	virtual void Unset() = 0;
public:
	virtual void AddInputFrameProvider(FrameProvider<InFrame> *input) = 0;
	virtual OutFrame * GetFrame(ID3D11DeviceContext *devcon) override;
};

template<class InFrame, class OutFrame>
OutFrame * Effect<InFrame, OutFrame>::GetFrame(ID3D11DeviceContext * devcon)
{
	Prepare();
	OutFrame *frame = DrawFrame();
	Unset();
	return frame;
}

