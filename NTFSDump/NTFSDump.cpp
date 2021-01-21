//============================================================================
// Name        : NTFSDump.cpp
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : NTFS file system dump, that bypasses the windows driver
//============================================================================
#include "NTFS.h"
#include "CPhysicalDrive.h"

int main() {
    TCHAR *VolumeName = _T("\\\\.\\PhysicalDrive0");
    CPhysicalDrive drive(VolumeName);
    drive.close();
    printf("Done\n");
    return 0;
}
