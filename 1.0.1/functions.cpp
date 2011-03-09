#include "functions.h"
#include "tray.h"
#include "log.h"


//Global Variable
template<typename TT>
void alert(char * str,TT item)
{
    std::ostringstream os;
    os << item;
    MessageBoxA(NULL, os.str().c_str(),  str, MB_OK | MB_ICONINFORMATION);
}

/*void SystemMessage()
{
    LPVOID lpMsgBuf;
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL,
                   ::GetLastError(),
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                   (LPTSTR) &lpMsgBuf,
                   0,
                   NULL );// Process any inserts in lpMsgBuf.
    MessageBox( NULL, (LPTSTR)lpMsgBuf, "Message", MB_OK | MB_ICONSTOP);
}*/

void DisableScreenSaver(bool flag)
{
    BOOL screenSaverActive;
    if (flag)
        SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, 0);
     else
        SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, 0);
//    SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &screenSaverActive, 0);
}
//Finds the string length
int StringLength(char inputString[])
{
    int length = 0;
    for (int i = 0; inputString[i]!= '\0'; i++)
        length++;
    return length;
}

//Check if exists an application with the same class name as this application
HWND GetRunningWindow(char szClassName[])
{
    HWND hWnd = FindWindow ( szClassName, NULL );
    if ( IsWindow ( hWnd ) )
    {
        HWND hWndPopup = GetLastActivePopup ( hWnd );
        if ( IsWindow ( hWndPopup ) )
            hWnd = hWndPopup;
    }
    else
        hWnd = NULL;
    return hWnd;
}

//Unregister WTSAPI32.dll to get notification
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

//Register WTSAPI32.dll to get notification
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

// Get the actual path of running application

char *GetProcessPath( DWORD dwPID , char* sExePath)
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );

    // Set the size of the structure before using it.
    me32.dwSize = sizeof( MODULEENTRY32 );

    // Now walk the module list of the process,
    // and display information about each module
    int counter = 0;
    do
    {
        if (counter == 1) sprintf(sExePath, TEXT("%s"),me32.szExePath);
        counter++;
    }
    while ( Module32Next( hModuleSnap, &me32 ));

    CloseHandle( hModuleSnap );
    return sExePath;
}

//compare two string.
int cmpistr(const char* s1, const char* s2)
{
    int c1, c2;
    int cmp = 0;
    if (s1 && s2)
        for (;;)
        {
            c1 = *s1++;
            c2 = *s2++;
            if (c1 && c2)
            {
                c1 = tolower(c1)&0xFF; // 8 bits
                c2 = tolower(c2)&0xFF; // only
                if (c1 < c2)
                {
                    cmp = -1;
                    break;
                }
                else if (c1 > c2)
                {
                    cmp = 1;
                    break;
                }
            }
            else
            {
                if (c1)
                    cmp = 1;
                else if (c2)
                    cmp = -1;
                break;
            }
        }
    return cmp;
}

/* Method to control Foobar media player*/
void ControlFooBar()
{
    PROCESSENTRY32 *entry = new PROCESSENTRY32;
    entry->dwFlags = sizeof( PROCESSENTRY32 );
    HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    HANDLE hProcess;
    TCHAR *filePath = new TCHAR[MAX_PATH];
    if ( Process32First( snapshot, entry ) == TRUE )
    {
        while ( Process32Next( snapshot, entry ) == TRUE )
        {
            if ( cmpistr( entry->szExeFile, "foobar2000.exe" ) == 0 )
            {
                ShellExecute (NULL, "open", GetProcessPath( entry->th32ProcessID, filePath ), "/playpause", NULL, SW_HIDE);
            }
        }
    }
    /*while ( Process32Next( snapshot, entry ) != FALSE ) {
        std::string s(entry->szExeFile);
        if(s == "foobar2000.exe")
        {
            ShellExecute (NULL, "open", GetProcessPath( entry->th32ProcessID, filePath ), "/playpause", NULL, SW_HIDE);
        }
        s.clear();
    }*/

    CloseHandle( snapshot );
    delete []entry;
    delete []filePath;
}

//Method to return true/false for Windows vista
bool IsWindowsVista()
{
    OSVERSIONINFO osvi;
    bool bIsWindowsXPorLater;

    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx (&osvi);
    bIsWindowsXPorLater =
        ( (osvi.dwMajorVersion > 6) ||
          ( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 0) ));

    return bIsWindowsXPorLater;
}

void AutoStartWithWindows ( bool setFlag )
{
    try
    {
        HKEY newValue;
        RegOpenKey ( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &newValue );
        if ( setFlag )
        {
            GetModuleFileName ( NULL, szPath, MAX_PATH );
            RegSetValueEx ( newValue, "MonitorES", 0, REG_SZ, ( LPBYTE ) szPath, sizeof ( szPath ) );
            throw 1;
        }
        else
        {
            RegDeleteValue ( newValue, "MonitorES" );
        }
        RegCloseKey ( newValue );
    }
    catch ( int i ) {}
}

void SleepMode()
{
    SetSuspendState(false,true,false);
}


void TurnOffMonitor(HWND hwnd)
{
    Sleep(500); // Eliminate user's interaction for 500 ms
    SendMessage(hwnd, WM_SYSCOMMAND,SC_MONITORPOWER, (LPARAM)2);
}

void TurnOnScreenSaver(HWND hwnd)
{
    Sleep(500); // Eliminate user's interaction for 500 ms
    SendMessage ( hwnd, WM_SYSCOMMAND, SC_SCREENSAVE, ( LPARAM ) 0 );
}

void ToggleMuteVista()
{
    typedef void (WINAPI*cfunc)();
    cfunc SetMuteStatus;
    char *mod = new char[256];
    HINSTANCE hLib = ::LoadLibrary("MuteVista.dll");
    GetModuleFileName((HMODULE)hLib, (LPTSTR)mod, MAXMODULE);

    if (hLib != NULL)
    {
        SetMuteStatus=(cfunc)GetProcAddress((HMODULE)hLib, "SetMuteStatus");
        if (SetMuteStatus != NULL)
        {
            SetMuteStatus();
        }
    }
    ::FreeLibrary(hLib);
    delete [] mod;
}


void MuteVista(bool flagMute)
{
    typedef void (WINAPI*cfunc)();
    cfunc MuteAudio,UnMuteAudio;
    char *mod = new char[256];
    HINSTANCE hLib = ::LoadLibrary("MuteVista.dll");
    GetModuleFileName((HMODULE)hLib, (LPTSTR)mod, MAXMODULE);
    if (hLib != NULL)
    {
        MuteAudio=(cfunc)GetProcAddress((HMODULE)hLib, "MuteAudio");
        UnMuteAudio=(cfunc)GetProcAddress((HMODULE)hLib, "UnMuteAudio");
        if (flagMute && MuteAudio != NULL) MuteAudio();
        if (!flagMute && UnMuteAudio != NULL) UnMuteAudio();
    }
    ::FreeLibrary(hLib);
    delete [] mod;
}


/*Control method to mute sound using WinMM header */
void ControlSound ( bool flagMute )
{
    if ( mixer.init() && mixer.GetMuteControl() )
    {
        if ( flagMute )
            mixer.SetMute ( true );
        else
            mixer.SetMute ( false );
    }
    mixer.close();
}

void ToggleMasterVolume()
{
    if (IsWindowsVista())
    {
        ToggleMuteVista();
    }
    else
    {
        if ( mixer.init() && mixer.GetMuteControl() )
        {
            bool val;
            mixer.GetMuteStatus(val);
            if (val)
                mixer.SetMute(false);
            else
                mixer.SetMute(true);
        }
        mixer.close();
    }

}

void MuteMasterVolume(bool flagMute)
{
    if (IsWindowsVista())
    {
        MuteVista(flagMute);
    }
    else
    {
        ControlSound(flagMute);
    }

}


/*Simple HTTP Request methos using Wininet header*/

/*void XMLHTTPRequest ( char *requesturl )
{
    HINTERNET Internet = InternetOpen ( "URL", INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0 );
    unsigned long lTimeOut = 100;
    InternetSetOption ( Internet, INTERNET_OPTION_RECEIVE_TIMEOUT, &lTimeOut, sizeof ( long ) );
    if ( !Internet ) { }
        InternetOpenUrl ( Internet, requesturl, 0, 0, 0, 0 );
    ::InternetCloseHandle ( Internet );
}*/


/* Method to control VLC Media Player using HTTP Interface*/

void ControlVLC()
{


}

/*void ControlVLC()
{
    HINTERNET Internet = InternetOpen ( "URL", INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0 );
    unsigned long lTimeOut = 100;
    InternetSetOption ( Internet, INTERNET_OPTION_RECEIVE_TIMEOUT, &lTimeOut, sizeof ( long ) );
    int status = 0;
    if ( !Internet ) { }
    HINTERNET Url = InternetOpenUrl ( Internet, "http://localhost:8080/requests/status.xml?command=pl_status", 0, 0, 0, 0 );
    if ( !Url ) { }
    char Buffer[64*1024];
    DWORD ReadLen = 0, CurrentPos = 0;
    do
    {
        InternetReadFile ( Url, ( void * ) ( Buffer + CurrentPos ), sizeof ( Buffer ) - CurrentPos, &ReadLen );
        CurrentPos += ReadLen;
    }
    while ( ReadLen );
    Buffer[CurrentPos] = 0;

    std::string strText = Buffer;
    size_t pause, play;
    pause = strText.find ( "paused" );
    if ( pause != std::string::npos )
    {
        status = 1;
    }
    play = strText.find ( "playing" );
    if ( play != std::string::npos )
    {
        status = 2;
    }
    switch ( status )
    {
    case 1:
        XMLHTTPRequest ( "http://localhost:8080/requests/status.xml?command=pl_play" );
        break;
    case 2:
        XMLHTTPRequest ( "http://localhost:8080/requests/status.xml?command=pl_pause" );
        break;
    }
    ::InternetCloseHandle ( Internet );
}*/

