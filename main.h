#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED


#ifndef WINVER
 #define WINVER 0x0500
#endif

#include <wtsapi32.h>

/*  Make the class name into a global variable  */
char szClassName[ ] = "MonitorES";
char szTitleText[ ] = "";

//BOOL WINAPI WTSRegisterSessionNotification(HWND hWnd, DWORD dwFlags);

#define WM_WTSSESSION_CHANGE 0x02B1
#define WTS_SESSION_LOGON                  0x5
#define WTS_SESSION_LOGOFF                 0x6
#define WTS_SESSION_LOCK                   0x7
#define WTS_SESSION_UNLOCK                 0x8

#define WM_CHANGESTATE = WM_APP + 12314;
#define MAX_X 0
#define MAX_Y 0
#define START_X 20

int GLOBAL_OP_FLAG = 1;

#endif // MAIN_H_INCLUDED
