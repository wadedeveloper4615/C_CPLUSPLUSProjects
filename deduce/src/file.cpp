//**********************************************************
//** Deduce copyright (c) 2020 Christopher D. Wade        **
//** File: file.cpp                                      **
//**********************************************************
#include "deduce.h"

VolumeIO::VolumeIO() {
    VolumeName = NULL;
    VolumeHandle = NULL;
}

VolumeIO::~VolumeIO() {
}

void VolumeIO::open(TCHAR *VolumeName) {
    DWORD read;

    VolumeHandle = CreateFile(VolumeName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    if (VolumeHandle == INVALID_HANDLE_VALUE) {
        exitWithLastError(_T("Failed opening the volume '%s'\n"), VolumeName);
    }
}

void VolumeIO::close() {
    CloseHandle(VolumeHandle);
}

LONGLONG VolumeIO::seek(LONGLONG distance, DWORD MoveMethod) {
    LARGE_INTEGER li;
    li.QuadPart = distance;
    li.LowPart = SetFilePointer(VolumeHandle, li.LowPart, &li.HighPart, MoveMethod);
    if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        li.QuadPart = -1;
    }

    return li.QuadPart;
}

WINBOOL VolumeIO::read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead) {
    return (WINBOOL) ReadFile(VolumeHandle, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, (LPOVERLAPPED) NULL);
}

