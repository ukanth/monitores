#include "tray.h"
#include "functions.h"

BOOL CALLBACK SetupDlgProc ( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam );


void AddTrayIcon( HWND hWnd, UINT uID, UINT uCallbackMsg, UINT uIcon,
                  LPSTR pszToolTip )
{
    NOTIFYICONDATA  nid;
    uIcon = TRAY_ICON;
    memset( &nid, 0, sizeof( nid ) );

    nid.cbSize              = sizeof( nid );
    nid.hWnd                = hWnd;
    nid.uID                 = uID;
    nid.uFlags              = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage    = uCallbackMsg;
    strcpy( nid.szTip, pszToolTip );
    nid.hIcon = (HICON)LoadIcon((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
                                MAKEINTRESOURCE(uIcon));
    Shell_NotifyIcon( NIM_ADD, &nid );
}

void ModifyTrayIcon( HWND hWnd, UINT uID, UINT uIcon, LPSTR pszToolTip )
{
    NOTIFYICONDATA  nid;

    memset( &nid, 0, sizeof( nid ) );

    nid.cbSize  = sizeof( nid );
    nid.hWnd    = hWnd;
    nid.uID     = uID;

//    if ( uIcon != (UINT)-1 ) {
//        nid.hIcon   = LoadSmallIcon( GetModuleHandle( NULL ), uIcon );
//        nid.uFlags  |= NIF_ICON;
//    }

    if ( pszToolTip )
    {
        strcpy( nid.szTip, pszToolTip );
        nid.uFlags  |= NIF_TIP;
    }

    if ( uIcon != (UINT)-1 || pszToolTip )
        Shell_NotifyIcon( NIM_MODIFY, &nid );
}

void RemoveTrayIcon( HWND hWnd, UINT uID )
{

    NOTIFYICONDATA  nid;

    memset( &nid, 0, sizeof( nid ) );

    nid.cbSize  = sizeof( nid );
    nid.hWnd    = hWnd;
    nid.uID     = uID;
    nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;

    Shell_NotifyIcon( NIM_DELETE, &nid );
}

//-----------------------------------------------------------------------------
void OnClose( HWND hWnd )
{
    //  Remove icon from system tray.
    RemoveTrayIcon( hWnd, ID_TRAYICON );
    PostQuitMessage( 0 );
}

BOOL ShowPopupMenu( HWND hWnd, POINT *curpos, int wDefaultItem )
{
    HMENU hPop = NULL;
    i = 0;
    if ( b_DialogShowing )
        return FALSE;

    hPop = CreatePopupMenu();

    if ( ! curpos )
    {
        GetCursorPos( &pt );
        curpos = &pt;
    }

    //InsertMenu( hPop, i++, MF_BYPOSITION | MF_STRING, ID_ABOUT, "About" );
    InsertMenu( hPop, i++, MF_BYPOSITION | MF_STRING, ID_OPTIONS, "Settings" );
    InsertMenu( hPop, i++, MF_BYPOSITION | MF_STRING, ID_ABOUT, "About" );
    InsertMenu( hPop, i++, MF_BYPOSITION | MF_STRING, ID_EXIT, "Exit" );

    SetMenuDefaultItem( hPop, ID_OPTIONS, FALSE );

    SetFocus( hWnd );

    SendMessage( hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0 );

    cmd = TrackPopupMenu( hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON
                          | TPM_RETURNCMD | TPM_NONOTIFY,
                          curpos->x, curpos->y, 0, hWnd, NULL );

    SendMessage( hWnd, WM_COMMAND, cmd, 0 );

    DestroyMenu( hPop );

    return cmd;
}

void OnTrayIconMouseMove( HWND hWnd )
{
    //  stub
}

//  Right-click on tray icon displays menu.
void OnTrayIconRBtnUp( HWND hWnd )
{
    SetForegroundWindow( hWnd );
    ShowPopupMenu( hWnd, NULL, -1 );
    PostMessage( hWnd, APPWM_NOP, 0, 0 );
}

//-----------------------------------------------------------------------------
void OnTrayIconLBtnDblClick( HWND hWnd )
{
    if (!b_DialogShowing )
    {
        DialogBox ( GetModuleHandle ( NULL ), MAKEINTRESOURCE ( IDD_SETUP_DIALOG ), hWnd, SetupDlgProc );
        b_DialogShowing = FALSE;
    }
}


//-----------------------------------------------------------------------------
void OnInitMenuPopup( HWND hWnd, HMENU hPop, UINT uID )
{
    //  stub
}

int GetCheckValue(HWND hwnd, UINT message)
{
    return SendDlgItemMessage ( hwnd, message, BM_GETCHECK, 0, 0 );
}

/*
void ProcessHotkey(HWND hwndHot, HWND hwndMain)
{
    WORD wHotkey;
    // Retrieve the hot key (virtual key code and modifiers).
    wHotkey = SendMessage(hwndHot, HKM_GETHOTKEY, 0, 0);
    // Use the result as wParam for WM_SETHOTKEY.
    SendMessage(hwndMain, WM_SETHOTKEY, wHotkey, 0);
}
*/
/* Setup Dialog screen */

BOOL CALLBACK SetupDlgProc ( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
    b_DialogShowing = TRUE;
    switch ( Message )
    {
    case WM_INITDIALOG:
        LoadRegistry();
        LoadYahooUser(hwnd);
        LoadCustomMessage(hwnd);
        SetValues(hwnd);
        //Filling Hibernate Timers
        return TRUE;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            EndDialog ( hwnd, 0 );
        }
        return 0;
    case WM_DESTROY:
        EndDialog ( hwnd, 0 );
        return TRUE;
    case WM_COMMAND:
        switch ( LOWORD ( wParam ) )
        {
        case IDC_MONITOR:
            IDC_MONITOR_FLAG = GetCheckValue(hwnd,IDC_MONITOR);
            break;
        case IDC_MON_HOTKEY:
            IDC_MON_HOTKEY_FLAG = GetCheckValue(hwnd,IDC_MON_HOTKEY);
            break;
        case IDC_SCREEN:
            IDC_SCREEN_FLAG = GetCheckValue(hwnd,IDC_SCREEN);
            break;
        case IDC_SCR_HOTKEY:
            IDC_SCR_HOTKEY_FLAG = GetCheckValue(hwnd,IDC_SCR_HOTKEY);
            break;
        case IDC_DISABLE_SCREEN:
            IDC_DISABLE_SCREEN_FLAG = GetCheckValue(hwnd,IDC_DISABLE_SCREEN);
            break;
        case IDC_HIDE_TRAY:
            IDC_HIDE_TRAY_FLAG = GetCheckValue(hwnd,IDC_HIDE_TRAY);
            break;
        case IDC_AUTO_MON:
            IDC_AUTO_MON_FLAG = GetCheckValue(hwnd,IDC_AUTO_MON);
            break;
        case IDC_HP_KEY:
            IDC_HP_KEY_FLAG = GetCheckValue(hwnd,IDC_HP_KEY);
            if (IDC_HP_KEY_FLAG)
            {
                ControlDialogCheck(hwnd,IDC_ONEHOTKEY,FALSE);
            }
            else
            {
                ControlDialogCheck(hwnd,IDC_ONEHOTKEY,TRUE);
            }
            break;
        case IDC_DO_ACTION:
            IDC_DO_ACTION_FLAG = GetCheckValue(hwnd,IDC_DO_ACTION);
            break;
        case IDC_START_WIN:
            IDC_START_WIN_FLAG = GetCheckValue(hwnd,IDC_START_WIN);
            break;
        case IDC_MUTE_VOL:
            IDC_MUTE_VOL_FLAG = GetCheckValue(hwnd,IDC_MUTE_VOL);
            break;
        case IDC_MUTE_HOTKEY:
            IDC_MUTE_HOTKEY_FLAG = GetCheckValue(hwnd,IDC_MUTE_HOTKEY);
            break;
            // Control Messenger settings
        case IDC_SET_AWAY:
            IDC_SET_AWAY_FLAG = GetCheckValue(hwnd,IDC_SET_AWAY);
            if (IDC_SET_AWAY_FLAG)
            {
                ControlDialogCheck(hwnd,IDC_YAHOO,TRUE);
                if(IDC_YAHOO_FLAG) ControlDialogCheck(hwnd,IDC_YAHOO_EDIT,TRUE);
                ControlDialogCheck(hwnd,IDC_CUSTOM_MSG,TRUE);
                if(IDC_CUSTOM_MSG_FLAG) ControlDialogCheck(hwnd,IDC_MSG_EDIT,TRUE);
            }
            else
            {
                ControlDialogCheck(hwnd,IDC_YAHOO,FALSE);
                ControlDialogCheck(hwnd,IDC_YAHOO_EDIT,FALSE);
                ControlDialogCheck(hwnd,IDC_CUSTOM_MSG,FALSE);
                ControlDialogCheck(hwnd,IDC_MSG_EDIT,FALSE);
            }
            break;
        case IDC_YAHOO:
            IDC_YAHOO_FLAG = GetCheckValue(hwnd,IDC_YAHOO);
            if (IDC_YAHOO_FLAG) ControlDialogCheck(hwnd,IDC_YAHOO_EDIT,TRUE);
            else ControlDialogCheck(hwnd,IDC_YAHOO_EDIT,FALSE);
            break;
        case IDC_CUSTOM_MSG:
            IDC_CUSTOM_MSG_FLAG = GetCheckValue(hwnd,IDC_CUSTOM_MSG);
            if (IDC_CUSTOM_MSG_FLAG) ControlDialogCheck(hwnd,IDC_MSG_EDIT,TRUE);
            else ControlDialogCheck(hwnd,IDC_MSG_EDIT,FALSE);
            break;
            // Control media programs
        case IDC_PAUSE_MEDIA:
            IDC_PAUSE_MEDIA_FLAG = GetCheckValue(hwnd,IDC_PAUSE_MEDIA);
            break;
        case IDC_MEDIA_HOTKEY:
            IDC_MEDIA_HOTKEY_FLAG = GetCheckValue(hwnd,IDC_MEDIA_HOTKEY);
            break;
            //case
        case IDC_SAVE:
            //save all settings to registry
            SaveAll(hwnd);
            EndDialog ( hwnd, IDC_SAVE );
            break;
        case IDC_CANCEL:
            EndDialog ( hwnd, IDC_CANCEL );
            b_DialogShowing = FALSE;
            break;
        }
        return TRUE;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL OnCommand( HWND hWnd, WORD wID, HWND hCtl )
{
    if ( b_DialogShowing )
        return TRUE;

    b_DialogShowing = TRUE;
    //  Have a look at the command and act accordingly
    switch ( wID )
    {
    case ID_OPTIONS:
        DialogBox ( GetModuleHandle ( NULL ), MAKEINTRESOURCE ( IDD_SETUP_DIALOG ), hWnd, reinterpret_cast<DLGPROC>(SetupDlgProc) );
        b_DialogShowing = FALSE;
        return 0;

    case ID_ABOUT:
        b_DialogShowing = TRUE;
        MessageBox( hWnd, HELP_ABOUT, "About MonitorES - Version 1.1 Final",
                    MB_ICONINFORMATION | MB_OK );
        b_DialogShowing = FALSE;
        return 0;

    case ID_EXIT:
        PostMessage( hWnd, WM_CLOSE, 0, 0 );
        return 0;
    }

    b_DialogShowing = FALSE;
    return 0;
}

