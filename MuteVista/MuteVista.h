#ifndef INDLL_H
#define INDLL_H

#include <windows.h>

#define WIN32_LEAN_AND_MEAN

extern "C" __declspec(dllexport) void MuteAudio();
extern "C" __declspec(dllexport) void UnMuteAudio();
extern "C" __declspec(dllexport) void SetMuteStatus();

#endif