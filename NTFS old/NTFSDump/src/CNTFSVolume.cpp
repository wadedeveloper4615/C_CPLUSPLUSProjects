/*
 * CNTFSVolume.cpp
 *
 *  Created on: Nov 5, 2020
 *      Author: wade4
 */
#include "NTFS.h"
#include "CFileRecord.h"
#include "CPhysicalDrive.h"
#include "CAttrVolInfo.h"
#include "CNTFSVolume.h"

CNTFSVolume::CNTFSVolume(CPhysicalDrive *drive, int i) {
    _tprintf(_T("\nInitializing NTFS Drive %d\n"), i);
    this->drive = drive;
    volumeOK = FALSE;
    if (!OpenVolume(drive, i)) {
        return;
    }
    fileRecord = new CFileRecord(this);
    fileRecord->SetAttrMask(MASK_VOLUME_NAME | MASK_VOLUME_INFORMATION);
    fileRecord->ParseFileRecord(MFT_IDX_VOLUME);
    fileRecord->ParseAttrs();
    CAttr_VolInfo *vi = (CAttr_VolInfo*) fileRecord->FindFirstAttr(ATTR_TYPE_VOLUME_INFORMATION);
    WORD Version = vi->GetVersion();
    printf("NTFS volume version: %u.%u\n", HIBYTE(Version), LOBYTE(Version));
    MFTRecord = new CFileRecord(this);
    MFTRecord->SetAttrMask(MASK_DATA);
    MFTRecord->ParseFileRecord(MFT_IDX_MFT);
    MFTRecord->ParseAttrs();
    MFTData = MFTRecord->FindFirstAttr(ATTR_TYPE_DATA);
}

CNTFSVolume::~CNTFSVolume() {
}

BOOL CNTFSVolume::OpenVolume(CPhysicalDrive *drive, int i) {
    DWORD amountRead;

    partitionOffset = drive->getMasterBootRecord()->partionTable[i].lbaStart * 512LL;
    drive->seek(partitionOffset, FILE_BEGIN);
    drive->read(&bootSector, sizeof(bootSector), &amountRead);
    if (amountRead != sizeof(bootSector)) {
        exitWithLastError(_T("Failed to read boot sector (%lx)\n"), GetLastError());
        return FALSE;
    }

    if (strncmp((const char*) bootSector.Signature, NTFS_SIGNATURE, 8) != 0) {
        exitWithLastError(_T("Wrong file system for volume %s (%lx)\n"), drive->getName(), GetLastError());
        return FALSE;
    }

    sectorSize = bootSector.BytesPerSector;

    clusterSize = sectorSize * bootSector.SectorsPerCluster;

    int sz = (char) bootSector.ClustersPerFileRecord;
    if (sz > 0)
        fileRecordSize = clusterSize * sz;
    else
        fileRecordSize = 1 << (-sz);

    sz = (char) bootSector.ClustersPerIndexBlock;
    if (sz > 0)
        indexBlockSize = clusterSize * sz;
    else
        indexBlockSize = 1 << (-sz);
    MFTAddr = bootSector.LCN_MFT * clusterSize;
    _tprintf(_T("Sector Size = %u bytes\n"), sectorSize);
    _tprintf(_T("Cluster Size = %u bytes\n"), clusterSize);
    _tprintf(_T("FileRecord Size = %u bytes\n"), fileRecordSize);
    _tprintf(_T("IndexBlock Size = %u bytes\n"), indexBlockSize);
    _tprintf(_T("MFT address = 0x%016I64X\n"), MFTAddr);

    return TRUE;
}

BOOL CNTFSVolume::IsVolumeOK() const {
    return volumeOK;
}

WORD CNTFSVolume::getSectorSize() const {
    return sectorSize;
}

DWORD CNTFSVolume::getClusterSize() const {
    return clusterSize;
}

DWORD CNTFSVolume::getFileRecordSize() const {
    return fileRecordSize;
}

DWORD CNTFSVolume::getIndexBlockSize() const {
    return indexBlockSize;
}

ULONGLONG CNTFSVolume::getMFTAddr() const {
    return MFTAddr;
}

CPhysicalDrive* CNTFSVolume::getDrive() const {
    return drive;
}

ULONGLONG CNTFSVolume::getPartitionOffset() const {
    return partitionOffset;
}

NTFSBootSector CNTFSVolume::getBootSector() const {
    return bootSector;
}

CFileRecord* CNTFSVolume::getFileRecord() const {
    return fileRecord;
}

CFileRecord* CNTFSVolume::getMFTRecord() const {
    return MFTRecord;
}

const CAttrBase* CNTFSVolume::getMFTData() const {
    return MFTData;
}

