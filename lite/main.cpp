/*
   MonitorES Lite

   Copyright 2009 Umakanthan Chandran (cumakt@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions
   and limitations under the License.
   */

#define _UNICODE
#include <windows.h>
#include "tray.h"

#define NOTIFY_FOR_THIS_SESSION 0
#define WM_WTSSESSION_CHANGE 0x02B1
#define WTS_SESSION_LOCK                   0x7
#define WTS_SESSION_UNLOCK                 0x8

#define MAX_X 0
#define MAX_Y 0
#define START_X 20
HINSTANCE g_hinst;
//HDC hdc;
//HBRUSH hbr;

int ID_GLOBAL_FLAG = 0;
int AUTO_START_FLAG = 0;
int MIN_CHECK_FLAG = 0;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "MonitorES";
char szTitleText[ ] = "";

char * originalMessage;

int (__stdcall * WorkStationStatus)();


void alert(char *item)
{
    MessageBox(NULL, item,  "Message", MB_OK | MB_ICONINFORMATION);
}
/* Main Program call */

HWND GetRunningWindow()
{
    // Check if exists an application with the same class name as this application
    HWND hWnd = FindWindow(szClassName, NULL);
    if (IsWindow(hWnd))
    {
        HWND hWndPopup = GetLastActivePopup(hWnd);
        if (IsWindow(hWndPopup))
            hWnd = hWndPopup; // Previous instance exists
    }
    else hWnd = NULL; // Previous instance doesnt exist
    return hWnd;
}
void UnregisterSession(HWND hwnd)
{
    typedef DWORD (WINAPI *tWTSUnRegisterSessionNotification)( HWND,DWORD );

    tWTSUnRegisterSessionNotification pWTSUnRegisterSessionNotification=0;
    HINSTANCE handle = ::LoadLibrary("wtsapi32.dll");
    pWTSUnRegisterSessionNotification = (tWTSUnRegisterSessionNotification) ::GetProcAddress(handle,"WTSUnRegisterSessionNotification");
    if (pWTSUnRegisterSessionNotification)
    {
        pWTSUnRegisterSessionNotification(hwnd,NOTIFY_FOR_THIS_SESSION);
    }
    ::FreeLibrary(handle);

}

void RegisterSession(HWND hwnd)
{
    typedef DWORD (WINAPI *tWTSRegisterSessionNotification)( HWND,DWORD );

    tWTSRegisterSessionNotification pWTSRegisterSessionNotification=0;
    HINSTANCE handle = ::LoadLibrary("wtsapi32.dll");
    pWTSRegisterSessionNotification = (tWTSRegisterSessionNotification) :: GetProcAddress(handle,"WTSRegisterSessionNotification");
    if (pWTSRegisterSessionNotification)
    {
        pWTSRegisterSessionNotification(hwnd,NOTIFY_FOR_THIS_SESSION);
    }
    ::FreeLibrary(handle);
}

void LoadRegistrySettings()
{
    HKEY hKey;
    char rgValue[2];
    DWORD size1 = sizeof(rgValue);
    DWORD Type;
    char pathValue[MAX_PATH];
    DWORD size2 = sizeof(pathValue);

    RegOpenKey(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&hKey);
    if (ERROR_SUCCESS == RegQueryValueEx( hKey, TEXT("MonitorES"), NULL, &Type, (LPBYTE)pathValue,&size2))
    {
        AUTO_START_FLAG = 1;
    }

    RegCloseKey(hKey);
}

void SaveEnergyShort()
{
    Sleep(500); // Eliminate user's interaction for 500 ms
    if (ID_GLOBAL_FLAG == 0)
    {
        SendMessage(HWND_BROADCAST, WM_SYSCOMMAND,SC_MONITORPOWER, (LPARAM) 2);
    }
}

//main function
int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{

    HWND hwnd,hCheckWnd;        /* This is the handle for our window */
    MSG messages;        /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    hCheckWnd = GetRunningWindow();

    if (hCheckWnd)   // hCheckWnd != NULL -> Previous instance exists
    {
        MessageBox(hCheckWnd, "The program is already running", "Info", MB_OK);
        return FALSE;       // Exit program
    }
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;       /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;        /* No menu */
    wincl.cbClsExtra = 0;        /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;        /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    //wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    wincl.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

   // int desktopwidth=GetSystemMetrics(SM_CXSCREEN);
    //int desktopheight=GetSystemMetrics(SM_CYSCREEN);


    /* The class is registered, let's create the program*/
    hwnd = CreateWindow (
               szClassName,        /* Classname */
               szTitleText,        /* Title Text */
               WS_DISABLED  , /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               MAX_X,        /* The programs width */
               MAX_Y,        /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,         /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL         /* No Window Creation data */
           );

    RegisterSession(hwnd);

    RegisterHotKey(NULL,1,MOD_CONTROL,0x71);

    SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T) -1, (SIZE_T) -1);

    bool gRet;

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while ((gRet = GetMessage (&messages, NULL, 0, 0)) != 0 )
    {
        if (gRet == -1)
        {
            UnregisterSession(hwnd);
            OnDestroyTray();
            PostQuitMessage (0);
        }
        else
        {
            /* Translate virtual-key messages into character messages */
            TranslateMessage(&messages);
            /* Send message to WindowProcedure */
            DispatchMessage(&messages);
            if (messages.message == WM_HOTKEY)
            {
                SaveEnergyShort();
            }
        }
    }
    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPMINMAXINFO lpmmi;

    switch (message)       /* handle the messages */
    {
    case WM_WTSSESSION_CHANGE:
        switch ( wParam )
        {
        case WTS_SESSION_LOCK:
            SaveEnergyShort();
            break;
        case WTS_SESSION_UNLOCK:
            break;
        default:
            break;
        }
        break;
    case WM_CREATE:
    {

        LoadRegistrySettings();

        TRAY_Init(hWnd,AUTO_START_FLAG);

        OnTray(wParam);

    }
    break;
    case WM_KEYDOWN :
        if (wParam == VK_ESCAPE)
        {
            ShowWindow(hWnd, SW_SHOWMINIMIZED);
        }
        break;
    case WM_SIZE:
        OnSizeTray(wParam); // Minimize/Restore to/from tray
        break;
    case WM_NOTIFYICONTRAY:
        OnNotifyTray(lParam); // Manages message from tray
        return TRUE;
    case WM_COMMAND:
    {
        if (OnCommandTrayMenu(wParam)) break;
     }
    break;
    case WM_RBUTTONDOWN:
    {
        TRAY_Menu_Show();//load POPUP menu in main window (why?)
    }
    break;
    case WM_CLOSE:
        ShowWindow(hWnd, SW_SHOWMINIMIZED); // Minimize/Restore to/from tray
        break;
    case WM_DESTROY:
        UnregisterSession(hWnd);
        OnDestroyTray();
        PostQuitMessage (0);  /* send a WM_QUIT to the message queue */
        break;
    default: /* for messages that we don't deal with */
        return DefWindowProc (hWnd, message, wParam, lParam);
    }

    return 0;
}