void ControlTunes ()
{
    HRESULT hRes;
    CoInitialize ( 0 );
    IiTunes *iITunes = 0;
    hRes = ::CoCreateInstance ( CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, ( PVOID * ) & iITunes );
    if ( hRes == S_OK && iITunes )
    {
        ITPlayerState iIPlayerState;
        iITunes->get_PlayerState ( &iIPlayerState );
        if(iIPlayerState == ITPlayerStatePlaying)
        {
            iITunes->Pause();
        } else if (iIPlayerState == ITPlayerStateStopped )
        {
            iITunes->Play();
        }
        iITunes->Release();
    }
    CoUninitialize();
}

//iTunes Control
void ControlTunesOnLock ()
{
    HRESULT hRes;
    CoInitialize ( 0 );
    IiTunes *iITunes = 0;
    hRes = ::CoCreateInstance ( CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, ( PVOID * ) & iITunes );
    if ( hRes == S_OK && iITunes )
    {
        ITPlayerState iIPlayerState;
        iITunes->get_PlayerState ( &iIPlayerState );
        if(iIPlayerState == ITPlayerStatePlaying)
        {
            PAUSED_TUNES = 1;
            iITunes->Pause();
        }
        iITunes->Release();
    }
    CoUninitialize();
}

//iTunes Control
void ControlTunesReset ()
{
    //LOG_DECLARE;
    //LOG_INIT("main.log");
    HRESULT hRes;
    CoInitialize ( 0 );
    IiTunes *iITunes = 0;
    hRes = ::CoCreateInstance ( CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, ( PVOID * ) & iITunes );
    if ( hRes == S_OK && iITunes )
    {
        ITPlayerState iIPlayerState;
        iITunes->get_PlayerState ( &iIPlayerState );
        if(iIPlayerState == ITPlayerStateStopped && PAUSED_TUNES == 1)
        {
            PAUSED_TUNES = 0;
            iITunes->Play();
        }
        iITunes->Release();
    }
    CoUninitialize();
}



BOOL CALLBACK EnumSongBirdChildProc ( HWND hwndChild, LPARAM lParam )
{
    //SendMouseEvent ( hwndChild, WM_LBUTTONDOWN, 67, 36 );
    //SendMouseEvent ( hwndChild, WM_LBUTTONUP, 67, 36 );
    SB_ID_COUNT++;
    return TRUE;
}

void ControlWinamp()
{
    //Winamp, AIMP, MediaMonkey
    HWND hwndWinamp = FindWindow ( "Winamp v1.x", NULL ); //Finding window
    if ( hwndWinamp != NULL )
    {
        int ret = SendMessage ( hwndWinamp, WM_USER, 0, 104 );
        switch ( ret )
        {
        case 1: // its playing
            SendMessage ( hwndWinamp, WM_COMMAND, 40046, 1 ); //pause
            break;
        case 3: // its paused
            SendMessage ( hwndWinamp, WM_COMMAND, 40045, 1 ); //play
            break;
        default:
            break;
        }
    }

}

void SkypeChange()
{
   HWND hwndSkype = FindWindow("tSkMainForm.UnicodeClass",NULL);
   if (hwndSkype != NULL)
   {
       COPYDATASTRUCT CopyData;
       char cStr[30];
       lstrcpy(cStr, "SET USERSTATUS ");
       lstrcat(cStr, "AWAY");
       //case ID_STATUS_ONLINE: lstrcat(cStr, "ONLINE");break;
       CopyData.dwData=0;
       CopyData.lpData=cStr;
       CopyData.cbData=strlen(cStr)+1;
       SendMessage(hwndSkype, WM_COPYDATA, (WPARAM)hwndSkype, (LPARAM)&CopyData);
    }
}

void ControlWMP()
{
    //Windows media player control
    HWND hwndWMP = FindWindow ( "WMPlayerApp", NULL );
    if ( hwndWMP != NULL )
    {
        SendMessage ( hwndWMP, WM_COMMAND, 0x00004978, 1 ); // Pause/play
    }


}
void Control1by1()
{
    //1by1 Player
    HWND hwndBY1 = FindWindow ( "1by1WndClass", NULL );
    if ( hwndBY1 != NULL )
    {
        SendMessage ( hwndBY1,  WM_COMMAND, 0x000001FF, 0x000904A8 ); // Pause/play
    }
}

void ControlSpotify()
{
    //Spotify Player
    /*
    CMD_PLAYPAUSE   = 917504
    CMD_MUTE        = 524288
    CMD_VOLUMEDOWN  = 589824
    CMD_VOLUMEUP    = 655360
    CMD_STOP        = 851968
    CMD_PREVIOUS    = 786432
    CMD_NEXT        = 720896
    */
    HWND hwndSP = FindWindow ( "SpotifyMainWindow", NULL );
    if ( hwndSP != NULL )
    {
        SendMessage ( hwndSP,  0x0319, 0, 917504 );
    }

}

void ControlBS()
{
    //BSPlayer
    HWND hwndBS = FindWindow ( "BSPlayer", NULL );
    if ( hwndBS != NULL )
    {
        SendMessage ( hwndBS, WM_BSP_CMD, BSP_Pause , 0 );
    }
}

void ControlQuickP()
{
    //QuickTime Player
    HWND hwndQTP = FindWindow ( "QuickTimePlayerMain", NULL );
    if ( hwndQTP != NULL )
    {
        ShowWindow ( hwndQTP, SW_RESTORE );
        SetFocus ( hwndQTP );
        SetActiveWindow ( hwndQTP );
        SendMessage ( hwndQTP, WM_KEYDOWN, VK_SPACE, 0 );
        SendMessage ( hwndQTP, WM_KEYUP, VK_SPACE, 0 );
        ShowWindow ( hwndQTP, SW_SHOWNOACTIVATE );
    }
}

void ControlGOM()
{
    //GOMPlayer
    HWND hwndGOM = FindWindow ( "GomPlayer1.x", NULL );
    if ( hwndGOM != NULL )
    {
        SendMessage ( hwndGOM, WM_COMMAND, 0x0001800C, 0 ); // Pause/play
    }
}

void ControlAl()
{
    //ALShow Player
    HWND hwndALS = FindWindow ( "ALShowMainWindow", NULL );
    if ( hwndALS != NULL )
    {
        SendMessage ( hwndALS, WM_COMMAND, 0x00018154, 0 ); // Pause/play
    }

}


void ControlWMPC()
{
    //Media Player Classic Player
    HWND hwndWMPC = FindWindow ( "MediaPlayerClassicW", NULL );
    if ( hwndWMPC != NULL )
    {
        if ( WMP_CLASSIC_RUN == 0 )
        {
            SendMessage ( hwndWMPC,  WM_COMMAND, 0x0000009B, 0x0004030E ); //Play
            WMP_CLASSIC_RUN = 1;
        }
        else
        {
            SendMessage ( hwndWMPC,  WM_COMMAND, 0x0000009C, 0x0004030E ); //Pause
            WMP_CLASSIC_RUN = 0;
        }
    }

}

void ControlXM()
{
    //XMplay
    HWND hwndXMP = FindWindow ( "XMPLAY-MAIN", NULL );
    if ( hwndXMP != NULL )
    {
        ShowWindow ( hwndXMP, SW_RESTORE );
        SetFocus ( hwndXMP );
        SetActiveWindow ( hwndXMP );
        SendMessage ( hwndXMP, WM_KEYDOWN, 0x50, 0 );
        SendMessage ( hwndXMP, WM_KEYUP, 0x50, 0 );
        ShowWindow ( hwndXMP, SW_SHOWNOACTIVATE );
    }
}

void ControlSongB()
{

    //Songbird
    HWND hwndSB = FindWindow ( "MozillaUIWindowClass", "Songbird");
    if ( hwndSB != NULL )
    {
        ShowWindow ( hwndSB, SW_RESTORE );
        SetFocus ( hwndSB );
        SetActiveWindow ( hwndSB );

        HWND hwndSBChild = FindWindowEx ( hwndSB, NULL, TEXT ( "MozillaWindowClass" ), NULL );

        SendMessage ( hwndSBChild, WM_IME_KEYDOWN, VK_RETURN, 0 );
        SendMessage ( hwndSBChild, WM_IME_KEYDOWN, VK_RETURN, 0 );

        SendMessage ( hwndSBChild, WM_KEYDOWN, VK_SPACE, 0 );
        SendMessage ( hwndSBChild, WM_KEYUP, VK_SPACE, 0 );

        //if (hwndSBChild != NULL)
        //{
        //}
        //SendMessage(hwndSB, WM_CHAR, VK_SPACE, 0);
        // ShowWindow ( hwndSB, SW_SHOWNOACTIVATE );
        /*HWND hwndSBChild = FindWindowEx ( hwndSB, NULL, TEXT ( "MozillaWindowClass" ), NULL );
        if (hwndSBChild != NULL)
        {
            EnumChildWindows(hwndSBChild , EnumSongBirdChildProc , 0);
        }*/

    }
}

void OneKeyMediaRunning()
{
    ControlWinamp();
    ControlWMP();

    //iTunes control using COM
    HWND hwndTunes = FindWindow ( "iTunes", NULL );
    if ( hwndTunes != NULL )
        ControlTunes();

    Control1by1();
    ControlSpotify();
    ControlBS();

    //VLC Player
    HWND hwndVLC = FindWindow ( "QWidget", NULL );
    if ( hwndVLC != NULL )
        ControlVLC();

    ControlQuickP();
    ControlGOM();
    ControlAl();
    ControlWMPC();
    ControlFooBar();
    ControlXM();
    ControlSongB();
}


