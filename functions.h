#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <windows.h>
#include <sstream>
#include <tlhelp32.h>
#include <commctrl.h>
//#include <psapi.h>
#include "iTunesCOMInterface.h"
#include "iTunesCOMInterface_i.c"
//#include <string.h>
#include <powrprof.h>
#include <winable.h>

#include "VolumeControl.cpp"

#define _UNICODE
#pragma once

#ifndef WINVER
#define WINVER 0x0500
#endif
#define WIN32_LEAN_AND_MEAN

#define NOTIFY_FOR_THIS_SESSION 0
#define WIN32_LEAN_AND_MEAN
#define WTS_CONSOLE_CONNECT                0x1
#define WTS_CONSOLE_DISCONNECT             0x2
#define MAXMODULE 50
#define WM_BSP_CMD WM_USER+2
#define BSP_Play 20
#define BSP_Pause 21
//#define AWAY_MESSAGE "Away"
#define PAUSE_HOTKEY 672
#define TURN_OFF_HOTKEY 673
#define TURN_SCR_HOTKEY 674
#define MUTE_SYS_HOTKEY 675

#define OPEN_PRE_HOTKEY 676


#define ES_SYSTEM_REQUIRED  0x00000001
#define ES_DISPLAY_REQUIRED  0x00000002
#define ES_USER_PRESENT     0x00000004
#define ES_CONTINUOUS     0x80000000
#define ES_AWAYMODE_REQUIRED 0x00000040


typedef ULONG EXECUTION_STATE;

int PAUSED_TUNES = 0;
int PAUSED_WINAMP = 0;
int WMP_CLASSIC_RUN = 0;
int size;
int SB_ID_COUNT = 0;
int actionCodeNum;
int actionItemNum;

static bool bDialogShowing = false;
char AWAY_MESSAGE[] = "Away";
char EMPTY_AWAY_MSG[] = "";

//char *COPY_AWAY_MESSAGE = "Available";
bool GL_MESSENGE_FLAG = false;

//char *originalMessage;
char *digsbyMessage;
char RegistryPath[] = "Software\\MonitorES";
//const char *NoHours[] = { "1", "2", "3","4", "5"};
//const char *DoAction[] = { "Logoff", "Sleep", "Shutdown"};

char YahooName[32];
char AwayMessage[128];
//char user[32];
//char awaymsg[128];
//char actionText[16];
//char actionNum[16];
//char actionItem[16];
//char getValue[2];
//char regSaveData[256];

TCHAR szPath[MAX_PATH];

DWORD valSize = 2;
DWORD regType = 0;
DWORD dataSize;
DWORD typeData;

LPARAM TURNOFF_LPARAM;
LPARAM PAUSE_LPARAM;
LPARAM SCR_LPARAM;
LPARAM MUTE_LPARAM;
LPARAM OPEN_LPARAM;

UINT_PTR timer = 0;
BOOL sysFlag = FALSE;

//functions
int StringLength(char inputString[]);
HWND GetRunningWindow(char szClassName[]);
void UnregisterSession(HWND hwnd);
void ControlDialogCheck(HWND hwnd,UINT item ,boolean flag);
BYTE GetVK(WORD key);
BYTE GetModifier(WORD key);
long SendMouseEvent ( HWND hwnd, long Msg, int X, int Y );


Mixer mixer;

#endif // FUNCTIONS_H_INCLUDED
