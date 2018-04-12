#include "game_capture_provider.h"
#include "game_capture.h"

extern "C" {
  char* dll_inject_path = "C:\\Program Files\\Bebo Dev\\bebodlls";
  extern bool load_graphics_offsets(bool is32bit);
}

GameCaptureProvider::GameCaptureProvider(ID3D11Device* device,
    ID3D11DeviceContext* device_context)
  : device_(device), device_context_(device_context), shtex_handle_(0), shared_texture_(NULL) {
  game_capture_config_ = new GameCaptureConfig;
  game_capture_config_->scale_cx = 1280;
  game_capture_config_->scale_cy = 720;
  game_capture_config_->force_scaling = 1;
  game_capture_config_->anticheat_hook = true;

  bool s32b = load_graphics_offsets(true);
  bool s64b = load_graphics_offsets(false);
}

GameCaptureProvider::~GameCaptureProvider() {
  delete game_capture_config_;
}

NativeFrame* GameCaptureProvider::GetFrame(ID3D11DeviceContext* device_context) {
  uint64_t frame_interval = 0;
  if (!game_capture_is_ready(game_capture_)) {
    char* window_class_name = "UnityWndClass"; 
    char* window_name = "Hearthstone";
    game_capture_ = game_capture_start(&game_capture_,
        window_class_name, window_name, 
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
  }

  if (!shared_texture_) {
    return NULL;
  }

  return new NativeFrame(shared_texture_);
}