void PauseMediaPrograms()
{
    if (IDC_PAUSE_MEDIA_FLAG)
    {
        HWND hwndWinamp = FindWindow ( "Winamp v1.x", NULL ); //Finding window
        if ( hwndWinamp != NULL )
        {
            int ret = SendMessage ( hwndWinamp, WM_USER, 0, 104 );
            switch ( ret )
            {
            case 1: // its playing
                PAUSED_WINAMP = 1;
                SendMessage ( hwndWinamp, WM_COMMAND, 40046, 1 ); //pause
                break;
            default:
                break;
            }
            // Pause/play
        }

        ControlWMP();

        Control1by1();
        ControlSpotify();
        ControlBS();

        HWND hwndVLC = FindWindow ( "QWidget", NULL );
        if ( hwndVLC != NULL )
            ControlVLC();

        ControlQuickP();
        ControlGOM();
        ControlAl();

        HWND hwndWMPC = FindWindow ( "MediaPlayerClassicW", NULL );
        if ( hwndWMPC != NULL )
        {
            SendMessage ( hwndWMPC,  WM_COMMAND, 0x0000009C, 0x0004030E );
        }

        ControlFooBar();
        ControlXM();
        ControlSongB();

        //iTunes control using COM
        HWND hwndTunes = FindWindow ( "iTunes", NULL );
        if ( hwndTunes != NULL )
            ControlTunes();

    }
}

void ResetMediaRunning()
{
    if (IDC_PAUSE_MEDIA_FLAG)
    {
        HWND hwndWinamp = FindWindow ( "Winamp v1.x", NULL ); //Finding window
        if ( hwndWinamp != NULL )
        {
            int ret = SendMessage ( hwndWinamp, WM_USER, 0, 104 );
            switch ( ret )
            {
            case 3: // its paused
                if (PAUSED_WINAMP == 1)
                {
                    SendMessage ( hwndWinamp, WM_COMMAND, 40045, 1 ); //play
                    PAUSED_WINAMP = 0;
                }
                break;
            default:
                break;
            }
            // Pause/play
        }

        ControlWMP();

        //iTunes control using COM
        HWND hwndTunes = FindWindow ( "iTunes", NULL );
        if ( hwndTunes != NULL )
            ControlTunes();

        Control1by1();
        ControlSpotify();
        ControlBS();

        //VLC Player
        HWND hwndVLC = FindWindow ( "QWidget", NULL );
        if ( hwndVLC != NULL )
            ControlVLC();

        ControlQuickP();
        ControlGOM();
        ControlAl();

        HWND hwndWMPC = FindWindow ( "MediaPlayerClassicW", NULL );
        if ( hwndWMPC != NULL )
        {
            SendMessage ( hwndWMPC,  WM_COMMAND, 0x0000009B, 0x0004030E );
        }

        ControlFooBar();
        ControlXM();
        ControlSongB();

    }
}


//Copyright Viktor Brange AKA Vikke
//You are allowed to change the code :P
//And don't forget to link to CustomizeTalk.com
//Thanks to Wumpus who made ChangeStatus for VB.

long SendMouseEvent ( HWND hwnd, long Msg, int X, int Y )
{
    LPARAM lParam =
        MAKELPARAM ( X * GetDeviceCaps ( GetDC ( NULL ), LOGPIXELSX ) / 1440,
                     Y * GetDeviceCaps ( GetDC ( NULL ), LOGPIXELSY ) / 1440 );

    return SendMessage ( hwnd, Msg, 0, lParam );
}

void mycpy(char *to, char *from)
{
  while(*from)
      *to++ = *from++;

  *to = '\0'; /* null terminates the string */
}

char * GetCaption ( HWND hwnd )
{
   WPARAM length = SendMessage( hwnd, WM_GETTEXTLENGTH, 0, 0 );
   char *text = new char[256];
   if( length > 0 )
   {
      char *buffy = new char [length+1];
      LRESULT got = SendMessage( hwnd, WM_GETTEXT, length+1, (LPARAM)buffy );
      if( (LRESULT)length == got )
         mycpy(text,buffy);
      delete [] buffy;
   }
   return text;
}



//Change the status of GTalk
void GTalkChangeStatus ( char * strText, bool Polygamy )
{
    HWND hwndMain = NULL;
    HWND hwndMainView = NULL;
    HWND hwndStatusView2 = NULL;
    HWND hwndRichEdit = NULL;
    HWND hwndSidebar = NULL;
    HWND hwndPluginHost = NULL;
    HWND hwndATL = NULL;

    do
    {

        // Find Google Talk window
        hwndMain = FindWindowEx ( NULL, hwndMain,
                                  TEXT ( "Google Talk - Google Xmpp Client GUI Window" ), NULL );

        if ( hwndMain == NULL )
        {

            //Check for sidebar
            hwndSidebar = FindWindowEx ( NULL, hwndSidebar, TEXT ( "_GD_Sidebar" ), NULL );
            if ( hwndSidebar == NULL )
                break;
            hwndPluginHost = FindWindowEx ( hwndSidebar, NULL, TEXT ( "PluginHost" ), NULL );
            if ( hwndPluginHost == NULL )
                break;

            hwndATL = FindWindowEx ( hwndPluginHost, NULL, TEXT ( "ATL:017DC0C0" ), NULL );
            if ( hwndATL == NULL )
                break;

            hwndMain = FindWindowEx ( hwndATL, NULL,
                                      TEXT ( "Google Talk - Google Xmpp Client GUI Window" ), NULL );
            if ( hwndMain == NULL )
                break;
        }

        //Find status field
        hwndMainView = FindWindowEx ( hwndMain, NULL, TEXT ( "Main View" ), NULL );
        if ( hwndMainView == NULL )
            break;

        hwndStatusView2 = FindWindowEx ( hwndMainView, NULL, TEXT ( "Status View 2" ), NULL );
        if ( hwndStatusView2 == NULL )
            break;

        hwndRichEdit = FindWindowEx ( hwndStatusView2, NULL, TEXT ( "RichEdit20W" ), NULL );
        if ( hwndRichEdit == NULL )
            break;

        char * OriginalMessage = GetCaption(hwndRichEdit);
        //Change status if not empty
        if (strcmp(OriginalMessage,strText))
        {
            //COPY_AWAY_MESSAGE = OriginalMessage;
            //strcpy(OriginalMessage,COPY_AWAY_MESSAGE);
            GL_MESSENGE_FLAG = true;
            SendMouseEvent ( hwndStatusView2, WM_LBUTTONDOWN, 350, 400 );
            SendMouseEvent ( hwndStatusView2, WM_LBUTTONUP, 350, 400 );
            SendMessage ( hwndRichEdit, WM_SETTEXT, 0, ( LPARAM ) strText );
            SendMessage ( hwndStatusView2, WM_IME_KEYDOWN, VK_RETURN, 0 );
        } else {
            GL_MESSENGE_FLAG = false;
        }

        //Application->ProcessMessages();
        //Do this multiple times
        if ( Polygamy == false )
            break;
    }
    while ( true );
}

BOOL CALLBACK EnumChildProc ( HWND hwndChild, LPARAM lParam)
{
    int len = GetWindowTextLength ( hwndChild );

    if ( len > 0 )
    {
        char *buf = ( char* ) GlobalAlloc ( GPTR, len + 1 );
        GetWindowText ( hwndChild, buf, len + 1 );
        if (strcmp(buf,"Window") == 0)
        {
            HWND EditControl = FindWindowEx( hwndChild , NULL , "Edit", NULL );
            if (EditControl != NULL )
            {
                SendMessage ( EditControl, WM_SETTEXT, 0, ( LPARAM ) digsbyMessage);
                SendMouseEvent ( EditControl, WM_LBUTTONDOWN, 50, 14 );
                SendMouseEvent ( EditControl, WM_LBUTTONDOWN, 50, 14 );
                SendMouseEvent ( hwndChild, WM_LBUTTONDOWN, 50, 14 );
                SendMouseEvent ( hwndChild, WM_LBUTTONDOWN, 50, 14 );
                SendMessage ( EditControl, WM_IME_KEYDOWN, VK_RETURN, 0 );
                SendMessage ( hwndChild, WM_IME_KEYDOWN, VK_RETURN, 0 );

            }
        }
        GlobalFree ( ( HANDLE ) buf );
    }
    return TRUE;
}


//Method to change away status Digsby
void ControlDigsby(char* text)
{

    digsbyMessage = new char[256];
    digsbyMessage = text;
    HWND hwndDBY1 = FindWindow(NULL,"Buddy List");
    if (hwndDBY1 != NULL)
    {
        HWND hwndDBY2 = FindWindowEx ( hwndDBY1, NULL, TEXT ( "wxWindowClassNR" ), NULL );
        if (hwndDBY2 != NULL)
        {
            HWND hwndDBY3 = FindWindowEx ( hwndDBY2, NULL, TEXT ( "wxWindowClass" ), NULL );
            if (hwndDBY3 != NULL)
            {
                HWND hwndDBY4 = FindWindowEx ( hwndDBY3, NULL, TEXT ( "wxWindowClassNR" ), NULL );
                if (hwndDBY4 != NULL)
                {
                    EnumChildWindows(hwndDBY4 , EnumChildProc , 0);
                }
            }
        }
    }
    delete [] digsbyMessage;
}


/*void SetAwayEmesene(char* strStatus)
{
    HWND hwndEme = FindWindow(NULL,"emesene");
    if (hwndEme != NULL)
    {
        EnumChildWindows(hwndEme , EnumChildProcEme , 0);

        HWND EditControl = GetWindow ( hwndEme, GW_CHILD );
         if (EditControl)
         {
             SendMouseEvent ( EditControl, WM_LBUTTONDOWN, 50, 14 );
             SendMouseEvent ( EditControl, WM_LBUTTONUP, 50, 14 );
             SendMessage ( EditControl, WM_SETTEXT, 0, ( LPARAM ) strStatus);

             SendMouseEvent ( hwndEme, WM_LBUTTONDOWN, 50, 14 );
             SendMouseEvent ( hwndEme, WM_LBUTTONUP, 50, 14 );
             SendMessage ( EditControl, WM_IME_KEYDOWN, VK_RETURN, 0 );
             SendMessage ( hwndEme, WM_IME_KEYDOWN, VK_RETURN, 0 );
         }
    }
}*/

