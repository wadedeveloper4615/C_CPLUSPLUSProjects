//**********************************************************
//** Deduce copyright (c) 2020 Christopher D. Wade        **
//** File: list.h                                         **
//**********************************************************
#ifndef FILE_H_
#define FILE_H_

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <locale.h>
#include <tchar.h>
#include <shellapi.h>

class VolumeIO {
        TCHAR *VolumeName;
        HANDLE VolumeHandle;
    public:
        VolumeIO();
        ~VolumeIO();
        void open(TCHAR *VolumeName);
        void close();
        LONGLONG seek(LONGLONG distance, DWORD MoveMethod);
        WINBOOL read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
};

#endif /* FILE_H_ */
