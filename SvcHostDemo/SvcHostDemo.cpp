#define _UNICODE

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

SERVICE_STATUS_HANDLE g_serviceStatusHandle;
HANDLE g_hSvcStopEvent = NULL;

SERVICE_STATUS g_serviceStatus = {SERVICE_WIN32_SHARE_PROCESS, SERVICE_START_PENDING, SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE};

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    OutputDebugString(L"DllMain");
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
                    OutputDebugString(L"DLL_PROCESS_ATTACH");
            break;
        case DLL_THREAD_ATTACH:
                    OutputDebugString(L"DLL_THREAD_ATTACH");
            break;
        case DLL_THREAD_DETACH:
                    OutputDebugString(L"DLL_THREAD_DETACH");
            break;
        case DLL_PROCESS_DETACH:
                    OutputDebugString(L"DLL_PROCESS_DETACH");
            break;
    }
    return TRUE;
}

DWORD WINAPI HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
    switch (dwControl)
    {
        case SERVICE_CONTROL_STOP:
            OutputDebugString(L"HandlerEx SERVICE_CONTROL_STOP");
            g_serviceStatus.dwCurrentState = SERVICE_STOPPED;
            SetEvent(g_hSvcStopEvent);
            break;
        case SERVICE_CONTROL_SHUTDOWN:
            OutputDebugString(L"HandlerEx SERVICE_CONTROL_SHUTDOWN");
            g_serviceStatus.dwCurrentState = SERVICE_STOPPED;
            SetEvent(g_hSvcStopEvent);
            break;
        case SERVICE_CONTROL_PAUSE:
            OutputDebugString(L"HandlerEx SERVICE_CONTROL_PAUSE");
            g_serviceStatus.dwCurrentState = SERVICE_PAUSED;
            break;
        case SERVICE_CONTROL_CONTINUE:
            OutputDebugString(L"HandlerEx SERVICE_CONTROL_CONTINUE");
            g_serviceStatus.dwCurrentState = SERVICE_RUNNING;
            break;
        case SERVICE_CONTROL_INTERROGATE:
            OutputDebugString(L"HandlerEx SERVICE_CONTROL_INTERROGATE");
            break;
        default:
            OutputDebugString(L"HandlerEx default");
            break;
    }

    SetServiceStatus(g_serviceStatusHandle, &g_serviceStatus);

    return NO_ERROR;
}

DWORD WINAPI Worker(LPVOID lpParam)
{
    while (TRUE)
    {
        if (g_serviceStatus.dwCurrentState == SERVICE_RUNNING)
            OutputDebugString(L"Working ...");
        Sleep(1000);
    }
}

extern "C" VOID WINAPI ServiceMain(DWORD dwArgc, LPCWSTR* lpszArgv)
{
    TCHAR szOutput[256];
    DWORD dwIter;

    OutputDebugString(L"Begin ServiceMain");
    StringCchPrintf(szOutput, 256, L"dwArgc = %d", dwArgc);
    OutputDebugString(szOutput);
    for (dwIter=0; dwIter<dwArgc; dwIter++)
        OutputDebugString(lpszArgv[dwIter]);

    if (dwArgc > 0)
        g_serviceStatusHandle = RegisterServiceCtrlHandlerExW(lpszArgv[0], HandlerEx, nullptr);
    else
        g_serviceStatusHandle = RegisterServiceCtrlHandlerExW(L"SvcHostDemo", HandlerEx, nullptr);
    if (!g_serviceStatusHandle)
    {
        OutputDebugString(L"Error 1 ServiceMain");
        return;
    }

    g_hSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (g_hSvcStopEvent == NULL)
    {
        OutputDebugString(L"Error 2 ServiceMain");
        return;
    }

    g_serviceStatus.dwCurrentState = SERVICE_RUNNING;

    SetServiceStatus(g_serviceStatusHandle, &g_serviceStatus);

    OutputDebugString(L"Starting Worker ServiceMain");

    if (NULL == CreateThread(NULL, 0, Worker, NULL, 0, NULL))
    {
        OutputDebugString(L"Error 3 ServiceMain");
        return;
    }

    OutputDebugString(L"Waiting ServiceMain");

    WaitForSingleObject(g_hSvcStopEvent, INFINITE);

    OutputDebugString(L"End ServiceMain");
}

/*
*  To create a 32-bit DLL:
*
*      i686-w64-mingw32-gcc -shared -municode -o SvcHostDemo.dll SvcHostDemo.def SvcHostDemo.cpp
*
*  To create a 64-bit DLL:
*
*      x86_64-w64-mingw32-gcc -shared -municode -o SvcHostDemo.dll SvcHostDemo.def SvcHostDemo.cpp
*
*/