//Method to change away status Miranda IM
void SetAwayMirandaIM(char* strStatus)
{
    HWND hwndMirada = FindWindow("Miranda",NULL);
    if (hwndMirada != NULL)
    {
        SendMessage ( hwndMirada, WM_COMMAND, 0x0000405F, 0 ); // set away MSN
        //HWND hwndSetAway = FindWindow(NULL , "Change Online Message");
        HWND hwndSetAway = FindWindow("#32770", NULL);
        if (hwndSetAway != NULL)
        {
            HWND hwndMIMEdit = FindWindowEx( hwndSetAway , NULL , "Edit", NULL );
            if (hwndMIMEdit != NULL)
            {
                SendMessage ( hwndMIMEdit, WM_SETTEXT, 0, ( LPARAM ) strStatus );
            }
        }
    }
}
//Method to reset away status Digsby
void ResetMirandaIM(char* strStatus)
{
    HWND hwndMirada = FindWindow("Miranda",NULL);
    if (hwndMirada != NULL)
    {
        SendMessage ( hwndMirada, WM_COMMAND, 0x0000405E, 0 ); // set away MSN
        HWND hwndSetAway = FindWindow("#32770", NULL);
        if (hwndSetAway != NULL)
        {
            HWND hwndMIMEdit = FindWindowEx( hwndSetAway , NULL , "Edit", NULL );
            if (hwndMIMEdit != NULL)
            {
                SendMessage ( hwndMIMEdit, WM_SETTEXT, 0, ( LPARAM ) strStatus );
            }
        }
    }
}

//Yahoo Messenger status control
void YahooStatusSet ( LPCTSTR szMessage , LPTSTR szProfile )
{
    //Copyright (C) GPL by Khoa Nguyen <instcode (at) gmail.com>
    HKEY hRootKey;
    TCHAR *lpszKeyPath= new TCHAR[256];
    wsprintf ( lpszKeyPath, "Software\\yahoo\\pager\\profiles\\%s\\Custom Msgs\\", szProfile );
    RegOpenKeyEx ( HKEY_CURRENT_USER, lpszKeyPath, 0, KEY_READ | KEY_SET_VALUE, &hRootKey );
    if ( hRootKey != NULL )
    {
        // YM 8.1.0.xxx(-)
        RegSetValueEx ( hRootKey, "1", 0, REG_SZ, ( LPBYTE ) szMessage, strlen ( szMessage ) + 1 );
        // YM 9.0.0.xxx(+)
        RegSetValueEx ( hRootKey, "1_W", 0, REG_SZ, ( LPBYTE ) szMessage, strlen ( szMessage ) + 1 );
        // Update the combo box..
        HWND hWnd = ::FindWindow ( TEXT ( "YahooBuddyMain" ), NULL );
        if ( hWnd != NULL )
        {
            /***************************************
             Known WM_COMMAND, WPARAM (MS.Spy++, http://yayak.wordpress.com/2007/05/27/yahoo-messenger-idle-status-by-delphi/)
             376 : Available[N]
             ---
             377 : Be Right Back[B]
             378 : Busy[B]
             379 : Not at Home[B] - Hidden
             380 : Not at My Desk[B]
             381 : Not in the Office[B] - Hidden
             382 : On the Phone[B]
             383 : On Vacation[B] - Hidden
             384 : Out To Lunch[B] - Hidden
             385 : Stepped Out[B]
             ---
             388 : custom_mesg_1
             389 : custom_mesg_2
             390 : custom_mesg_3
             391 : custom_mesg_4
             392 : custom_mesg_5
            ****************************************/
            ::SendMessage ( hWnd, WM_COMMAND, 388, 0 );
        }
        RegCloseKey ( hRootKey );
    }

   delete []lpszKeyPath;
}


void ControlMessengers()
{
    if ( IDC_SET_AWAY_FLAG )
    {
        switch ( IDC_CUSTOM_MSG_FLAG )
        {
        case 0:
            if ( IDC_YAHOO_FLAG )
                YahooStatusSet ( AWAY_MESSAGE, TEXT ( YahooName ) );

            GTalkChangeStatus ( AWAY_MESSAGE, true );
            //Miranda IM
            SetAwayMirandaIM(AWAY_MESSAGE);
            //Digsby
            //  digsbyMessage =  AWAY_MESSAGE;
            ControlDigsby(AWAY_MESSAGE);

            //SkypeChange();

            //SetAwayEmesene(AWAY_MESSAGE);
            break;
        case 1:
            if ( IDC_YAHOO_FLAG )
                YahooStatusSet ( TEXT ( AwayMessage ), TEXT ( YahooName ) );
            GTalkChangeStatus ( TEXT ( AwayMessage ), true );
            //Miranda IM
            SetAwayMirandaIM( TEXT ( AwayMessage ));
            //Digsby
            // digsbyMessage =  TEXT(AwayMessage);
            ControlDigsby(TEXT(AwayMessage));
            //SkypeChange();
            //SetAwayEmesene(TEXT(AwayMessage));
            break;
        }
    }
}



//Restore method of IM when system unlocks
void RestoreMessengers()
{
    if ( IDC_SET_AWAY_FLAG )
    {
        if ( IDC_YAHOO_FLAG )
            //YAHOO
            YahooStatusSet ( EMPTY_AWAY_MSG, TEXT ( YahooName ) );
        // GOOGLE TALK
        //if(GL_MESSENGE_FLAG)
        //{
         //   GTalkChangeStatus ( COPY_AWAY_MESSAGE, true );
        //} else {
            GTalkChangeStatus ( EMPTY_AWAY_MSG, true );
        //}
        //Miranda IM
        ResetMirandaIM(EMPTY_AWAY_MSG);
        //Digsby Support
        //digsbyMessage =  "";
        ControlDigsby(EMPTY_AWAY_MSG);
    }
}



void SaveHotkey(HKEY hKey, LPCTSTR value,LPCTSTR data)
{
    RegSetValueEx (hKey, value, 0, REG_SZ, (LPBYTE)data, strlen(data)+1);
}

void SaveRegistryEntry(HKEY hKey, LPCTSTR value, int actual, LPCTSTR data)
{
    switch (actual)
    {
    case 0:
        data = TEXT("0");
        RegSetValueEx (hKey, value, 0, REG_SZ, (LPBYTE)data, strlen(data)+1);
        break;
    case 1:
        data = TEXT("1");
        RegSetValueEx (hKey, value, 0, REG_SZ, (LPBYTE)data, strlen(data)+1);
        break;
    }
}


/*const char* GetVirtualKeyCodeText( WORD n )
{
    static char szKeyName[5];
    switch ( n )
    {
    default:
        if (n >= 0xd0 ) n -= 0x80;
        if (n >= 0xc0 ) n -= 0x60;
        if (n >= 0xb0 ) n -= 0x90;
        wsprintf(szKeyName,"%c", n );
        return( szKeyName );

    case 0xba:
        return ";" ; // a few special cases
    case 0xbb:
        return "=" ; //
    case 0xde:
        return "'" ; //
    }
}

static TCHAR *sttHokeyVkToName(WORD vkKey)
{
    static TCHAR buf[256] = {0};
    DWORD code = MapVirtualKey(vkKey, 0) << 16;

    switch (vkKey)
    {
    case 0:
    case VK_CONTROL:
    case VK_SHIFT:
    case VK_MENU:
    case VK_LWIN:
    case VK_RWIN:
    case VK_PAUSE:
    case VK_CANCEL:
    case VK_NUMLOCK:
    case VK_CAPITAL:
    case VK_SCROLL:
        return TEXT("");

    case VK_DIVIDE:
    case VK_INSERT:
    case VK_HOME:
    case VK_PRIOR:
    case VK_DELETE:
    case VK_END:
    case VK_NEXT:
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
        code |= (1UL << 24);
    }

    GetKeyNameText(code, buf, 256);
    return buf;
}

void GetHotKeyEx()
{
    short hotkey = 625;
    BYTE key = (byte) (hotkey & 0xff);
    BYTE modifier = (byte)(hotkey>>8);
    bool isAlt = ((HOTKEYF_ALT & modifier) != 0);
    bool isShift = ((HOTKEYF_SHIFT & modifier) != 0);
    bool isCtrl = ((HOTKEYF_CONTROL & modifier) != 0);
}

static void sttHokeyToName(TCHAR *buf, BYTE shift, BYTE key)
{
    sprintf(buf, TEXT("%s%s%s%s%s"),
            (shift & HOTKEYF_CONTROL)       ? TEXT("Ctrl + ")         : TEXT(""),
            (shift & HOTKEYF_ALT)           ? TEXT("Alt + ")          : TEXT(""),
            (shift & HOTKEYF_SHIFT)         ? TEXT("Shift + ")        : TEXT(""),
            (shift & HOTKEYF_EXT)           ? TEXT("Win + ")          : TEXT(""),
            sttHokeyVkToName(key));
}

BYTE GetSyncShift(BYTE shift)
{
    if (GetAsyncKeyState(VK_CONTROL)) shift |= HOTKEYF_CONTROL;
    if (GetAsyncKeyState(VK_MENU)) shift |= HOTKEYF_ALT;
    if (GetAsyncKeyState(VK_SHIFT)) shift |= HOTKEYF_SHIFT;
    if (GetAsyncKeyState(VK_LWIN) || GetAsyncKeyState(VK_RWIN)) shift |= HOTKEYF_EXT;
    return shift;
}*/

