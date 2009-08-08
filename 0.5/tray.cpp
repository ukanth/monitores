//Original Code by Massimiliano Miani
#include <windows.h>
#include "tray.h"

BOOL  LoadAppIcon();                            //Application icon
void  CreateItemList(HMENU hMenu);              //Crate Popup menu
BOOL  SetIcon(PSTR pszTip, int nAction);        //Change tray icon
void  SetBitmaps(HMENU hMenu);                  //Set bitmaps for popup menu items
DWORD CheckMenuItem(UINT IDItem, BOOL Checked); //Check/uncheck an item in popup menu
BOOL  IsItemChecked(UINT IDItem);               //Is popup item checked?

//POPUP MENU
HMENU hMenu;                                    // menu handle
HWND  hwnd;                                     // main window handle
//TRAY
bool  bTray;                                    // tray state (is window minimized in tray?)
bool isMax = false;
// Popup menu events
// Return TRUE if an event is processed, otherwise FALSE;

void MinimizeOnStart(boolean flag)
{
    if(flag) bTray =  true;
    else bTray = false;
     
}

void alert1(int a) {
  TCHAR sMessage[255];
  wsprintf(sMessage, TEXT("%d"), a); 
  MessageBox(NULL, sMessage,  "Message", MB_OK | MB_ICONINFORMATION);
}  

void alert2(char *item) {
    MessageBox(NULL, item,  TEXT("Message"), MB_OK | MB_ICONINFORMATION);
}
BOOL OnCommandTrayMenu(WPARAM wParam)
{
 switch(LOWORD(wParam))
 {
    //POPUP MENU 
    case IDM_ONTOP: //ALWAYS ON TOP
    {   
        HWND HWND_ORDER;
        BOOL Checked;
        
        //If menu item is checked put window on top
		if (IsItemChecked(LOWORD(wParam))){ 
		  HWND_ORDER = HWND_NOTOPMOST;  // 
		  Checked=false;
		  //isMonitor = true;
		}else{
	      HWND_ORDER = HWND_TOPMOST;
	      Checked=true;
		}
		
		CheckMenuItem(LOWORD(wParam),Checked);
		//SetWindowPos(hwnd, HWND_ORDER, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		
	}return TRUE;	
    case IDM_ONTOP_2: //ALWAYS ON TOP
    {   
        HWND HWND_ORDER;
        BOOL Checked;
        
        //If menu item is checked put window on top
		if (IsItemChecked(LOWORD(wParam))){ 
		  HWND_ORDER = HWND_NOTOPMOST;  // 
		  Checked=false;
		  //isMonitor = true;
		}else{
	      HWND_ORDER = HWND_TOPMOST;
	      Checked=true;
		}
		
		CheckMenuItem(LOWORD(wParam),Checked);
		//SetWindowPos(hwnd, HWND_ORDER, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		
	}return TRUE;	      
	//POPUP MENU ******************************************************//
	case IDM_MINIMIZE:{ //minimize/restore window
		//NOTE: SW_SHOWMINIMIZED and SW_RESTORE cause WM_SIZE event
        //(see also OnSizeTray())       
        if (!bTray){                      
			ShowWindow(hwnd, SW_SHOWMINIMIZED);//minimize window in tray
     				
		}else{       			
			ShowWindow(hwnd, SW_RESTORE);//restore window 
        }	
	}return TRUE;
	case IDM_ABOUT:{
         MessageBox(hwnd, APP_ABOUT, TEXT(" About MonitorES "), MB_ICONINFORMATION);
    }return TRUE;	
	case IDM_CLOSE: {
        SendMessage(hwnd, WM_DESTROY, 0, 0);
	}return TRUE;
	
 }
  
return FALSE; 
}
/**************************************************************************************************/
/**************************************************************************************************/
//Gestisce il ridimensionamento in tray ed il ripristino dalla tray
BOOL OnSizeTray(WPARAM wParam, int flag)
{
    if (wParam == SIZE_MINIMIZED || flag == 1)
    {   
        ModifyMenu(hMenu, IDM_MINIMIZE, MF_BYCOMMAND, IDM_MINIMIZE, IDM_TEXT_RESTORE);//CAMBIA IL TESTO DEL MENU
		SetIcon("Monitor Energy Saver", 1);
		ShowWindow(hwnd, SW_HIDE);
		bTray = true;
    } else {  
        ModifyMenu(hMenu, IDM_MINIMIZE, MF_BYCOMMAND, IDM_MINIMIZE, IDM_TEXT_MINIMIZE);//CAMBIA IL TESTO DEL MENU
		SetIcon("Monitor Energy Saver", 0);
		bTray = false;
    }
  return bTray;
}

