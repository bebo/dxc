#include "stdafx.h"
#include "MediaFoundation.h"
#include "MediaFoundationDebug.h"

using namespace std;

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

std::list<MediaDevice> MediaFoundation::GetVideoDevices()
{
	auto device_list = std::list<MediaDevice>();

	IMFAttributes *pAttributes = NULL;
	IMFActivate **ppDevices = NULL;

	// Create an attribute store to specify the enumeration parameters.
	HRESULT hr = MFCreateAttributes(&pAttributes, 1);

	// Source type: video capture devices
	hr = pAttributes->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
	);

	// Enumerate devices.
	UINT32 count;
	//WCHAR *friendlyName;
	UINT32 cchName;

	hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
	for (UINT i = 0; i < count; i++) {
		MediaDevice device;
		ppDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &device.pFriendlyName, &cchName);
		ppDevices[i]->ActivateObject(IID_PPV_ARGS(&device.pMediaSource));
		device_list.push_front(device);
	}

	return device_list;
}

//TODO: just gets the first device
void MediaFoundation::EnumerateVideoDevices(IMFMediaSource **pSource)
{
	IMFAttributes *pAttributes = NULL;
	IMFActivate **ppDevices = NULL;

	// Create an attribute store to specify the enumeration parameters.
	HRESULT hr = MFCreateAttributes(&pAttributes, 1);

	// Source type: video capture devices
	hr = pAttributes->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
	);

	// Enumerate devices.
	UINT32 count;
	WCHAR *friendlyName;
	UINT32 cchName;
	hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
	ppDevices[0]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &friendlyName, &cchName);
	OutputDebugString(friendlyName);
	CoTaskMemFree(friendlyName);
	ppDevices[0]->ActivateObject(IID_PPV_ARGS(pSource));
}

//TODO: just gets the first format
void MediaFoundation::EnumerateFormats(IMFMediaSource *pSource, IMFMediaType **type)
{
	IMFPresentationDescriptor *pPD = NULL;
	IMFStreamDescriptor *pSD = NULL;
	IMFMediaTypeHandler *pHandler = NULL;
	IMFMediaType *pType = NULL;

	HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
	if (FAILED(hr))
	{
		goto done;
	}

	BOOL fSelected;
	hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pSD->GetMediaTypeHandler(&pHandler);
	if (FAILED(hr))
	{
		goto done;
	}

	DWORD cTypes = 0;
	hr = pHandler->GetMediaTypeCount(&cTypes);
	if (FAILED(hr))
	{
		goto done;
	}

	for (DWORD i = 0; i < cTypes; i++)
	{
		hr = pHandler->GetMediaTypeByIndex(i, &pType);
		if (FAILED(hr))
		{
			goto done;
		}

		LogMediaType(pType);
		SafeRelease(&pType);
	}

done:
	SafeRelease(&pPD);
	SafeRelease(&pSD);
	SafeRelease(&pHandler);
	SafeRelease(&pType);
}

//TODO: just sets the first format
void MediaFoundation::SetDeviceFormat(IMFMediaSource * pSource)
{
	IMFPresentationDescriptor *pPD = NULL;
	IMFStreamDescriptor *pSD = NULL;
	IMFMediaTypeHandler *pHandler = NULL;
	IMFMediaType *pType = NULL;

	HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
	if (FAILED(hr))
	{
		goto done;
	}

	BOOL fSelected;
	hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pSD->GetMediaTypeHandler(&pHandler);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pHandler->GetMediaTypeByIndex(0, &pType);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pHandler->SetCurrentMediaType(pType);

done:
	SafeRelease(&pPD);
	SafeRelease(&pSD);
	SafeRelease(&pHandler);
	SafeRelease(&pType);
}

void MediaFoundation::CreateSourceReader(IMFMediaSource * pSource, IMFSourceReader ** pReader)
{
	IMFAttributes *attributes = NULL;
	MFCreateAttributes(&attributes, 2);
	attributes->SetUINT32(MF_LOW_LATENCY, 1);
	attributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, 1);
	MFCreateSourceReaderFromMediaSource(pSource, attributes, pReader);
}
