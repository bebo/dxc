#include "game_capture_provider.h"
#include "game_capture.h"

extern "C" {
  char* dll_inject_path = "C:\\Program Files\\Bebo Dev\\bebodlls";
  extern bool load_graphics_offsets(bool is32bit);
}

GameCaptureProvider::GameCaptureProvider(ID3D11Device* device,
    ID3D11DeviceContext* device_context, UINT width, UINT height)
  : FrameProvider(width, height), 
    device_(device), device_context_(device_context), 
    shtex_handle_(0), shared_texture_(NULL) {
  load_graphics_offsets(true);
  load_graphics_offsets(false);

  game_capture_config_ = new GameCaptureConfig;
  game_capture_config_->scale_cx = width;
  game_capture_config_->scale_cy = height;
  game_capture_config_->force_scaling = 1;
  game_capture_config_->anticheat_hook = true;

  // TODO: make use of game_capture_config's window name / class name
  capture_window_class_name_ = "UnityWndClass";
  capture_window_name_ = "Hearthstone";
}

GameCaptureProvider::~GameCaptureProvider() {
  delete game_capture_config_;
}

NativeFrame* GameCaptureProvider::GetFrame(ID3D11DeviceContext* device_context) {
  uint64_t frame_interval = 0;
  if (!game_capture_is_ready(game_capture_)) {
    game_capture_ = game_capture_start(&game_capture_,
        capture_window_class_name_, capture_window_name_, 
        game_capture_config_, frame_interval);
  }

  if (game_capture_is_active(game_capture_) &&
      !game_capture_tick(game_capture_)) {
    return NULL; // no game frame
  }

  uint32_t gc_shtex_handle = game_capture_get_shtex_handle(game_capture_);
  if (gc_shtex_handle != shtex_handle_) {
    // setup texture
    HRESULT hr = device_->OpenSharedResource((HANDLE)(DWORD_PTR)gc_shtex_handle,
      __uuidof(ID3D11Texture2D), (void**)&shared_texture_);
    shtex_handle_ = gc_shtex_handle;

    if (hr == S_OK) {
      D3D11_TEXTURE2D_DESC texture_desc = { 0 };
      shared_texture_->GetDesc(&texture_desc);

      D3D11_SHADER_RESOURCE_VIEW_DESC shader_desc;
      memset(&shader_desc, 0, sizeof(shader_desc));
      shader_desc.Format = texture_desc.Format;
      shader_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      shader_desc.Texture2D.MipLevels = 1;
      device_->CreateShaderResourceView(shared_texture_, &shader_desc, &shared_shader_view_);
    }
  }

  if (!shared_texture_) {
    return NULL;
  }

  return new NativeFrame(shared_texture_, shared_shader_view_);
}
