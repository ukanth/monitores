#ifndef TRAY_H_INCLUDED
#define TRAY_H_INCLUDED

static bool b_DialogShowing        = FALSE;

#define HELP_ABOUT "MonitorES 1.1 final\n\nMonitorES helps system to perform set of actions automatically when user locks the workstations.\n * Auto turnoff monitor ( Energy Save ) \n * Auto pause running media players\n * Auto away status of instant messengers etc., \n\n\nDeveloped by Umakanthan Chandran\ncumakt@gmail.com - http://ukanth.in"

char TOOL_TIP[] = "MonitorES 1.1 final is Running";
//-----------------------------------------------------------------------------
enum
{
    //  Tray icon crap
    ID_TRAYICON         = 1,

    APPWM_TRAYICON      = WM_APP,
    APPWM_NOP           = WM_APP + 1,

    //  Our commands
    ID_OPTIONS          = 2000,
    ID_ABOUT,
    ID_EXIT,
};

#define TRAY_ICON 91
#define WM_NOTIFYICONTRAY (WM_APP + 1)//tray message

#ifndef IDC_STATIC
#define IDC_STATIC (-1)
#endif

#define IDD_SETUP_DIALOG                        101
#define IDC_YAHOO                               1028
#define IDC_CUSTOM_MSG                          1029
#define IDC_SET_AWAY                            1032
#define IDC_MONITOR                             1033
#define IDC_MON_HOTKEY                          1034
#define IDC_SCREEN                              1037
#define IDC_SCR_HOTKEY                          1038
#define IDC_DISABLE_SCREEN                      1039
#define IDC_PAUSE_MEDIA                         1040
#define IDC_MEDIA_HOTKEY                        1041
#define IDC_MUTE_VOL                            1042
#define IDC_YAHOO_EDIT                          1043
#define IDC_MSG_EDIT                            1044
#define IDC_MHOTKEY                             1045
#define IDC_SHOTKEY                             1046
#define IDC_ONEHOTKEY                           1047
#define IDC_DO_ACTION                           1048
#define IDC_ACTION_OPTN                         1050
#define IDC_DO_OPTN                             1051
#define IDC_START_WIN                           1052
#define IDC_START_MIN                           1053
#define IDC_SAVE                                1054
#define IDC_CANCEL                              1055
#define IDC_MUTE_HOTKEY                         1056
#define IDC_MUHOTKEY                            1057
#define IDC_M_TEXT                              1058
#define IDC_S_TEXT                              1059
#define IDC_AUTO_MON                            1060
#define IDC_HP_KEY                              1061
#define IDC_SUSPEND_OPERATION                   1062
#define IDC_PREF_HKEY                           1063
#define IDC_PREFLABEL                           1064
#define IDC_HIDE_TRAY                           1065


//int IDD_SETUP_DIALOG_FLAG = 0;
int IDC_YAHOO_FLAG = 0;
int IDC_CUSTOM_MSG_FLAG  = 0;
int IDC_SET_AWAY_FLAG    = 0;
int IDC_MONITOR_FLAG     = 0;
int IDC_MON_HOTKEY_FLAG  = 0;
int IDC_SCREEN_FLAG      = 0;
int IDC_SCR_HOTKEY_FLAG  = 0;
int IDC_DISABLE_SCREEN_FLAG   = 0;
int IDC_PAUSE_MEDIA_FLAG = 0;
int IDC_MEDIA_HOTKEY_FLAG = 0;
int IDC_MUTE_VOL_FLAG    = 0;
int IDC_YAHOO_EDIT_FLAG  = 0;
int IDC_MSG_EDIT_FLAG    = 0;
//int IDC_MHOTKEY_FLAG     = 0;
//int IDC_SHOTKEY_FLAG     = 0;
int IDC_MUTE_HOTKEY_FLAG = 0;
int IDC_PREF_HKEY_FLAG = 0;
//int IDC_ONEHOTKEY_FLAG   = 0;
int IDC_DO_ACTION_FLAG   = 0;
int IDC_ACTION_OPTN_FLAG = 0;
int IDC_DO_OPTN_FLAG     = 0;
int IDC_START_WIN_FLAG   = 0;
//int IDC_SAVE_FLAG        = 0;
//int IDC_CANCEL_FLAG      = 0;
int IDC_AUTO_MON_FLAG    = 0;
int IDC_HP_KEY_FLAG = 0;
int IDC_SUSPEND_OPERATION_FLAG = 0;
int IDC_HIDE_TRAY_FLAG = 0;


int     i           = 0;
WORD    cmd;
POINT   pt;

void    AddTrayIcon( HWND hWnd, UINT uID, UINT uCallbackMsg, UINT uIcon,
                     LPSTR pszToolTip );
void    RemoveTrayIcon( HWND hWnd, UINT uID);
void    ModifyTrayIcon( HWND hWnd, UINT uID, UINT uIcon, LPSTR pszToolTip );

HICON   LoadSmallIcon( HINSTANCE hInstance, UINT uID );

BOOL    ShowPopupMenu( HWND hWnd, POINT *curpos, int wDefaultItem );
void    OnInitMenuPopup( HWND hWnd, HMENU hMenu, UINT uID );

BOOL    OnCommand( HWND hWnd, WORD wID, HWND hCtl );

void    OnTrayIconMouseMove( HWND hWnd );
void    OnTrayIconRBtnUp( HWND hWnd );
void    OnTrayIconLBtnDblClick( HWND hWnd );

void    OnClose( HWND hWnd );

void    RegisterMainWndClass( HINSTANCE hInstance );
void    UnregisterMainWndClass( HINSTANCE hInstance );


#endif // TRAY_H_INCLUDED