//On startup Minimize

BOOL OnTray(WPARAM wParam)
{
        ModifyMenu(hMenu, IDM_MINIMIZE, MF_BYCOMMAND, IDM_MINIMIZE, IDM_TEXT_MINIMIZE);//CAMBIA IL TESTO DEL MENU
		SetIcon("Monitor Energy Saver", 1);
		ShowWindow(hwnd, SW_HIDE);
		bTray = false;
        return bTray;
}
/**************************************************************************************************/
/**************************************************************************************************/
// Mouse clicks on tray
void OnNotifyTray(LPARAM lParam)
{
  switch(lParam)
	{
        case WM_RBUTTONDOWN:           
            POINT pt;// point structure
            GetCursorPos(&pt);// get the cursors position
            SetForegroundWindow(hwnd);// set the foreground window
            TrackPopupMenu(hMenu,TPM_RIGHTALIGN,pt.x,pt.y,0,hwnd,NULL);
            break;
		case WM_LBUTTONDOWN:
            if(!isMax) { 
               SetForegroundWindow(hwnd);
               ShowWindow(hwnd, SW_RESTORE);
               ModifyMenu(hMenu, IDM_MINIMIZE, MF_BYCOMMAND, IDM_MINIMIZE, IDM_TEXT_MINIMIZE);
               SetIcon("Monitor Energy Saver", 1);
               isMax = true;
            } else {
               ShowWindow(hwnd, SW_HIDE);
               ModifyMenu(hMenu, IDM_MINIMIZE, MF_BYCOMMAND, IDM_MINIMIZE, IDM_TEXT_RESTORE);//CAMBIA IL TESTO DEL MENU
		       SetIcon("Monitor Energy Saver", 1);
               isMax = false;                        
            }          
			break;
	     default:break;
	}
  PostMessage(hwnd,WM_NULL,0,0);
}	

/**************************************************************************************************/
/**************************************************************************************************/
// Bye bye...
void OnDestroyTray()
{
    //Destroy popup menu
	DestroyMenu(hMenu);
	//Ensure to remove tray icon
    SetIcon("", 0);
}
/**************************************************************************************************/
/**************************************************************************************************/
// Return tray state, TRUE if the app. is minimized to tray, otherwise FALSE.
BOOL TRAY_IsInTray(){ return bTray; }

/**************************************************************************************************/
/**************************************************************************************************/
//Show popup menu
void TRAY_Menu_Show()
{			
	POINT pt;// point structure
	GetCursorPos(&pt);// get the cursors position
	SetForegroundWindow(hwnd);// set the foreground window
	TrackPopupMenu(hMenu,TPM_RIGHTALIGN,pt.x,pt.y,0,hwnd,NULL);// track the popup
	PostMessage(hwnd,WM_NULL,0,0);// post a null message	    
}
/**************************************************************************************************/
/**************************************************************************************************/
//Initialize tray
void TRAY_Init(HWND hWnd)
{
    hwnd = hWnd;
    hMenu=CreatePopupMenu();
    CreateItemList(hMenu); 
    LoadAppIcon();
} 
/**************************************************************************************************/
/**************************************************************************************************/


/*************
 *************
 ** PRIVATE **
 *************
 *************/


