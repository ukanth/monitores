// tray.h

//About message
//Original Code by Massimiliano Miani

#define APP_ABOUT "Monitor Energy Saver Lite \
\n\n\Monitor Energy Saver helps to turn off monitors \
automatically when user locks \
workstation or laptop or with hotkey(CTRL + F2)\
Coded by Umakanthan Chandran (cumakt@gmail.com) \n\n Sincere Thanks \
To : Massimiliano Miani(Tray) \
Everaldo Coelho(Icons) \n Disclaimer: USE THIS TOOL AT YOUR OWN RISK!"

//Other message
#define MY_MSG "Minimize this window in tray and click with \n\nleft button on icon to see a 'main menu'"

//Application icon, Tray icon and Popup menu bitmaps
#define APP_ICON       90 //app icon
#define TRAY_ICON      91 //tray icon
#define IDB_MENU_CHECK 92 //checked item bitmap (popup menu)
#define IDB_MENU_CLOSE 93 //close bitmap (popup menu)
#define IDB_MENU_ABOUT 94 //about bitmap (popup menu)

//#define MES_OPTIONS 95
//#define MES_ABOUT 96
//#define MES_EXIT 97

#define ABOUTDLG 391

#define WM_NOTIFYICONTRAY (WM_APP + 1)//tray message

//Popup Menu items
#define IDM_MINIMIZE   421

#define IDM_ABOUT	   422
#define IDM_TEXT_ABOUT "  About "

#define IDM_CLOSE	   423
#define IDM_TEXT_CLOSE "  Close "

#define IDM_AUTOSTART	   424
#define IDM_TEXT_START " Auto Start"

#define IDM_MON	   425
#define IDM_TEXT_MON   " Enable Turnoff"


//Functions
void  TRAY_Init(HWND hwnd,int flag);             //Create tray and its popup menu
BOOL  TRAY_IsInTray(void);              //Is window minimized in tray?
void  TRAY_Menu_Show(void);             //Show popup menu
void  OnNotifyTray(LPARAM lParam);      //Tray message (mouse clicks on tray icon)
BOOL  OnSizeTray(WPARAM wParam);        //WM_SIZE message
void  OnDestroyTray(void);              //WM_DESTROY message
BOOL  OnCommandTrayMenu(WPARAM wParam); //Popup menu events
BOOL  OnTray(WPARAM wParam);

