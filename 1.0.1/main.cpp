#include "functions.cpp"
#include "main.h"
#include "tray.cpp"
#include "volume.h"

//#pragma comment(lib,"ControlVolume.lib")

// FIXED - Wakeup bug fixed.

// TODO (Umakanth_2#1#): Emesene Support - 0%
// TODO (Umakanth_2#1#): Memory Leaks - FIXME - 70%
// TODO (Umakanth_2#1#): i18n Support - 0% - This Should !
// TODO (Umakanth_2#9#): WMP11 bug - 100% - Done!
// TODO (Umakanth_2#1#): Songbird Support - 40% - Check WinDBus !
// TODO (Umakanth_2#1#): Pidgin Support - 0% - Check WinDbus !
// TODO (Umakanth_2#1#): Live Messenger Support - 0% - Not possible as of now

using namespace std;
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,

                    int nCmdShow)
{
    HWND hwnd,hCheckWnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    memset( &wincl, 0, sizeof( wincl ) );

    hCheckWnd = GetRunningWindow(szClassName);

    if (hCheckWnd)   // hCheckWnd != NULL -> Previous instance exists
    {
        MessageBox(hCheckWnd, "MonitorES is already running. Check in System Tray ", "Information", MB_OK);
        AddTrayIcon( hCheckWnd, ID_TRAYICON, APPWM_TRAYICON, TRAY_ICON, TOOL_TIP );
        return FALSE;
    }
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    //wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    wincl.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               szTitleText,       /* Title Text */
               WS_DISABLED , /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               MAX_X,                 /* The programs width */
               MAX_Y,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    RegisterSession ( hwnd );

    ProgramOnLoad();

    bool gRet;

    //SetThreadExecutionState(ES_CONTINUOUS);
    //BOOL screenSaverActive;
    //SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &screenSaverActive, 0);
    //if (screenSaverActive)
    //SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, 0);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while ((gRet = GetMessage (&messages, NULL, 0, 0)) != 0 )
    {
        if (gRet == -1)
        {
            UnregisterSession(hwnd);
            RemoveTrayIcon( hwnd, ID_TRAYICON );
            PostQuitMessage (0);
        }
        else
        {
            TranslateMessage ( &messages );
            DispatchMessage ( &messages );

            if (messages.message == WM_HOTKEY)
            {

                if (messages.lParam == TURNOFF_LPARAM)
                {
                    TurnOffMonitor(hwnd);
                }
                if (messages.lParam == PAUSE_LPARAM || HIWORD ( messages.lParam ) == 0x13 )
                {
                   OneKeyMediaRunning();
                }
                if (messages.lParam == SCR_LPARAM)
                {
                    TurnOnScreenSaver(hwnd);
                }
                if (messages.lParam == MUTE_LPARAM)
                {
                    ToggleMasterVolume();
                }
                if (messages.lParam == OPEN_LPARAM)
                {
                    DialogBox ( GetModuleHandle ( NULL ), MAKEINTRESOURCE ( IDD_SETUP_DIALOG ), hwnd, reinterpret_cast<DLGPROC>(SetupDlgProc) );
                    b_DialogShowing = FALSE;
                }
            }

            /* Translate virtual-key messages into character messages */
            TranslateMessage(&messages);
            /* Send message to WindowProcedure */
            DispatchMessage(&messages);
        }

    }

    UnregisterHotKey ( NULL, PAUSE_HOTKEY );
    UnregisterHotKey ( NULL, TURN_OFF_HOTKEY );
    UnregisterHotKey ( NULL, TURN_SCR_HOTKEY );
    UnregisterHotKey ( NULL, MUTE_SYS_HOTKEY );

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_WTSSESSION_CHANGE:
        switch ( wParam )
        {
        case WTS_SESSION_LOCK:
            if(GLOBAL_OP_FLAG == 1)
            {
                if (IDC_MONITOR_FLAG) TurnOffMonitor(hwnd);
                if (IDC_SCREEN_FLAG) TurnOnScreenSaver(hwnd);
                if (IDC_PAUSE_MEDIA_FLAG) PauseMediaPrograms();
                if (IDC_MUTE_VOL_FLAG) MuteMasterVolume(true);
                ControlMessengers();
                GLOBAL_OP_FLAG = 0;
                break;
            }
        case WTS_SESSION_UNLOCK:
            if(GLOBAL_OP_FLAG == 1)
            {
                if (IDC_PAUSE_MEDIA_FLAG) ResetMediaRunning();
                if (IDC_MUTE_VOL_FLAG) MuteMasterVolume(false);
                RestoreMessengers();
                GLOBAL_OP_FLAG = 0;
                break;
            }
        default:
            GLOBAL_OP_FLAG = 1;
            break;
        }
    break;
    case WM_CREATE:
        LoadRegistry();
        if(!IDC_HIDE_TRAY_FLAG)
        {
            AddTrayIcon( hwnd, ID_TRAYICON, APPWM_TRAYICON, TRAY_ICON, TOOL_TIP );
        }
        break;
    case APPWM_NOP:
        break;
    case APPWM_TRAYICON:
        SetForegroundWindow( hwnd );
        switch ( lParam )
        {
        case WM_MOUSEMOVE:
            OnTrayIconMouseMove( hwnd );
            return 0;

        case WM_RBUTTONUP:
            //  There's a long comment in OnTrayIconRBtnUp() which
            //  explains what we're doing here.
            OnTrayIconRBtnUp( hwnd );
            return 0;

        case WM_LBUTTONDBLCLK:
            OnTrayIconLBtnDblClick( hwnd );
            return 0;
        }
        return 0;
   case WM_COMMAND:
        return OnCommand( hwnd, LOWORD(wParam), (HWND)lParam );

   case WM_INITMENUPOPUP:
        OnInitMenuPopup( hwnd, (HMENU)wParam, lParam );
        return 0;
   case WM_DESTROY:
        OnClose( hwnd );
        UnregisterSession(hwnd);
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    case WM_POWERBROADCAST:
       if(wParam == PBT_APMRESUMEAUTOMATIC || wParam == PBT_APMQUERYSUSPEND ) GLOBAL_OP_FLAG = 0;
       break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
