//**********************************************************
//** Deduce copyright (c) 2020 Christopher D. Wade        **
//** File: print.cpp                                      **
//**********************************************************
#include "deduce.h"

/*
 void printMasterBootRecord(TCHAR *text, PartitionEntryPtr entry, int i) {
 if (entry->fileSystem != 0x00) {
 _tprintf(_T("\n%s %d\n"), text, i);
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
 _tprintf(_T("Begin Absolute Sector:\t\t %s\n"), addCommas(entry->lbaStart));
 _tprintf(_T("End Absolute Sector:\t\t %s\n"), addCommas(endSector));
 _tprintf(_T("Size in Sectors:\t\t %s\n"), addCommas(entry->partitionSize));

 ULONGLONG beginByteOffset = (ULONGLONG) entry->lbaStart * 512LL;
 ULONGLONG endByteOffset = beginByteOffset + (ULONGLONG) entry->partitionSize * 512LL;
 ULONGLONG sizeOfPartition = endByteOffset - beginByteOffset;
 _tprintf(_T("Begin Byte offset:\t\t %s\n"), addCommas(beginByteOffset));
 _tprintf(_T("End Byte offset:\t\t %s\n"), addCommas(endByteOffset));
 double sizeInGB = (sizeOfPartition) / (1024LL * 1024LL);
 if (sizeInGB < 1024.0) {
 _tprintf(_T("Size:\t\t\t\t %s (%4.2lf MB)\n"), addCommas(sizeOfPartition), sizeInGB);
 } else {
 _tprintf(_T("Size:\t\t\t\t %s (%4.2lf GB)\n"), addCommas(sizeOfPartition), sizeInGB / 1024LL);
 }
 }
 }
 */
