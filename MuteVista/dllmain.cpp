// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "MuteVista.h"
#include <objbase.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport)
void MuteAudio()
{
    HRESULT hr;
    CoInitialize(NULL);
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    if (!SUCCEEDED(hr)) 
    {
        CoUninitialize();
    }

    IMMDevice *defaultDevice = NULL;

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;
    if (!SUCCEEDED(hr))
    {
        CoUninitialize();
    }

    IAudioEndpointVolume *endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
    defaultDevice->Release();
    defaultDevice = NULL; 

    if (!SUCCEEDED(hr)) 
    {
        CoUninitialize();
    }

    // -------------------------

    endpointVolume->SetMute(TRUE, NULL);

    endpointVolume->Release();

    CoUninitialize();
}


extern "C" __declspec(dllexport)
void UnMuteAudio()
{
    HRESULT hr;

    CoInitialize(NULL);
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    if (!SUCCEEDED(hr)) 
    {
        CoUninitialize();
    }

    IMMDevice *defaultDevice = NULL;

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;
    if (!SUCCEEDED(hr))
    {
        CoUninitialize();
    }

    IAudioEndpointVolume *endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
    defaultDevice->Release();
    defaultDevice = NULL; 

    if (!SUCCEEDED(hr)) 
    {
        CoUninitialize();
    }

    // -------------------------

    endpointVolume->SetMute(FALSE, NULL);

    endpointVolume->Release();

    CoUninitialize();
} 


extern "C" __declspec(dllexport)
void SetMuteStatus()
{
    HRESULT hr;

    CoInitialize(NULL);
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    if (!SUCCEEDED(hr)) 
    {
        CoUninitialize();
    }

    IMMDevice *defaultDevice = NULL;

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;
    if (!SUCCEEDED(hr))
    {
        CoUninitialize();
    }

    IAudioEndpointVolume *endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
    defaultDevice->Release();
    defaultDevice = NULL; 

    if (!SUCCEEDED(hr)) 
    {
        CoUninitialize();
    }

    // -------------------------
	BOOL b;

	endpointVolume->GetMute(&b);
	b=!b;
	endpointVolume->SetMute(b, NULL);
	endpointVolume->Release();
    CoUninitialize();
}