/**************************************************************************************************/
/**************************************************************************************************/
//Set bitmaps for popup menu items
void SetBitmaps(HMENU hMenu)
{    
    //Checked item bitmap
    HBITMAP hBitmapChecked = LoadBitmap((HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),
                                                    MAKEINTRESOURCE(IDB_MENU_CHECK));
    SetMenuItemBitmaps(hMenu, IDM_MINIMIZE, MF_BYCOMMAND, hBitmapChecked, 
                                                                NULL); 
    //Close bitmap
    hBitmapChecked = LoadBitmap((HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE), 
                                            MAKEINTRESOURCE(IDB_MENU_CLOSE));
    SetMenuItemBitmaps(hMenu, IDM_CLOSE, MF_BYCOMMAND, hBitmapChecked, NULL);
    
    //About bitmap  
    hBitmapChecked = LoadBitmap((HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE), 
                                            MAKEINTRESOURCE(IDB_MENU_ABOUT));
    SetMenuItemBitmaps(hMenu, IDM_ABOUT, MF_BYCOMMAND, hBitmapChecked, NULL);    
}
/**************************************************************************************************/
/**************************************************************************************************/
//Create menu items
void CreateItemList(HMENU hMenu)
{
	int ret=0; // buffer for errors
 		
	// CREATE POPUP MENU ( on the fly )
	//hMenu = CreatePopupMenu(); // create the popupmenu (See BOOL TRAY_Init()...)
	//AppendMenu(hMenu,MF_STRING ,IDM_ONTOP,IDM_TEXT_ONTOP);// append menu  
	//AppendMenu(hMenu,MF_STRING ,IDM_ONTOP,IDM_TEXT_MON);
  	//AppendMenu(hMenu,MF_STRING ,IDM_ONTOP,IDM_TEXT_SCR);// append menu  
	AppendMenu(hMenu,MF_STRING ,IDM_MINIMIZE,IDM_TEXT_MINIMIZE);// append menu
	//AppendMenu(hMenu,MF_SEPARATOR,0,0);// append menu
	AppendMenu(hMenu,MF_STRING,IDM_ABOUT,IDM_TEXT_ABOUT);// append menu  
	AppendMenu(hMenu,MF_STRING ,IDM_CLOSE,IDM_TEXT_CLOSE);// append menu 
	//AppendMenu(hMenu,MF_SEPARATOR,0,0);// append menu
	
	
    SetBitmaps(hMenu); 
}
/**************************************************************************************************/
/**************************************************************************************************/
//Load app. icon
BOOL LoadAppIcon()
{
    // Load icon from resource
    HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON), 
                                                IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    if (hIcon) {
    // Associate icon to the app.
	SendMessage(hwnd, WM_SETICON, WPARAM (ICON_SMALL), LPARAM (hIcon));// App. icon
	SendMessage(hwnd, WM_SETICON, WPARAM (ICON_BIG), LPARAM (hIcon));  //ALT+TAB icon
	return true;
    }

return false;
}
/**************************************************************************************************/
/**************************************************************************************************/
// Add/Set tray icon
// nAction - 1 = Create Tray Icon
//           0 = Remove Tray Icon
//           n = Change Tray Icon
// pszTip = Tray icon ToolTip text
BOOL SetIcon(PSTR pszTip , int nAction) 
{              
UINT  uIcon; // Icon ID buffer
DWORD dMsg;  // Message buffer (ADD/DELETE/MODIFY)

	NOTIFYICONDATA ni;
	ni.cbSize = sizeof(ni);
	ni.uID = 0;
	lstrcpyn(ni.szTip, pszTip, sizeof(ni.szTip)); 
	ni.hWnd = hwnd;
	ni.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON; // We want to use icon, tip, and callback message
	ni.uCallbackMessage = WM_NOTIFYICONTRAY;      // Our custom callback message (WM_APP + 1)
    
    switch (nAction){
        case 0:// Remove tray icon
                return Shell_NotifyIcon(NIM_DELETE, &ni);
           break; 
        case 1://Add icon in tray
                uIcon=APP_ICON; // Set tray icon
                dMsg=NIM_ADD; // Add tray icon        
           break;
        case 2:// Modify current icon in tray
                uIcon=TRAY_ICON; // Set new tray icon	
                dMsg=NIM_MODIFY; // Modify tray icon     
           break;               
        case 3:// Modify current icon in tray
                uIcon=APP_ICON; // Set new tray icon	
                dMsg=NIM_MODIFY; // Modify tray icon      
           break;                                
     }
     
     //Load selected icon
     ni.hIcon = (HICON)LoadIcon((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
                                                       MAKEINTRESOURCE(uIcon));	
     // Windooows, ohohooo, we have changed icon! :)                                                
     return Shell_NotifyIcon(dMsg, &ni);       
     
     
}
/**************************************************************************************************/
/**************************************************************************************************/
// Set checked/unchecked state of an item in the popup menu
// Return value: MF_CHECKED, MF_UNCHECKED or -1 if item not found.
DWORD CheckMenuItem(UINT IDItem,BOOL Checked)
{
  UINT uCheck;
  uCheck = Checked ? MF_CHECKED : MF_UNCHECKED; //retrieve state (checked/unchecked)

  return CheckMenuItem(hMenu, IDItem, MF_BYCOMMAND|uCheck);
}
/**************************************************************************************************/
/**************************************************************************************************/
//Is popup item checked?
BOOL IsItemChecked(UINT IDItem)
{
DWORD uCheck;

  uCheck = CheckMenuItem(hMenu, IDItem, MF_BYCOMMAND);
  
  if (uCheck != -1){
     return (uCheck == MF_CHECKED) ? 1 : 0; //retrieve state (checked/unchecked)
  }
  
return 0;
}