void printNTFSBootSector(NTFSBootSectorPtr bootSector) {
    DWORD sectorSize = bootSector->BytesPerSector;
    DWORD clusterSize = bootSector->BytesPerSector * bootSector->SectorsPerCluster;
    DWORD FileRecordSize;
    DWORD sz = bootSector->ClustersPerFileRecord;
    if (sz > 0)
        FileRecordSize = clusterSize * sz;
    else
        FileRecordSize = 1 << (-sz);
    LONGLONG totalCluster = bootSector->TotalSectors / bootSector->SectorsPerCluster;
    LONGLONG driveSize = bootSector->TotalSectors * bootSector->BytesPerSector;
    sz = bootSector->ClustersPerIndexBlock;
    DWORD IndexBlockSize;
    if (sz > 0)
        IndexBlockSize = clusterSize * sz;
    else
        IndexBlockSize = 1 << (-sz);
    LONGLONG MFTAddr = bootSector->LCN_MFT * clusterSize;
    LONGLONG MFTMirrAddr = bootSector->LCN_MFTMirr * clusterSize;

    _tprintf(_T("OEM ID:\t\t\t\t \"%.8s\"\n"), bootSector->Signature);
    _tprintf(_T("Byte/Sector:\t\t\t %u\n"), bootSector->BytesPerSector);
    _tprintf(_T("Sector/Cluster:\t\t\t %u\n"), bootSector->SectorsPerCluster);
    _tprintf(_T("Media descriptor:\t\t %X\n"), bootSector->MediaDescriptor);
    _tprintf(_T("Sector/Track:\t\t\t %u\n"), bootSector->SectorsPerTrack);
    _tprintf(_T("Head Number:\t\t\t %u\n"), bootSector->NumberOfHeads);
    _tprintf(_T("Hidden Sector:\t\t\t %s\n"), addCommas(bootSector->HiddenSectors));
    _tprintf(_T("Total Sector:\t\t\t %s\n"), addCommas(bootSector->TotalSectors));
    _tprintf(_T("MFT Cluster:\t\t\t %s\n"), addCommas(bootSector->LCN_MFT));
    _tprintf(_T("MFT Mirror Cluster:\t\t %llu\n"), bootSector->LCN_MFTMirr);
    _tprintf(_T("Cluster/Record:\t\t\t %u\n"), bootSector->ClustersPerFileRecord);
    _tprintf(_T("Cluster/Block:\t\t\t %u\n"), bootSector->ClustersPerIndexBlock);
    _tprintf(_T("Serial Number:\t\t\t %llX\n"), bootSector->VolumeSN);
    _tprintf(_T("End Marker:\t\t\t %X\n"), bootSector->endMarker);
    _tprintf(_T("Cluster Size:\t\t\t %s\n"), addCommas(clusterSize));
    _tprintf(_T("File Record Size:\t\t %s\n"), addCommas(FileRecordSize));
    _tprintf(_T("Index Block Size:\t\t %s\n"), addCommas(IndexBlockSize));
    _tprintf(_T("MFT address:     \t\t 0x%016I64X\n"), MFTAddr);
    _tprintf(_T("MFT Mirror Addr :\t\t 0x%016I64X\n"), MFTMirrAddr);
    double sizeInGB = (driveSize) / (1024LL * 1024LL);
    if (sizeInGB < 1024.0) {
        _tprintf(_T("Drive Size:\t\t\t %s (%4.2lf MB)\n"), addCommas(driveSize), sizeInGB);
    } else {
        _tprintf(_T("Drive Size:\t\t\t %s (%4.2lf GB)\n"), addCommas(driveSize), sizeInGB / 1024LL);
    }
}
/*
 void readAndPrintMasterBootSector(VolumeIO volume, ULONGLONG offset, int i) {
 MasterBootRecord bootSector1;
 DWORD read;

 volume.seek(offset, FILE_BEGIN);
 volume.read(&bootSector1, sizeof bootSector1, &read);
 if (read != sizeof bootSector1) {
 exitWithLastError(_T("Failed to read boot sector (%lx)\n"), GetLastError());
 }
 _tprintf(_T("\nBoot sector for partition %d\n"), i);
 handleMasterBootRecord(volume, offset, _T("Extended Partition Master Boot Record"), &bootSector1);
 }

 void readAndPrintNTFSBootSector(VolumeIO volume, ULONGLONG offset, int i) {
 NTFSBootSector bootSector;
 DWORD read;

 volume.seek(offset, FILE_BEGIN);
 volume.read(&bootSector, sizeof bootSector, &read);
 if (read != sizeof bootSector) {
 exitWithLastError(_T("Failed to read boot sector (%lx)\n"), GetLastError());
 }
 _tprintf(_T("\nBoot sector for partition %d\n"), i);
 printNTFSBootSector(&bootSector);
 }

 void handleMasterBootRecord(VolumeIO volume, ULONGLONG start, TCHAR *text, MasterBootRecordPtr mbr) {
 for (int i = 0; i <= 3; i++) {
 if (mbr->partionTable[i].fileSystem != 0x27) {
 printMasterBootRecord(text, &mbr->partionTable[i], i);
 }
 ULONGLONG offset = start + mbr->partionTable[i].lbaStart * 512LL;
 if (mbr->partionTable[i].fileSystem == 0x07) {
 readAndPrintNTFSBootSector(volume, offset, i);
 } else if (mbr->partionTable[i].fileSystem == 0x0C) {
 //readAndPrintFAT32BootSector(volume, offset, i);
 } else if (mbr->partionTable[i].fileSystem == 0x05) {
 //readAndPrintMasterBootSector(volume, offset, i);
 }
 }
 }
 */
void readNTFSBootSector(VolumeIO volume, ULONGLONG offset, int i, NTFSBootSectorPtr bootSector) {
    DWORD read;

    volume.seek(offset, FILE_BEGIN);
    volume.read(bootSector, sizeof(NTFSBootSector), &read);
    if (read != sizeof(NTFSBootSector)) {
        exitWithLastError(_T("Failed to read boot sector (%lx)\n"), GetLastError());
    }
}

void loadBootRecord(VolumeIO volume, ULONGLONG start, MasterBootRecordDataPtr recordData) {
    for (int i = 0; i <= 3; i++) {
        ULONGLONG offset = start + recordData->mbr.partionTable[i].lbaStart * 512LL;
        if (recordData->mbr.partionTable[i].fileSystem == 0x07) {
            _tprintf(_T("Reading Boot Record %d...\n"), i);
            NTFSBootSector bootSector;
            readNTFSBootSector(volume, offset, i, &bootSector);
            recordData->bootSectors.addAtEnd(bootSector);
            printNTFSBootSector(&bootSector);
            _tprintf(_T("\n"));
        }
    }
}
