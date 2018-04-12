#include "windows/win-version.h"
#include "platform.h"
#include "dstr.h"
#include "config-file.h"
#include "pipe.h"

#include <windows.h>
#include "graphics-hook-info.h"


extern struct graphics_offsets offsets32;
extern struct graphics_offsets offsets64;
extern char *bebo_find_file(const char *file);

static inline bool load_offsets_from_string(struct graphics_offsets *offsets,
    const char *str)
{
  config_t *config;

  if (config_open_string(&config, str) != CONFIG_SUCCESS) {
    return false;
  }

  offsets->d3d8.present =
    (uint32_t)config_get_uint(config, "d3d8", "present");

  offsets->d3d9.present =
    (uint32_t)config_get_uint(config, "d3d9", "present");
  offsets->d3d9.present_ex =
    (uint32_t)config_get_uint(config, "d3d9", "present_ex");
  offsets->d3d9.present_swap =
    (uint32_t)config_get_uint(config, "d3d9", "present_swap");
  offsets->d3d9.d3d9_clsoff =
    (uint32_t)config_get_uint(config, "d3d9", "d3d9_clsoff");
  offsets->d3d9.is_d3d9ex_clsoff =
    (uint32_t)config_get_uint(config, "d3d9", "is_d3d9ex_clsoff");

  offsets->dxgi.present =
    (uint32_t)config_get_uint(config, "dxgi", "present");
  offsets->dxgi.present1 =
    (uint32_t)config_get_uint(config, "dxgi", "present1");
  offsets->dxgi.resize =
    (uint32_t)config_get_uint(config, "dxgi", "resize");

  config_close(config);
  return true;
}
#if 0

static inline bool load_offsets_from_file(struct graphics_offsets *offsets,
    const char *file)
{
  char *str = os_quick_read_utf8_file(file);
  bool success = false;
  if (str && *str)
    success = load_offsets_from_string(offsets, str);
  bfree(str);
  return success;
}

static inline bool config_ver_mismatch(
    config_t *ver_config,
    const char *section,
    struct win_version_info *ver)
{
  struct win_version_info config_ver;
  bool mismatch = false;

#define get_sub_ver(subver) \
  config_ver.subver = (int)config_get_int(ver_config, section, #subver); \
  mismatch |= config_ver.subver != ver->subver;

  get_sub_ver(major);
  get_sub_ver(minor);
  get_sub_ver(build);
  get_sub_ver(revis);

#undef get_sub_ver

  return mismatch;
}

static inline void write_config_ver(config_t *ver_config, const char *section,
    struct win_version_info *ver)
{
#define set_sub_ver(subver) \
  config_set_int(ver_config, section, #subver, ver->subver);

  set_sub_ver(major);
  set_sub_ver(minor);
  set_sub_ver(build);
  set_sub_ver(revis);

#undef set_sub_ver
}

static bool get_32bit_system_dll_ver(const wchar_t *system_lib,
    struct win_version_info *ver)
{
  wchar_t path[MAX_PATH];
  UINT ret;

#ifdef _WIN64
  ret = GetSystemWow64DirectoryW(path, MAX_PATH);
#else
  ret = GetSystemDirectoryW(path, MAX_PATH);
#endif
  if (!ret) {
    blog(LOG_ERROR, "Failed to get windows 32bit system path: "
        "%lu", GetLastError());
    return false;
  }

  wcscat(path, L"\\");
  wcscat(path, system_lib);
  return get_dll_ver(path, ver);
}

bool cached_versions_match(void)
{
  struct win_version_info d3d8_ver  = {0};
  struct win_version_info d3d9_ver  = {0};
  struct win_version_info dxgi_ver = {0};
  bool ver_mismatch = false;
  config_t *config;
  char *ver_file;
  int ret;

  ver_mismatch |= !get_32bit_system_dll_ver(L"d3d8.dll", &d3d8_ver);
  ver_mismatch |= !get_32bit_system_dll_ver(L"d3d9.dll", &d3d9_ver);
  ver_mismatch |= !get_32bit_system_dll_ver(L"dxgi.dll", &dxgi_ver);

  ver_file = obs_module_config_path("version.ini");
  if (!ver_file)
    return false;

  ret = config_open(&config, ver_file, CONFIG_OPEN_ALWAYS);
  if (ret != CONFIG_SUCCESS)
    goto failed;

  ver_mismatch |= config_ver_mismatch(config, "d3d8", &d3d8_ver);
  ver_mismatch |= config_ver_mismatch(config, "d3d9", &d3d9_ver);
  ver_mismatch |= config_ver_mismatch(config, "dxgi", &dxgi_ver);

  if (ver_mismatch) {
    write_config_ver(config, "d3d8", &d3d8_ver);
    write_config_ver(config, "d3d9", &d3d9_ver);
    write_config_ver(config, "dxgi", &dxgi_ver);
    config_save_safe(config, "tmp", NULL);
  }

failed:
  bfree(ver_file);
  config_close(config);
  return !ver_mismatch;
}
#endif

bool load_graphics_offsets(bool is32bit)
{
  char *offset_exe_path = NULL;
  struct dstr offset_exe = {0};
  char *config_ini = NULL;
  struct dstr str = {0};
  os_process_pipe_t *pp;
  bool success = false;
  char data[128];

#ifndef _WIN64
  if (!is32bit && !is_64_bit_windows()) {
    return true;
  }
#endif

  dstr_copy(&offset_exe, "get-graphics-offsets");
  dstr_cat(&offset_exe, is32bit ? "32.exe" : "64.exe");
  offset_exe_path = bebo_find_file(offset_exe.array);

  pp = os_process_pipe_create(offset_exe_path, "r");
  if (!pp) {
    blog(LOG_INFO, "load_graphics_offsets: Failed to start '%s'",
        offset_exe.array);
    goto error;
  }

  for (;;) {
    size_t len = os_process_pipe_read(pp, (uint8_t*)data, 128);
    if (!len)
      break;

    dstr_ncat(&str, data, len);
  }

  //	config_ini = obs_module_config_path(is32bit ? "32.ini" : "64.ini");
  //	os_quick_write_utf8_file_safe(config_ini, str.array, str.len, false,
  //			"tmp", NULL);
  //	bfree(config_ini);

  success = load_offsets_from_string(is32bit ? &offsets32 : &offsets64,
      str.array);
  if (!success) {
    blog(LOG_INFO, "load_graphics_offsets: Failed to load string");
  }

  os_process_pipe_destroy(pp);

error:
  bfree(offset_exe_path);
  dstr_free(&offset_exe);
  dstr_free(&str);
  return success;
}

#if 0
bool load_cached_graphics_offsets(bool is32bit)
{
  char *config_ini = NULL;
  bool success;

  config_ini = obs_module_config_path(is32bit ? "32.ini" : "64.ini");
  success = load_offsets_from_file(is32bit ? &offsets32 : &offsets64,
      config_ini);
  if (!success)
    success = load_graphics_offsets(is32bit);

  bfree(config_ini);
  return success;
}
#endif

