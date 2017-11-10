#include <windows.h>
#include "packages/minhook.1.3.3/lib/native/include/MinHook.h"
#include "Detour.h"
#include <stdio.h>
#include <Softpub.h>

#pragma comment(lib, "packages/minhook.1.3.3/lib/native/lib/libMinHook.lib")
#pragma comment(lib, "Wintrust.lib")

typedef HANDLE (WINAPI *CREATEFILE) 
(
	
	LPCTSTR lpFileName, 
	DWORD dwDesiredAccess, 
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes, 
	HANDLE hTemplateFile 

);

typedef LONG (WINAPI* WINVERIFYTRUST) 
(
	
	HWND hwnd, 
	GUID *pgActionID, 
	LPVOID pWVTData 

);

CREATEFILE pCreateFileW = NULL;
WINVERIFYTRUST pWinVerifyTrust = NULL;

WCHAR exePathW[8192];
WCHAR xlivePathW[8192];


static void __stdcall Debug_Log_W(WCHAR *name)
{
	
	static FILE *fp_w = 0;


#ifndef NO_TRACE
	return;
#endif
	
	if( fp_w == 0 )
	{
		
		fp_w = _wfopen( L"xlive-log-w.txt", L"w" );
		
		fwprintf( fp_w, L"%s\n", xlivePathW );
	
	}
	
	fwprintf( fp_w, L"%s\n", name );	
	fflush( fp_w );

}


static void __stdcall Path_Strip_W( WCHAR *name )
{
	
	int len = wcslen(name);

	while (name[len] != L'\\')
	{

		len--;

	}

	name[len+1] = 0;

}


static void __stdcall DetourCreateFileW_code( HANDLE &result, LPCTSTR lpFileName )
{
	
	Debug_Log_W(L"===  DetourCreateFileW");
	Debug_Log_W((WCHAR *) lpFileName);

}


static HANDLE WINAPI DetourCreateFileW(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	
	HANDLE result;

	result = pCreateFileW( lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	DetourCreateFileW_code(result, lpFileName);

	return result;

}


static LONG WINAPI DetourWinVerifyTrust(HWND hwnd, GUID *pgActionID, LPVOID pWVTData)
{
	
	return 0;

}


void Detour()
{
	
	GetModuleFileNameW(NULL, (LPWCH) &exePathW, 8192);
	wcscpy(xlivePathW, exePathW);
	Path_Strip_W(xlivePathW);
	wcscat(xlivePathW, L"xlive.dll");


#ifndef NO_TRACE
	Debug_Log_W(L"DetourCreateFileW");
#endif

	MH_Initialize();

	DWORD *addr = (DWORD *) (&WinVerifyTrust);
	addr = (DWORD *) ( (BYTE *) addr + 2 );
	addr = (DWORD *) ( *addr );

	MH_CreateHook( (VOID *) ( *addr ), DetourWinVerifyTrust, reinterpret_cast<void**>(&pWinVerifyTrust) );
	MH_QueueEnableHook((VOID *) ( *addr ));
	MH_ApplyQueued();

}
