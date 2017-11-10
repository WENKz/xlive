// -No Copyright- 2010 Stanislav "listener" Golovin
// This file donated to the public domain

#include "stdafx.h"
#include "Detour.h"
#include <io.h>
#include <fcntl.h>
#include <iostream>

using namespace std;

HMODULE hThis = NULL;
CRITICAL_SECTION d_lock;
BOOL consoleEnabled = FALSE;
UINT g_online = 1;
UINT g_debug = 0;
CHAR g_server[16 + 1]  = "127.0.0.1";
UINT g_signin[4] = { 1,0,0,0 };
CHAR g_szUserName[4][XUSER_NAME_SIZE] = { "xLiveLess", "xLiveLess", "xLiveLess", "xLiveLess" };
XUID xFakeXuid[4] = { 0xEE000000DEADC0DE, 0xEE000000DEADC0DE, 0xEE000000DEADC0DE, 0xEE000000DEADC0DE };
CHAR g_profileDirectory[512] = "Profiles";
CHAR g_password[XUSER_PASSWORD_SIZE] = "Password";

std::wstring dlcbasepath;

VOID Console()
{

#ifdef NDEBUG
	return;
#endif

	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
	FILE* hf_out = _wfdopen(hCrt, L"w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;
	consoleEnabled = TRUE;

}


std::string ModulePathA(HMODULE hModule = NULL)
{
	
	static char strPath[MAX_PATH];
	
	GetModuleFileNameA(hModule, strPath, MAX_PATH);
	
	return strPath;

}


std::wstring ModulePathW(HMODULE hModule = NULL)
{
	static wchar_t strPath[MAX_PATH];
	GetModuleFileNameW(hModule, strPath, MAX_PATH);
	
	return strPath;

}


#ifndef NO_TRACE

FILE * logfile = NULL;

void trace(LPWSTR message, ...)
{
	if (!logfile)
	{
		
		return;

	}

	if (!consoleEnabled)
	{

		Console();

	}

	EnterCriticalSection (&d_lock);
	
	SYSTEMTIME	t;
	
	GetLocalTime (&t);

	fwprintf (logfile, L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	wprintf (L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

	va_list	arg;
	va_start (arg, message);

	vwprintf (message, arg);
	wprintf(L"\n");

	vfwprintf (logfile, message, arg);
	fwprintf(logfile, L"\n");

	fflush (stdin);
	fflush (logfile);
	va_end (arg);
	LeaveCriticalSection (&d_lock);

}


void trace2(LPWSTR message, ...)
{
	
	if (!logfile)
	{

		return;

	}

	if (!consoleEnabled)
	{

		Console();

	}

	EnterCriticalSection (&d_lock);

	SYSTEMTIME	t;

	GetLocalTime (&t);

	fwprintf (logfile, L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	wprintf (L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

	va_list	arg;
	va_start (arg, message);

	vwprintf (message, arg);
	wprintf(L"\n");

	vfwprintf (logfile, message, arg);
	fwprintf(logfile, L"\n");

	fflush (stdin);
	fflush (logfile);
	va_end (arg);
	LeaveCriticalSection (&d_lock);

}
#endif


void InitInstance()
{
	
	static bool init = true;

	if(init)
	{
		
		init = false;

#ifdef _DEBUG
		
		int CurrentFlags;
		CurrentFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		CurrentFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
		CurrentFlags |= _CRTDBG_LEAK_CHECK_DF;
		CurrentFlags |= _CRTDBG_CHECK_ALWAYS_DF;
		_CrtSetDbgFlag(CurrentFlags);

#endif
		
		InitializeCriticalSection (&d_lock);

		dlcbasepath = L"DLC";
		
		FILE *fp;
		fp = fopen( "xlive.ini", "r" );

		if( fp )
		{
			
			while( !feof(fp) )
			{
				
				char str[256];

				fgets( str, 256, fp );

#define CHECK_ARG_STR(x,y) \
				if( strstr( str,x ) == str ) \
				{ \
					sscanf( str + strlen(x), "%s", &y ); \
					continue; \
				}


#define CHECK_ARG(x,y) \
				if( strstr( str,x ) == str ) \
				{ \
					sscanf( str + strlen(x), "%d", &y ); \
					continue; \
				}


#define CHECK_ARG_I64(x,y) \
				if( strstr( str,x ) == str ) \
				{ \
					sscanf( str + strlen(x), "%I64x", &y ); \
					continue; \
				}
	
				CHECK_ARG_STR( "Player =", g_szUserName[0] );
				CHECK_ARG_STR("Password =", g_password);
				CHECK_ARG_STR( "ServerIp =", g_server);

			}

			
			fclose(fp);

		}

#ifndef NO_TRACE
		
		logfile = _wfopen(L"xlive_trace.log", L"wt");
		
		WCHAR gameName[256];

		GetModuleFileNameW( NULL, (LPWCH) &gameName, sizeof(gameName));
		
#endif

		extern void LoadAchievements();

		LoadAchievements();
	
	}

}


void ExitInstance()
{
	
	EnterCriticalSection (&d_lock);

#ifndef NO_TRACE
	
	TRACE( "Shutting down");

	if (logfile)
	{
		
		fflush (logfile);
		fclose (logfile);
		
		logfile = NULL;
	
	}

#endif

	LeaveCriticalSection (&d_lock);
	DeleteCriticalSection (&d_lock);

	extern void SaveAchievements();

	SaveAchievements();

}

//=============================================================================
// Entry Point
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	
	switch (ul_reason_for_call)
	{
	
		case DLL_PROCESS_ATTACH:
			hThis = hModule;	

			Detour();
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			ExitInstance();
			break;
	}
	
	return TRUE;

}