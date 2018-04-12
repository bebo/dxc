#ifndef GAME_CAPTURE_PROVIDER_
#define GAME_CAPTURE_PROVIDER_

#include "FrameProvider.h"
#include "game_capture.h"
#include "native_frame.h"

class GameCaptureProvider : public FrameProvider<NativeFrame> {
  public:
    GameCaptureProvider(
        ID3D11Device* device,
        ID3D11DeviceContext* device_context,
        UINT width, UINT height);
    ~GameCaptureProvider();

    // Inherited via FrameProvider
    NativeFrame* GetFrame(ID3D11DeviceContext* device_context) override;

  private:
    ID3D11Device*         device_;
    ID3D11DeviceContext*  device_context_;
    void*                 game_capture_;
    GameCaptureConfig*    game_capture_config_;
    char*                 capture_window_class_name_;
    char*                 capture_window_name_;

    uint32_t                      shtex_handle_;
    ID3D11Texture2D*              shared_texture_;
    ID3D11ShaderResourceView*     shared_shader_view_;

};

#endif