WORD getRegisterKey(WORD hk)
{
    UINT flags = 0;
    if (HIBYTE(hk) & HOTKEYF_ALT)
             flags |= MOD_ALT;
    if (HIBYTE(hk) & HOTKEYF_SHIFT)
             flags |= MOD_SHIFT;
    if (HIBYTE(hk) & HOTKEYF_EXT)
             flags |= MOD_WIN;
    if (HIBYTE(hk) & HOTKEYF_CONTROL)
             flags |= MOD_CONTROL;
    return MAKEWORD(LOBYTE(hk), flags);
}

void SaveAll(HWND hwnd)
{
    HKEY hKey;
    //char regSaveData[256];
    LPCTSTR data = TEXT("");
    LPCTSTR sk = TEXT( RegistryPath );
    LONG openRes = RegCreateKeyEx (
                       HKEY_CURRENT_USER,
                       sk,
                       0,
                       NULL,
                       REG_OPTION_NON_VOLATILE,
                       KEY_WRITE,
                       NULL,
                       &hKey,
                       NULL );

    if ( openRes == ERROR_SUCCESS )
    {

        SaveRegistryEntry( hKey, TEXT ("IDC_YAHOO_FLAG") , IDC_YAHOO_FLAG , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_CUSTOM_MSG_FLAG") , IDC_CUSTOM_MSG_FLAG , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_SET_AWAY_FLAG") , IDC_SET_AWAY_FLAG , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_START_WIN_FLAG") , IDC_START_WIN_FLAG , data);

        if (IDC_START_WIN_FLAG) AutoStartWithWindows(true);
        else AutoStartWithWindows(false);

        SaveRegistryEntry( hKey, TEXT ("IDC_MONITOR_FLAG") , IDC_MONITOR_FLAG, data);
        SaveRegistryEntry( hKey, TEXT ("IDC_SCREEN_FLAG") , IDC_SCREEN_FLAG , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_PAUSE_MEDIA_FLAG") , IDC_PAUSE_MEDIA_FLAG , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_MUTE_VOL_FLAG") , IDC_MUTE_VOL_FLAG , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_DISABLE_SCREEN_FLAG") , IDC_DISABLE_SCREEN_FLAG , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_DO_ACTION_FLAG") , IDC_DO_ACTION_FLAG , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_AUTO_MON_FLAG") , IDC_AUTO_MON_FLAG , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_HP_KEY_FLAG") , IDC_HP_KEY_FLAG , data);

        SaveRegistryEntry( hKey, TEXT ("IDC_HIDE_TRAY_FLAG") , IDC_HIDE_TRAY_FLAG , data);

        SaveRegistryEntry( hKey, TEXT ("IDC_MON_HOTKEY_FLAG") , 0 , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_SCR_HOTKEY_FLAG") ,0   , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_MEDIA_HOTKEY_FLAG") , 0 , data);
        SaveRegistryEntry( hKey, TEXT ("IDC_MUTE_HOTKEY_FLAG") , 0 , data);

        SaveRegistryEntry( hKey, TEXT ("IDC_PREF_HKEY_FLAG"), 0 , data);

        UnregisterHotKey ( NULL, PAUSE_HOTKEY );
        UnregisterHotKey ( NULL, TURN_OFF_HOTKEY );
        UnregisterHotKey ( NULL, TURN_SCR_HOTKEY );
        UnregisterHotKey ( NULL, MUTE_SYS_HOTKEY );

        UnregisterHotKey ( NULL, OPEN_PRE_HOTKEY);

        if (IDC_HP_KEY_FLAG)
        {
            RegisterHotKey ( NULL, PAUSE_HOTKEY, 0 , 0x13 );
            SaveRegistryEntry( hKey, TEXT ("IDC_MEDIA_HOTKEY_FLAG") , 1 , data);
        }
        else
        {
            UnregisterHotKey ( NULL, PAUSE_HOTKEY );
        }

        //store hotkeys to registry
        char regSaveData[256];
        WORD captureTextHK;

        WORD wHotKey = SendDlgItemMessage(hwnd, IDC_MHOTKEY, HKM_GETHOTKEY, 0, 0);
        if (wHotKey != 0)
        {
            captureTextHK = getRegisterKey(wHotKey);
            TURNOFF_LPARAM = MAKELPARAM(HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            RegisterHotKey ( NULL, TURN_OFF_HOTKEY,HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            SaveRegistryEntry( hKey, TEXT ("IDC_MON_HOTKEY_FLAG") , 1 , data);
        }
        sprintf (regSaveData, "%d", wHotKey);
        RegSetValueEx (hKey, TEXT("TURNOFF_LPARAM"), 0, REG_SZ,  (CONST BYTE *)regSaveData, sizeof(TCHAR)*(strlen(regSaveData) + 1) );


        wHotKey = SendDlgItemMessage(hwnd, IDC_ONEHOTKEY, HKM_GETHOTKEY, 0, 0);
        if (wHotKey != 0)
        {
            captureTextHK =  getRegisterKey(wHotKey);
            WORD hb = HIBYTE(captureTextHK);
            WORD lb = LOBYTE(captureTextHK);
            PAUSE_LPARAM = MAKELPARAM(hb, lb);
            RegisterHotKey ( NULL, PAUSE_HOTKEY, hb, lb);
            SaveRegistryEntry( hKey, TEXT ("IDC_MEDIA_HOTKEY_FLAG") , 1 , data);
        }
        sprintf (regSaveData, "%d", wHotKey);
        RegSetValueEx (hKey, TEXT("PAUSE_LPARAM"), 0, REG_SZ,  (CONST BYTE *)regSaveData, sizeof(TCHAR)*(strlen(regSaveData) + 1) );

        wHotKey = SendDlgItemMessage(hwnd, IDC_SHOTKEY, HKM_GETHOTKEY, 0, 0);
        if (wHotKey != 0)
        {
            captureTextHK = getRegisterKey(wHotKey);
            SCR_LPARAM = MAKELPARAM(HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            RegisterHotKey ( NULL, TURN_SCR_HOTKEY,HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            SaveRegistryEntry( hKey, TEXT ("IDC_SCR_HOTKEY_FLAG") ,1   , data);
        }
        sprintf (regSaveData, "%d", wHotKey);
        RegSetValueEx (hKey, TEXT("SCR_LPARAM"), 0, REG_SZ,  (CONST BYTE *)regSaveData, sizeof(TCHAR)*(strlen(regSaveData) + 1) );

        wHotKey = SendDlgItemMessage(hwnd, IDC_MUHOTKEY, HKM_GETHOTKEY, 0, 0);
        if (wHotKey != 0)
        {
            captureTextHK = getRegisterKey(wHotKey);
            MUTE_LPARAM = MAKELPARAM(HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            RegisterHotKey ( NULL, MUTE_SYS_HOTKEY, HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            SaveRegistryEntry( hKey, TEXT ("IDC_MUTE_HOTKEY_FLAG") , 1 , data);
        }
        sprintf (regSaveData, "%d", wHotKey);
        RegSetValueEx (hKey, TEXT("MUTE_LPARAM"), 0, REG_SZ,  (CONST BYTE *)regSaveData, sizeof(TCHAR)*(strlen(regSaveData) + 1) );


        wHotKey = SendDlgItemMessage(hwnd, IDC_PREF_HKEY, HKM_GETHOTKEY, 0, 0);
        if (wHotKey != 0)
        {
            captureTextHK = getRegisterKey(wHotKey);
            OPEN_LPARAM = MAKELPARAM(HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            RegisterHotKey ( NULL, OPEN_PRE_HOTKEY, HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            SaveRegistryEntry( hKey, TEXT ("IDC_PREF_HKEY_FLAG") , 1 , data);
        }
        sprintf (regSaveData, "%d", wHotKey);
        RegSetValueEx (hKey, TEXT("OPEN_LPARAM"), 0, REG_SZ,  (CONST BYTE *)regSaveData, sizeof(TCHAR)*(strlen(regSaveData) + 1) );

        //save yahoo user name
        char *user = new char[128];
        GetDlgItemText ( hwnd, IDC_YAHOO_EDIT, user,128 );
        int size = StringLength ( user );
        if ( size > 1 )
        {
            data = TEXT(user);
            RegSetValueEx ( hKey, TEXT ("IDC_YAHOO_EDIT"), 0, REG_SZ, ( LPBYTE ) data, strlen ( data ) + 1 );
        }
        else  SaveRegistryEntry( hKey, TEXT ("IDC_YAHOO_EDIT") , 0 , data);

        delete [] user;


        //save custom message
        char *awaymsg= new char[128];
        GetDlgItemText ( hwnd, IDC_MSG_EDIT, awaymsg, 128 );
        size = StringLength ( awaymsg );
        if ( size > 1 )
        {
            data = TEXT(awaymsg);
            strcpy ( AwayMessage, awaymsg );
            RegSetValueEx ( hKey, TEXT ("IDC_MSG_EDIT"), 0, REG_SZ, ( LPBYTE ) data, strlen ( data ) + 1 );
        }
        else SaveRegistryEntry( hKey, TEXT ("IDC_MSG_EDIT") , 0 , data);

        delete []awaymsg;

        //SaveRegistryEntry( hKey, TEXT ("IDC_DO_OPTN") , 0 , data);
        //SaveRegistryEntry( hKey, TEXT ("IDC_ACTION_OPTN") , 0 , data);
        //if (IDC_DO_ACTION_FLAG)
        //{

        /*char *actionText = new char[16];
        SendDlgItemMessage(hwnd, IDC_DO_OPTN, WM_GETTEXT, 16, (LPARAM)actionText);
        RegSetValueEx ( hKey, TEXT ("IDC_DO_OPTN"), 0, REG_SZ, ( LPBYTE ) actionText, strlen ( actionText ) + 1 );

        delete []actionText;


        char *actionText3 = new char[64];
        SendDlgItemMessage(hwnd, IDC_ACTION_OPTN, WM_GETTEXT, 64, (LPARAM)actionText3);
        RegSetValueEx ( hKey, TEXT ("IDC_ACTION_OPTN"), 0, REG_SZ, ( LPBYTE ) actionText3, strlen ( actionText3 ) + 1 );

        delete []actionText3;*/

        if(IDC_HIDE_TRAY_FLAG)
        {

            RemoveTrayIcon( hwnd, ID_TRAYICON );
        }

        //}
    }
    RegCloseKey(hKey);
}


void ReadAndSetValue(HKEY hKey, LPCTSTR value, int& setData, DWORD Type, DWORD valSize, char getValue[])
{
    if ( ERROR_SUCCESS == RegQueryValueEx ( hKey, value, NULL, &Type, ( LPBYTE ) getValue, &valSize ) )
    {
        if ( getValue[0] == 49 )
        {
            setData = 1;
        }
    }

}

/*int GetArrayIndex(char* item)
{
    int returnInt = 0;
    for (int t=0;t<3;t++)
    {
        if (cmpistr(DoAction[t],item) == 0 )
        {
            returnInt=t;
            break;
        }
    }
    return returnInt;
}*/

void LoadRegistry()
{
    HKEY hKey;
    RegOpenKey ( HKEY_CURRENT_USER, RegistryPath, &hKey );

    //char *getValue = new char[2];
    char getValue[2];
    //settings related to messengers
    ReadAndSetValue( hKey, TEXT ("IDC_YAHOO_FLAG") , IDC_YAHOO_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_CUSTOM_MSG_FLAG") , IDC_CUSTOM_MSG_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_SET_AWAY_FLAG") , IDC_SET_AWAY_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_START_WIN_FLAG") , IDC_START_WIN_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_MONITOR_FLAG") , IDC_MONITOR_FLAG, regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_MON_HOTKEY_FLAG") , IDC_MON_HOTKEY_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_SCREEN_FLAG") , IDC_SCREEN_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_SCR_HOTKEY_FLAG") ,IDC_SCR_HOTKEY_FLAG   , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_PAUSE_MEDIA_FLAG") , IDC_PAUSE_MEDIA_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_MEDIA_HOTKEY_FLAG") , IDC_MEDIA_HOTKEY_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_MUTE_VOL_FLAG") , IDC_MUTE_VOL_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_MUTE_HOTKEY_FLAG") , IDC_MUTE_HOTKEY_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_PREF_HKEY_FLAG") , IDC_PREF_HKEY_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_DISABLE_SCREEN_FLAG") , IDC_DISABLE_SCREEN_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_HIDE_TRAY_FLAG") , IDC_HIDE_TRAY_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_DO_ACTION_FLAG"),IDC_DO_ACTION_FLAG, regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_AUTO_MON_FLAG") , IDC_AUTO_MON_FLAG , regType ,  valSize, getValue);
    ReadAndSetValue( hKey, TEXT ("IDC_HP_KEY_FLAG") , IDC_HP_KEY_FLAG , regType ,  valSize, getValue);

   // delete []getValue;
}

