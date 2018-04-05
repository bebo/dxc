#pragma once
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <list>

typedef struct MediaDevice {
	WCHAR * pFriendlyName;
	IMFMediaSource * pMediaSource;

	void destroy() {
		if (pFriendlyName) CoTaskMemFree(pFriendlyName);
		if (pMediaSource) pMediaSource->Release();
	}

} MediaDevice;

class MediaFoundation
{
public:
	static std::list<MediaDevice> GetVideoDevices();
	static void EnumerateVideoDevices(IMFMediaSource **pSource);
	static void EnumerateFormats(IMFMediaSource *pSource, IMFMediaType **type);
	static void SetDeviceFormat(IMFMediaSource *pSource);
	static void CreateSourceReader(IMFMediaSource *pSource, IMFSourceReader **pReader);
};