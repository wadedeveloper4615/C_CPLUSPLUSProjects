/*
 * CPhysicalDrive.cpp
 *
 *  Created on: Nov 12, 2020
 *      Author: wade4
 */
#include "NTFS.h"
#include "CNTFSVolume.h"
#include "CPhysicalDrive.h"

CPhysicalDrive::CPhysicalDrive(TCHAR *VolumeName) {
    this->VolumeName = VolumeName;
    VolumeHandle = NULL;
    open(VolumeName);
    readMasterBootRecord();
    printMasterBootRecord();
    for (int i = 0; i <= 3; i++) {
        PartitionEntryPtr entry = &mbr.partionTable[i];
        if (entry->fileSystem == 0x07) {
            partition[i] = new CNTFSVolume(this, i);
        } else {
            partition[i] = NULL;
        }
    }
}

CPhysicalDrive::~CPhysicalDrive() {
}

CNTFSVolume** CPhysicalDrive::getPartitions() {
    return (CNTFSVolume**) &partition;
}

void CPhysicalDrive::open(TCHAR *VolumeName) {
    DWORD read;

    VolumeHandle = CreateFile(VolumeName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    if (VolumeHandle == INVALID_HANDLE_VALUE) {
        exitWithLastError(_T("Failed opening the volume '%s'\n"), VolumeName);
    }
}

void CPhysicalDrive::readMasterBootRecord() {
    DWORD amount;
    read(&mbr, sizeof(MasterBootRecord), &amount);
    if (amount != sizeof(MasterBootRecord)) {
        exitWithLastError(_T("read in %ld instead of %d in dumpDrive (%lx)\n"), amount, sizeof(MasterBootRecord), GetLastError());
    } else {
        _tprintf(_T("Read %d bytes of the Master Boot Record...\n"), amount);
    }
}

MasterBootRecordPtr CPhysicalDrive::getMasterBootRecord() {
    return &mbr;
}

void CPhysicalDrive::close() {
    CloseHandle(VolumeHandle);
}

TCHAR* CPhysicalDrive::getName() {
    return VolumeName;
}

LONGLONG CPhysicalDrive::seek(LONGLONG distance, DWORD MoveMethod) {
    LARGE_INTEGER li;
    li.QuadPart = distance;
    li.LowPart = SetFilePointer(VolumeHandle, li.LowPart, &li.HighPart, MoveMethod);
    if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        li.QuadPart = -1;
    }

    return li.QuadPart;
}

WINBOOL CPhysicalDrive::read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead) {
    return (WINBOOL) ReadFile(VolumeHandle, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, (LPOVERLAPPED) NULL);
}