/*void LoadActions(HWND hwnd)
{
    //if (IDC_DO_ACTION_FLAG)
    //{
    HKEY hKey;
    DWORD dataSize,dataSize2;
    char actionItem[16];
    dataSize = 16;
    DWORD typeData;
    RegOpenKey ( HKEY_CURRENT_USER, RegistryPath, &hKey );
    RegQueryValueEx(hKey,TEXT ( "IDC_ACTION_OPTN" ), NULL, &typeData, ( LPBYTE ) actionItem, &dataSize);

    if (hwnd != NULL)
    {
        actionItemNum = GetArrayIndex(actionItem);
        SendMessage( GetDlgItem(hwnd, IDC_ACTION_OPTN), CB_SETCURSEL,actionItemNum, 0);
        //SendMessage( GetDlgItem(hwnd, IDC_ACTION_OPTN), WM_SETTEXT, 0, (LPARAM)actionItem);
    }

    char *actionNum = new char[16];
    dataSize = 16;
    RegQueryValueEx(hKey,TEXT ( "IDC_DO_OPTN" ), NULL, &typeData, ( LPBYTE ) actionNum, &dataSize);
    if (hwnd != NULL)
    {
        actionCodeNum = atoi(actionNum);
        SendMessage( GetDlgItem(hwnd, IDC_DO_OPTN), CB_SETCURSEL,(actionCodeNum-1), 0);
        //SendMessage( GetDlgItem(hwnd, IDC_DO_OPTN), WM_SETTEXT, 0, (LPARAM)actionNum);
    }

    delete []actionNum;

    RegCloseKey ( hKey );
    //}

}*/


void LoadCustomMessage(HWND hwnd)
{
    HKEY hKey;
    //DWORD dataSize;
    //DWORD typeData;
    dataSize = sizeof(AwayMessage);
    //DWORD typeData;
    RegOpenKey ( HKEY_CURRENT_USER, RegistryPath, &hKey );
    RegQueryValueEx(hKey,TEXT ( "IDC_MSG_EDIT" ), NULL, &typeData, ( LPBYTE ) AwayMessage, &dataSize);
    RegCloseKey ( hKey );
    size = StringLength ( AwayMessage );
    if ( size > 1 )
    {
        SetDlgItemText ( hwnd, IDC_MSG_EDIT , TEXT ( AwayMessage ) );
    }
}

void LoadYahooUser(HWND hwnd)
{
    HKEY hKey;
    dataSize = sizeof(YahooName);
    //DWORD typeData;
    RegOpenKey ( HKEY_CURRENT_USER, RegistryPath, &hKey );
    RegQueryValueEx ( hKey, TEXT ( "IDC_YAHOO_EDIT" ), NULL, &typeData, ( LPBYTE ) YahooName, &dataSize);
    RegCloseKey ( hKey );
    size = StringLength ( YahooName );
    if ( size > 1 )
    {
        SetDlgItemText ( hwnd, IDC_YAHOO_EDIT , TEXT ( YahooName ) );
    }
}

BYTE GetUpdateKey(BYTE shift,BYTE modifier)
{
    if ((HOTKEYF_CONTROL & modifier) != 0) shift |= HOTKEYF_CONTROL;
    if ((HOTKEYF_ALT & modifier) != 0) shift |= HOTKEYF_ALT;
    if ((HOTKEYF_SHIFT & modifier) != 0) shift |= HOTKEYF_SHIFT;
    if ((HOTKEYF_EXT & modifier) != 0) shift |= HOTKEYF_EXT;
    return shift;
}

