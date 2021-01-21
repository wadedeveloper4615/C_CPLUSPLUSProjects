/*
 * Error.cpp
 *
 *  Created on: Nov 17, 2020
 *      Author: wade4
 */
#include "NTFS.h"

void exitWithLastError(TCHAR *format, ...) {
    LPVOID lpMsgBuf;
    DWORD lastError = GetLastError();

    va_list args;
    va_start(args, format);

    if (lastError) {
        FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
        _ftprintf(stderr, _T("\n")); // @suppress("Invalid arguments")
        _vftprintf(stderr, format, args);
        _ftprintf(stderr, _T("GetLastError()=%d: %s\n"), lastError, lpMsgBuf); // @suppress("Invalid arguments")
    } else {
        _ftprintf(stderr, _T("\n"));
        _vftprintf(stderr, format, args);
    }
    va_end(args);
    LocalFree(lpMsgBuf);
    exit(lastError);
}

