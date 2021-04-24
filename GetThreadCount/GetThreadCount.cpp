// GetThreadCount.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>

//  Forward declarations:
LONG ListProcessThreads(DWORD dwOwnerPID);
void printError(TCHAR* msg);

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Must give process ID as first argument.";
        return 0;
    }
    int processID = atoi(argv[1]);
    LONG threadCount = ListProcessThreads(static_cast<DWORD>(processID)); //GetCurrentProcessId()
    return threadCount;
}

LONG ListProcessThreads(DWORD dwOwnerPID)
{
    HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
    THREADENTRY32 te32;

    // Take a snapshot of all running threads  
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE)
        return(FALSE);

    // Fill in the size of the structure before using it. 
    te32.dwSize = sizeof(THREADENTRY32);

    // Retrieve information about the first thread,
    // and exit if unsuccessful
    if (!Thread32First(hThreadSnap, &te32))
    {
        //printError(TEXT(const "Thread32First"));  // Show cause of failure //This line was causing an error that I didn't feel like TS'ing
        CloseHandle(hThreadSnap);     // Must clean up the snapshot object!
        return(FALSE);
    }

    // Now walk the thread list of the system,
    // and display information about each thread
    // associated with the specified process
    LONG count = 0;
    do
    {
        if (te32.th32OwnerProcessID == dwOwnerPID)
        {
            /*
            _tprintf(TEXT("\n     THREAD ID      = 0x%08X"), te32.th32ThreadID);
            _tprintf(TEXT("\n     base priority  = %d"), te32.tpBasePri);
            _tprintf(TEXT("\n     delta priority = %d"), te32.tpDeltaPri);
            */
            count++;
        }
    } while (Thread32Next(hThreadSnap, &te32));

    /*
    _tprintf(TEXT("\nThread Count: %d"), count);
    _tprintf(TEXT("\n"));
    */

    //  Don't forget to clean up the snapshot object.
    CloseHandle(hThreadSnap);
    return(count);
}

void printError(TCHAR* msg)
{
    DWORD eNum;
    TCHAR sysMsg[256];
    TCHAR* p;

    eNum = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, eNum,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        sysMsg, 256, NULL);

    // Trim the end of the line and terminate it with a null
    p = sysMsg;
    while ((*p > 31) || (*p == 9))
        ++p;
    do { *p-- = 0; } while ((p >= sysMsg) &&
        ((*p == '.') || (*p < 33)));

    // Display the message
    _tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);
}