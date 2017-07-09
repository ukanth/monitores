// Common Methods

#ifndef WINVER
 #define WINVER 0x0500
#endif

#include <tlhelp32.h>

#define NOTIFY_FOR_THIS_SESSION 0

void alert(char *item) {
    MessageBox(NULL, item,  TEXT("Message"), MB_OK | MB_ICONINFORMATION);
}

void alert(int a) {
  TCHAR sMessage[255];
  wsprintf(sMessage, TEXT("%d"), a); 
  MessageBox(NULL, sMessage,  "Message", MB_OK | MB_ICONINFORMATION);
}  

void alert(boolean item) {
    if(item)  
        MessageBox(NULL, "TRUE",  TEXT("Message"), MB_OK | MB_ICONINFORMATION);
    else 
        MessageBox(NULL, "FALSE",  TEXT("Message"), MB_OK | MB_ICONINFORMATION);     
}

int StringLength(char inputString[])
{
    int length = 0;    
    for (int i = 0; inputString[i]!= '\0'; i++)
        length++;    
    return length;
}

//Unregister WTSAPI32.dll to get notification
void UnregisterSession(HWND hwnd) {   
    typedef DWORD (WINAPI *tWTSUnRegisterSessionNotification)( HWND,DWORD );

    tWTSUnRegisterSessionNotification pWTSUnRegisterSessionNotification=0;
    HINSTANCE handle = ::LoadLibrary("wtsapi32.dll");
    pWTSUnRegisterSessionNotification = (tWTSUnRegisterSessionNotification) ::GetProcAddress(handle,"WTSUnRegisterSessionNotification");
    if (pWTSUnRegisterSessionNotification) {
        pWTSUnRegisterSessionNotification(hwnd,NOTIFY_FOR_THIS_SESSION);
    }
    ::FreeLibrary(handle);

}

//Register WTSAPI32.dll to get notification
void RegisterSession(HWND hwnd) {
    typedef DWORD (WINAPI *tWTSRegisterSessionNotification)( HWND,DWORD );

    tWTSRegisterSessionNotification pWTSRegisterSessionNotification=0;
    HINSTANCE handle = ::LoadLibrary("wtsapi32.dll");
    pWTSRegisterSessionNotification = (tWTSRegisterSessionNotification) :: GetProcAddress(handle,"WTSRegisterSessionNotification");
    if (pWTSRegisterSessionNotification) {
        pWTSRegisterSessionNotification(hwnd,NOTIFY_FOR_THIS_SESSION);
    }
    ::FreeLibrary(handle);
}


BOOL isNotScreenSaverRunning ()
{
   BOOL bSaver = ::SystemParametersInfo (SPI_GETSCREENSAVERRUNNING,0,&bSaver,SPIF_SENDCHANGE);
   return bSaver; 
}


char *GetProcessPath( DWORD dwPID )
{
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
  MODULEENTRY32 me32;

  // Take a snapshot of all modules in the specified process.
  hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );

  // Set the size of the structure before using it.
  me32.dwSize = sizeof( MODULEENTRY32 );

  // Now walk the module list of the process,
  // and display information about each module
  int count = 0;
  char *sExePath;
  do
  {
     if(count == 1)
     { 
       sprintf(sExePath, TEXT("%s"),me32.szExePath);
       break;
     }
     count++;     
  } while( Module32Next( hModuleSnap, &me32 ));
  CloseHandle( hModuleSnap );
  return sExePath;
}

int cmpistr(const char* s1, const char* s2)
{
    int    c1, c2;
    int    cmp = 0;
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
void ControlFooBar() {
    PROCESSENTRY32 entry;
    entry.dwFlags = sizeof( PROCESSENTRY32 );
    int iReturn = 0;
    char* getPath;
    HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    HANDLE hProcess;
    if ( Process32First( snapshot, &entry ) == TRUE ) {         
    	while ( Process32Next( snapshot, &entry ) == TRUE ) {
    		if ( cmpistr( entry.szExeFile, "foobar2000.exe" ) == 0 ) { 
    			hProcess = OpenProcess( PROCESS_QUERY_INFORMATION , FALSE, entry.th32ProcessID );
                getPath =  GetProcessPath( entry.th32ProcessID );  // Do stuff..
                char command[256];
                sprintf(command, TEXT("%s"), getPath);
                iReturn = (int) ShellExecute (NULL, "open", command, "/playpause", NULL, SW_HIDE); 
    			CloseHandle( hProcess );

    		}
    	}
    }

    CloseHandle( snapshot );
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
 
 