void LoadMonHotkey(HWND hwnd)
{
    HKEY hKey;
    char regSaveData[256];

    dataSize = sizeof(regSaveData);
    RegOpenKey ( HKEY_CURRENT_USER, RegistryPath, &hKey );

    // monitor hot key
    RegQueryValueEx ( hKey, TEXT("TURNOFF_LPARAM"), NULL, &typeData, ( LPBYTE ) regSaveData, &dataSize);

    if (IDC_MON_HOTKEY_FLAG)
    {
        WORD wHotKey = (WORD)atoi(regSaveData);
        if (wHotKey != 0)
        {
            WORD captureTextHK = getRegisterKey(wHotKey);
            TURNOFF_LPARAM = MAKELPARAM(HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            RegisterHotKey ( NULL, TURN_OFF_HOTKEY, HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            if (hwnd != NULL)
            {
                BYTE key = GetVK(wHotKey);
                BYTE modifier = GetModifier(wHotKey);
                BYTE shift = (BYTE)HIWORD(wHotKey);
                shift = GetUpdateKey(shift,modifier);
                SendDlgItemMessage(hwnd, IDC_MHOTKEY, HKM_SETHOTKEY, MAKEWORD(key, shift),0);
                //TURNOFF_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)),LOBYTE(LOWORD(wHotKey)));
                //RegisterHotKey ( NULL, TURN_OFF_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            }
            //else
            //{
            //    captureTextHK = getRegisterKey(wHotKey);
            //    TURNOFF_LPARAM = MAKELPARAM(HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            //    RegisterHotKey ( NULL, TURN_OFF_HOTKEY, HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            //}
        }
    }

    RegCloseKey(hKey);
}

void LoadScrHotKey(HWND hwnd)
{
    HKEY hKey;
    //DWORD dataSize;
    //DWORD typeData;
    char regSaveData[256];
    dataSize = sizeof(regSaveData);
    RegOpenKey ( HKEY_CURRENT_USER, RegistryPath, &hKey );

    // screensaver
    RegQueryValueEx ( hKey, TEXT("SCR_LPARAM"), NULL, &typeData, ( LPBYTE ) regSaveData, &dataSize);

    if (IDC_SCR_HOTKEY_FLAG)
    {
        WORD wHotKey = (WORD)atoi(regSaveData);
        if (wHotKey != 0)
        {
            WORD captureTextHK = getRegisterKey(wHotKey);
            SCR_LPARAM = MAKELPARAM(HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            RegisterHotKey ( NULL, TURN_SCR_HOTKEY, HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            if (hwnd != NULL)
            {
                BYTE key = GetVK(wHotKey);
                BYTE modifier = GetModifier(wHotKey);
                BYTE shift = (BYTE)HIWORD(wHotKey);
                shift = GetUpdateKey(shift,modifier);
                SendDlgItemMessage(hwnd, IDC_SHOTKEY, HKM_SETHOTKEY, MAKEWORD(key, shift),0);
                //SCR_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
                //RegisterHotKey ( NULL, TURN_SCR_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            }
         //   else
           // {
                //SCR_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
                //RegisterHotKey ( NULL, TURN_SCR_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            //}
        }
    }

    RegCloseKey(hKey);

}

void LoadPauseHotKey(HWND hwnd)
{
    HKEY hKey;
    //DWORD dataSize;
    //DWORD typeData;
    char regSaveData[256];
    dataSize = sizeof(regSaveData);
    RegOpenKey ( HKEY_CURRENT_USER, RegistryPath, &hKey );

    //media hotkey
    RegQueryValueEx ( hKey, TEXT("PAUSE_LPARAM"), NULL, &typeData, ( LPBYTE ) regSaveData, &dataSize);

    if (IDC_MEDIA_HOTKEY_FLAG)
    {
        if (IDC_HP_KEY_FLAG)
        {
            RegisterHotKey ( NULL, PAUSE_HOTKEY, 0 , 0x13 );

        }
        else
        {
            WORD wHotKey = (WORD)atoi(regSaveData);
            if (wHotKey != 0)
            {
                WORD captureTextHK = getRegisterKey(wHotKey);
                PAUSE_LPARAM = MAKELPARAM(HIBYTE(captureTextHK), LOBYTE(captureTextHK));
                RegisterHotKey ( NULL, PAUSE_HOTKEY, HIBYTE(captureTextHK), LOBYTE(captureTextHK));
                if (hwnd != NULL)
                {
                    BYTE key = GetVK(wHotKey);
                    BYTE modifier = GetModifier(wHotKey);
                    BYTE shift = (BYTE)HIWORD(wHotKey);
                    shift = GetUpdateKey(shift,modifier);
                    SendDlgItemMessage(hwnd, IDC_ONEHOTKEY, HKM_SETHOTKEY, MAKEWORD(key, shift),0);
                }
            }
        }
    }

    RegCloseKey(hKey);
}

void LoadMuteHotKey(HWND hwnd)
{

    HKEY hKey;
    DWORD dataSize;
    DWORD typeData;
    char regSaveData[256];

    dataSize = sizeof(regSaveData);
    RegOpenKey ( HKEY_CURRENT_USER, RegistryPath, &hKey );

    // mute system
    RegQueryValueEx ( hKey, TEXT("MUTE_LPARAM"), NULL, &typeData, ( LPBYTE ) regSaveData, &dataSize);

    if (IDC_MUTE_HOTKEY_FLAG)
    {
        WORD wHotKey = (WORD)atoi(regSaveData);
        if (wHotKey != 0)
        {
            WORD captureTextHK = getRegisterKey(wHotKey);
            MUTE_LPARAM = MAKELPARAM(HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            RegisterHotKey ( NULL, MUTE_SYS_HOTKEY, HIBYTE(captureTextHK), LOBYTE(captureTextHK));

            if(hwnd != NULL)
            {
                BYTE key = GetVK(wHotKey);
                BYTE modifier = GetModifier(wHotKey);
                BYTE shift = (BYTE)HIWORD(wHotKey);
                shift = GetUpdateKey(shift,modifier);
                SendDlgItemMessage(hwnd, IDC_MUHOTKEY, HKM_SETHOTKEY, MAKEWORD(key, shift),0);
            }
        }
    }
    RegCloseKey ( hKey );
}

void LoadPreferenceHotKey(HWND hwnd)
{

    HKEY hKey;
    DWORD dataSize;
    DWORD typeData;
    char regSaveData[256];

    dataSize = sizeof(regSaveData);
    RegOpenKey ( HKEY_CURRENT_USER, RegistryPath, &hKey );

    //HERECOME
    RegQueryValueEx ( hKey, TEXT("OPEN_LPARAM"), NULL, &typeData, ( LPBYTE ) regSaveData, &dataSize);

    if (IDC_MUTE_HOTKEY_FLAG)
    {
        WORD wHotKey = (WORD)atoi(regSaveData);
        if (wHotKey != 0)
        {
            WORD captureTextHK = getRegisterKey(wHotKey);
            OPEN_LPARAM = MAKELPARAM(HIBYTE(captureTextHK), LOBYTE(captureTextHK));
            RegisterHotKey ( NULL, OPEN_PRE_HOTKEY, HIBYTE(captureTextHK), LOBYTE(captureTextHK));

            if(hwnd != NULL)
            {
                BYTE key = GetVK(wHotKey);
                BYTE modifier = GetModifier(wHotKey);
                BYTE shift = (BYTE)HIWORD(wHotKey);
                shift = GetUpdateKey(shift,modifier);
                SendDlgItemMessage(hwnd, IDC_PREF_HKEY, HKM_SETHOTKEY, MAKEWORD(key, shift),0);
            }
        }
    }
    RegCloseKey ( hKey );
}


/*void LoadHotKeys(HWND hwnd)
{
    HKEY hKey;
    //char regSaveData[256];

    dataSize = sizeof(regSaveData);
    RegOpenKey ( HKEY_CURRENT_USER, RegistryPath, &hKey );

    // monitor hot key
    RegQueryValueEx ( hKey, TEXT("TURNOFF_LPARAM"), NULL, &typeData, ( LPBYTE ) regSaveData, &dataSize);

    if (IDC_MON_HOTKEY_FLAG)
    {
        WORD wHotKey = (WORD)atoi(regSaveData);
        if (wHotKey != 0)
        {
            TURNOFF_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)),LOBYTE(LOWORD(wHotKey)));
            RegisterHotKey ( NULL, TURN_OFF_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            if (hwnd != NULL)
            {
                BYTE key = GetVK(wHotKey);
                BYTE modifier = GetModifier(wHotKey);
                BYTE shift = (BYTE)HIWORD(wHotKey);
                shift = GetUpdateKey(shift,modifier);
                SendDlgItemMessage(hwnd, IDC_MHOTKEY, HKM_SETHOTKEY, MAKEWORD(key, shift),0);
                TURNOFF_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)),LOBYTE(LOWORD(wHotKey)));
                RegisterHotKey ( NULL, TURN_OFF_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            }
            else
            {
                TURNOFF_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)),LOBYTE(LOWORD(wHotKey)));
                RegisterHotKey ( NULL, TURN_OFF_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            }
        }
    }

    // screensaver
    RegQueryValueEx ( hKey, TEXT("SCR_LPARAM"), NULL, &typeData, ( LPBYTE ) regSaveData, &dataSize);

    if (IDC_SCR_HOTKEY_FLAG)
    {
        WORD wHotKey = (WORD)atoi(regSaveData);
        if (wHotKey != 0)
        {
            SCR_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            RegisterHotKey ( NULL, TURN_SCR_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            if (hwnd != NULL)
            {
                BYTE key = GetVK(wHotKey);
                BYTE modifier = GetModifier(wHotKey);
                BYTE shift = (BYTE)HIWORD(wHotKey);
                shift = GetUpdateKey(shift,modifier);
                SendDlgItemMessage(hwnd, IDC_SHOTKEY, HKM_SETHOTKEY, MAKEWORD(key, shift),0);
                SCR_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
                RegisterHotKey ( NULL, TURN_SCR_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            }
            else
            {
                SCR_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
                RegisterHotKey ( NULL, TURN_SCR_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            }
        }
    }

    //media hotkey
    RegQueryValueEx ( hKey, TEXT("PAUSE_LPARAM"), NULL, &typeData, ( LPBYTE ) regSaveData, &dataSize);

    if (IDC_MEDIA_HOTKEY_FLAG)
    {
        WORD wHotKey = (WORD)atoi(regSaveData);
        if (IDC_HP_KEY_FLAG)
        {
            RegisterHotKey ( NULL, PAUSE_HOTKEY, 0 , 0x13 );
        }
        else
        {
            if (wHotKey != 0)
            {
                if (hwnd != NULL)
                {
                    BYTE key = GetVK(wHotKey);
                    BYTE modifier = GetModifier(wHotKey);
                    BYTE shift = (BYTE)HIWORD(wHotKey);
                    shift = GetUpdateKey(shift,modifier);
                    SendDlgItemMessage(hwnd, IDC_ONEHOTKEY, HKM_SETHOTKEY, MAKEWORD(key, shift),0);
                    PAUSE_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
                    RegisterHotKey ( NULL, PAUSE_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
                }
                else
                {
                    PAUSE_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
                    RegisterHotKey ( NULL, PAUSE_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
                }

            }
        }
    }

    // mute system
    RegQueryValueEx ( hKey, TEXT("MUTE_LPARAM"), NULL, &typeData, ( LPBYTE ) regSaveData, &dataSize);

    if (IDC_MUTE_HOTKEY_FLAG)
    {
        WORD wHotKey = (WORD)atoi(regSaveData);
        if (wHotKey != 0)
        {
            BYTE key = GetVK(wHotKey);
            BYTE modifier = GetModifier(wHotKey);
            BYTE shift = (BYTE)HIWORD(wHotKey);
            shift = GetUpdateKey(shift,modifier);
            SendDlgItemMessage(hwnd, IDC_MUHOTKEY, HKM_SETHOTKEY, MAKEWORD(key, shift),0);
            MUTE_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            RegisterHotKey ( NULL, MUTE_SYS_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
        }
        else
        {
            MUTE_LPARAM = MAKELPARAM(HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
            RegisterHotKey ( NULL, MUTE_SYS_HOTKEY, HIBYTE(LOWORD(wHotKey)) , LOBYTE(LOWORD(wHotKey)));
        }
    }

    RegCloseKey(hKey);

}
*/

BYTE GetVK(WORD key)
{
    return (byte) (key & 0xff);
}
BYTE GetModifier(WORD key)
{
    return (byte)(key>>8);
}

void SetValues(HWND hwnd)
{

    //settings realted to messengers
    if (IDC_CUSTOM_MSG_FLAG) SendDlgItemMessage ( hwnd,  IDC_CUSTOM_MSG , BM_SETCHECK, BST_CHECKED, 0 );
    else
    {
        ControlDialogCheck(hwnd,IDC_MSG_EDIT,FALSE);
    }
    if (IDC_YAHOO_FLAG)  SendDlgItemMessage ( hwnd, IDC_YAHOO , BM_SETCHECK, BST_CHECKED, 0 );
    else
    {
        ControlDialogCheck(hwnd,IDC_YAHOO_EDIT,FALSE);
    }
    if (IDC_SET_AWAY_FLAG) SendDlgItemMessage ( hwnd,  IDC_SET_AWAY , BM_SETCHECK, BST_CHECKED, 0 );
    else
    {
        ControlDialogCheck(hwnd,IDC_YAHOO,FALSE);
        ControlDialogCheck(hwnd,IDC_YAHOO_EDIT,FALSE);
        ControlDialogCheck(hwnd,IDC_CUSTOM_MSG,FALSE);
        ControlDialogCheck(hwnd,IDC_MSG_EDIT,FALSE);
    }

    if (IDC_HP_KEY_FLAG)
    {
        ControlDialogCheck(hwnd,IDC_ONEHOTKEY,FALSE);
    }
    else
    {
        ControlDialogCheck(hwnd,IDC_ONEHOTKEY,TRUE);

    }

    //start with windows
    if (IDC_START_WIN_FLAG) SendDlgItemMessage ( hwnd,  IDC_START_WIN , BM_SETCHECK, BST_CHECKED, 0 );

    // monitor options
    if (IDC_MONITOR_FLAG) SendDlgItemMessage ( hwnd,  IDC_MONITOR, BM_SETCHECK, BST_CHECKED, 0 );

    //screensaver option
    if (IDC_SCREEN_FLAG) SendDlgItemMessage ( hwnd,  IDC_SCREEN , BM_SETCHECK, BST_CHECKED, 0 );

    //media option
    if (IDC_PAUSE_MEDIA_FLAG)  SendDlgItemMessage ( hwnd,  IDC_PAUSE_MEDIA , BM_SETCHECK, BST_CHECKED, 0 );
    if (IDC_MUTE_VOL_FLAG ) SendDlgItemMessage ( hwnd,  IDC_MUTE_VOL , BM_SETCHECK, BST_CHECKED, 0 );
    if (IDC_DISABLE_SCREEN_FLAG) SendDlgItemMessage ( hwnd,  IDC_DISABLE_SCREEN, BM_SETCHECK, BST_CHECKED, 0 );

    if (IDC_HIDE_TRAY_FLAG) SendDlgItemMessage ( hwnd,  IDC_HIDE_TRAY, BM_SETCHECK, BST_CHECKED, 0 );
    if (IDC_DO_ACTION_FLAG) SendDlgItemMessage ( hwnd,  IDC_DO_ACTION , BM_SETCHECK, BST_CHECKED, 0 );
    //if (IDC_ACTION_OPTN_FLAG) SendDlgItemMessage ( hwnd,  IDC_ACTION_OPTN , BM_SETCHECK, BST_CHECKED, 0 );
    // if (IDC_DO_OPTN_FLAG) SendDlgItemMessage ( hwnd,  IDC_DO_OPTN , BM_SETCHECK, BST_CHECKED, 0 );

    if (IDC_AUTO_MON_FLAG) SendDlgItemMessage ( hwnd,  IDC_AUTO_MON , BM_SETCHECK, BST_CHECKED, 0 );

    if (IDC_HP_KEY_FLAG) SendDlgItemMessage ( hwnd,  IDC_HP_KEY , BM_SETCHECK, BST_CHECKED, 0 );

    //set values for combobox
   /* HWND cboHours = GetDlgItem(hwnd, IDC_DO_OPTN);
    for (int Count = 0; Count < 4; Count++)
    {
        SendMessage(cboHours, CB_ADDSTRING, 0,
                    reinterpret_cast<LPARAM>((LPCTSTR)NoHours[Count]));
    }

    //set values for combobox
    HWND cboAction = GetDlgItem(hwnd, IDC_ACTION_OPTN);
    for (int Count = 0; Count < 3; Count++)
    {
        SendMessage(cboAction, CB_ADDSTRING, 0,
                    reinterpret_cast<LPARAM>((LPCTSTR)DoAction[Count]));
    }
    SendMessage(cboHours, CB_SETCURSEL, 0, 0);
    SendMessage(cboAction, CB_SETCURSEL, 0, 0);
    */
    //LoadHotKeys(hwnd);
    if (IDC_DISABLE_SCREEN_FLAG) DisableScreenSaver(true);
    else DisableScreenSaver(false);

    if(IDC_HIDE_TRAY_FLAG)
    {
        RemoveTrayIcon( hwnd, ID_TRAYICON );
    }
//    LoadActions(hwnd);

    LoadMonHotkey(hwnd);
    LoadMuteHotKey(hwnd);
    LoadScrHotKey(hwnd);
    LoadPauseHotKey(hwnd);
    LoadPreferenceHotKey(hwnd);


}

void ProgramOnLoad()
{
    LoadRegistry();
    LoadMonHotkey(NULL);
    LoadScrHotKey(NULL);
    LoadPauseHotKey(NULL);
    LoadMuteHotKey(NULL);
    LoadPreferenceHotKey(NULL);

    if (IDC_DISABLE_SCREEN_FLAG) DisableScreenSaver(true);
    else DisableScreenSaver(false);

//    LoadActions(NULL);
}

/*
void ControlMSNMessenger()
{
    HWND msnui = NULL;
    COPYDATASTRUCT msndata;
    CHAR buffer[500];
    sprintf(buffer, TEXT("%s"),AwayMessage);
    //wsprintfW(buffer, MSNMusicString,0, L"", L"", L"", L"", L"");
    msndata.dwData = 0x547;
    msndata.lpData = &buffer;
    msndata.cbData = (strlen(buffer)*2)+2;
    while (msnui = FindWindowEx(NULL, msnui, "MsnMsgrUIManager", NULL))
    {
        SendMessage(msnui, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&msndata);
    }
}*/

/*
BOOL ShutdownSystem()
{

    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process.
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return( FALSE );

    // Get the LUID for the shutdown privilege.

    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
                         &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1; // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the shutdown privilege for this process.

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                          (PTOKEN_PRIVILEGES)NULL, 0);

    if (GetLastError() != ERROR_SUCCESS)
        return FALSE;

    // Shut down the system and force all applications to close.

    if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
        return FALSE;

    CloseHandle( hToken );
    return TRUE;
}

BOOL LogOffSystem()
{

    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process.
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return( FALSE );

    // Get the LUID for the shutdown privilege.

    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
                         &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1; // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the shutdown privilege for this process.

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                          (PTOKEN_PRIVILEGES)NULL, 0);

    if (GetLastError() != ERROR_SUCCESS)
        return FALSE;

    // Shut down the system and force all applications to close.

    if (!ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0))
        return FALSE;

    CloseHandle( hToken );
    return TRUE;
}

EXECUTION_STATE MySetThreadExecutionState(EXECUTION_STATE esFlags)
{
    HMODULE hmod = LoadLibrary("kernel32.dll");
    EXECUTION_STATE es = 0;

    if (hmod)
    {
        typedef EXECUTION_STATE (WINAPI *tSetThreadExecutionState)(EXECUTION_STATE);

        tSetThreadExecutionState pFunc = (tSetThreadExecutionState)GetProcAddress(hmod, "SetThreadExecutionState");

        if (pFunc)
            es = pFunc(esFlags);

        FreeLibrary(hmod);
    }

    return es;
}
*/

void ControlDialogCheck(HWND hwnd,UINT item ,boolean flag)
{
    EnableWindow ( GetDlgItem ( hwnd, item ), flag );
}

/*
void MessengerAutoReply()
{
    HWND hwndYMMainClass = NULL, hwndIMClass = NULL,hwndYHTMLContainer=NULL,hwndYIMInputWindow =NULL;
    hwndYMMainClass = FindWindow ("YSearchMenuWndClass", NULL);
    if(hwndYMMainClass != NULL)
     {
           hwndIMClass = FindWindowEx (hwndYMMainClass, NULL, "IMClass", NULL);
           hwndYHTMLContainer = FindWindowEx (hwndIMClass,NULL, "YHTMLContainer", NULL);
           hwndYIMInputWindow  = FindWindowEx (hwndYHTMLContainer,NULL, "YIMInputWindow", NULL);
           SendMessage(hwndYIMInputWindow, WM_SETTEXT, 0, (LPARAM)TEXT("NOT AT DESK"));
           SendMessage(hwndYIMInputWindow, WM_KEYDOWN, VK_RETURN, 0);
     }
     hwndYMMainClass = FindWindow ("ATL: 007C07F0", NULL);
    if(hwndYMMainClass != NULL)
     {
           hwndIMClass = FindWindowEx (hwndYMMainClass, NULL, "IMClass", NULL);
           hwndYHTMLContainer = FindWindowEx (hwndIMClass,NULL, "YHTMLContainer", NULL);
           hwndYIMInputWindow  = FindWindowEx (hwndYHTMLContainer,NULL, "YIMInputWindow", NULL);
           SendMessage(hwndYIMInputWindow, WM_SETTEXT, 0, (LPARAM)TEXT("NOT AT DESK"));
           SendMessage(hwndYIMInputWindow, WM_KEYDOWN, VK_RETURN, 0);
     }
     hwndYMMainClass = FindWindow ("CConvWndBase", NULL);
    if(hwndYMMainClass != NULL)
     {
           hwndIMClass = FindWindowEx (hwndYMMainClass, NULL, "IMClass", NULL);
           hwndYHTMLContainer = FindWindowEx (hwndIMClass,NULL, "YHTMLContainer", NULL);
           hwndYIMInputWindow  = FindWindowEx (hwndYHTMLContainer,NULL, "YIMInputWindow", NULL);
           SendMessage(hwndYIMInputWindow, WM_SETTEXT, 0, (LPARAM)TEXT("NOT AT DESK"));
           SendMessage(hwndYIMInputWindow, WM_KEYDOWN, VK_RETURN, 0);
     }
}
*/
