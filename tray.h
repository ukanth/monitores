// tray.h

//About message
//Original Code by Massimiliano Miani

#define APP_ABOUT "Monitor Energy Saver 0.5 \
\n\n\Monitor Energy Saver helps to turn off monitor \
or activate screensaver automatically when user locks \
workstation or laptop or with hotkey(CTRL + F2).It also helps to \
pause all the media players and set custom/away status message \
of Messengers and restore it when you back \n\n\
\t\t\tCoded by Umakanthan Chandran (cumakt@gmail.com) \n\nSincere Thanks \
To : Massimiliano Miani(Tray) Viktor Brange(Gtalk) \
Everaldo Coelho(Icons) \n\t\t\tDisclaimer: USE THIS TOOL AT YOUR OWN RISK!"

//Other message
#define MY_MSG "Minimize this window in tray and click with \n\nleft button on icon to see a 'button effect'"

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
#define ID_DIALOGOK 394
#define ID_DIALOGCANCEL 395
#define IDMEDIA_DISABLE 396
#define IDCHECK_START 397
#define IDD_STATIC1 398
#define IDD_STATIC2 399
#define IDMESSENGER 400
#define ID_HOTKEY_DIALOG 401
#define IDSOUND_CONTROL 402
#define IDCHECK_MINIMIZE 403
#define ID_PAUSE_HOT 404
#define ID_YAHOO_USERNAME 405
#define IDD_STATIC3 406
#define ID_CUSTOM_MESSAGE 407
#define WM_NOTIFYICONTRAY (WM_APP + 1)//tray message

//Popup Menu items
#define IDM_MINIMIZE   421 							
#define IDM_TEXT_MINIMIZE " Minimize to tray"			
#define IDM_TEXT_RESTORE "  Show "

#define IDM_ABOUT	   422							
#define IDM_TEXT_ABOUT "  About "

#define IDM_CLOSE	   423						
#define IDM_TEXT_CLOSE "  Close "

#define IDM_ONTOP	   424						
#define IDM_TEXT_SCR " Enable ScreenSaver On"

#define IDM_ONTOP_2	   425						
#define IDM_TEXT_MON " Enable Monitor Off"


//Functions
void  TRAY_Init(HWND hwnd);             //Create tray and its popup menu 
BOOL  TRAY_IsInTray(void);              //Is window minimized in tray?
void  TRAY_Menu_Show(void);             //Show popup menu
void  OnNotifyTray(LPARAM lParam);      //Tray message (mouse clicks on tray icon)
BOOL  OnSizeTray(WPARAM wParam,int flag);        //WM_SIZE message
void  OnDestroyTray(void);              //WM_DESTROY message 
BOOL  OnCommandTrayMenu(WPARAM wParam); //Popup menu events
BOOL  OnTray(WPARAM wParam);
void  MinimizeOnStart(boolean flag);