void CPhysicalDrive::printMasterBootRecordPartition(PartitionEntryPtr entry, int i) {
    if (entry->fileSystem != 0x00) {
        if (entry->bootType == 0x80) {
            _tprintf(_T("Boot type:\t\t\t 0x%X (bootable)\n"), entry->bootType & 0xFF);
        } else {
            _tprintf(_T("Boot type:\t\t\t 0x%X (not bootable)\n"), entry->bootType & 0xFF);
        }
        _tprintf(_T("Begin Head:\t\t\t %d\n"), entry->beginHead & 0xFF);
        int bsector = HI(entry->beginSectorCylinder) | ((LO(entry->beginSectorCylinder) & 0xC0) >> 2);
        int bcylinder = entry->beginSectorCylinder & 0x3F;
        _tprintf(_T("Begin sector :\t\t\t %u\n"), bsector);
        _tprintf(_T("Begin cylinder :\t\t %u\n"), bcylinder);
        if (entry->fileSystem == 0x07 && entry->bootType == 0x80) {
            _tprintf(_T("File System:\t\t\t 0x%02X (Win10 System Boot using NTFS)\n"), entry->fileSystem & 0xFF);
        } else if (entry->fileSystem == 0x07 && entry->bootType == 0x00) {
            _tprintf(_T("File System:\t\t\t 0x%02X (Win10 Data Drive using NTFS)\n"), entry->fileSystem & 0xFF);
        } else if (entry->fileSystem == 0x0C) {
            _tprintf(_T("File System:\t\t\t 0x%02X (FAT32 with LBA)\n"), entry->fileSystem & 0xFF);
        } else if (entry->fileSystem == 0x82) {
            _tprintf(_T("File System:\t\t\t 0x%02X (Linux Swap)\n"), entry->fileSystem & 0xFF);
        } else if (entry->fileSystem == 0x83) {
            _tprintf(_T("File System:\t\t\t 0x%02X (Linux Drive)\n"), entry->fileSystem & 0xFF);
        } else if (entry->fileSystem == 0x02) {
            _tprintf(_T("File System:\t\t\t 0x%02X (Linux Root)\n"), entry->fileSystem & 0xFF);
        } else if (entry->fileSystem == 0x0F) {
            _tprintf(_T("File System:\t\t\t 0x%02X (Extended with LBA)\n"), entry->fileSystem & 0xFF);
        } else if (entry->fileSystem == 0x05) {
            _tprintf(_T("File System:\t\t\t 0x%02X (Extended with CHS)\n"), entry->fileSystem & 0xFF);
        } else if (entry->fileSystem == 0x27) {
            _tprintf(_T("File System:\t\t\t 0x%02X (Win10 Recovery Partiton)\n"), entry->fileSystem & 0xFF);
        } else {
            _tprintf(_T("File System:\t\t\t 0x%02X (Unknown)\n"), entry->fileSystem & 0xFF);
        }
        ULONGLONG endSector = (ULONGLONG) entry->lbaStart + (ULONGLONG) entry->partitionSize;
        _tprintf(_T("End Head:\t\t\t %d\n"), entry->endHead & 0xFF);
        int esector = HI(entry->endSectorCylinder) | ((LO(entry->endSectorCylinder) & 0xC0) >> 2);
        int ecylinder = entry->endSectorCylinder & 0x3F;
        _tprintf(_T("End sector :\t\t\t %u\n"), esector);
        _tprintf(_T("End cylinder :\t\t\t %u\n"), ecylinder);
        _tprintf(_T("Begin Absolute Sector:\t\t %ld\n"), entry->lbaStart);
        _tprintf(_T("End Absolute Sector:\t\t %lld\n"), endSector);
        _tprintf(_T("Size in Sectors:\t\t %ld\n"), entry->partitionSize);

        ULONGLONG beginByteOffset = (ULONGLONG) entry->lbaStart * 512LL;
        ULONGLONG endByteOffset = beginByteOffset + (ULONGLONG) entry->partitionSize * 512LL;
        ULONGLONG sizeOfPartition = endByteOffset - beginByteOffset;
        _tprintf(_T("Begin Byte offset:\t\t %lld\n"), beginByteOffset);
        _tprintf(_T("End Byte offset:\t\t %lld\n"), endByteOffset);
        double sizeInGB = (sizeOfPartition) / (1024LL * 1024LL);
        if (sizeInGB < 1024.0) {
            _tprintf(_T("Size:\t\t\t\t %lld (%4.2lf MB)\n"), sizeOfPartition, sizeInGB);
        } else {
            _tprintf(_T("Size:\t\t\t\t %lld (%4.2lf GB)\n"), sizeOfPartition, sizeInGB / 1024LL);
        }
    }
    _tprintf(_T("\n"));
}

void CPhysicalDrive::printMasterBootRecord() {
    for (int i = 0; i <= 3; i++) {
        if (mbr.partionTable[i].fileSystem != 0x27) {
            printMasterBootRecordPartition(&mbr.partionTable[i], i);
        }
    }
}

HANDLE CPhysicalDrive::getVolumeHandle() {
    return VolumeHandle;
}

