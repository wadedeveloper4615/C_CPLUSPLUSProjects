/*
 * CNTFSVolume.cpp
 *
 *  Created on: Nov 3, 2020
 *      Author: wade4
 */
#include "NTFS_Common.h"
#include "CFileRecord.h"
#include "CNTFSVolume.h"

CNTFSVolume::CNTFSVolume(_TCHAR volume) {
    VolumeHandle = INVALID_HANDLE_VALUE;
    VolumeOK = FALSE;
    Version = 0;
    ClearAttrRawCB();

    if (!OpenVolume(volume))
        return;

    CFileRecord vol(this);
    vol.SetAttrMask(MASK_VOLUME_NAME | MASK_VOLUME_INFORMATION);
    if (!vol.ParseFileRecord(MFT_IDX_VOLUME))
        return;

    vol.ParseAttrs();
}

CNTFSVolume::~CNTFSVolume() {
    if (VolumeHandle != INVALID_HANDLE_VALUE)
        CloseHandle(VolumeHandle);
    printf("Volume is closed\n");
}

BOOL CNTFSVolume::OpenVolume(_TCHAR volume) {
    // Verify parameter
    if (!_istalpha(volume)) {
        printf("Volume name error, should be like 'C', 'D'\n");
        return FALSE;
    }

    _TCHAR VolumeName[7];
    _sntprintf(VolumeName, 6, _T("\\\\.\\%c:"), volume);
    VolumeName[6] = _T('\0');

    VolumeHandle = CreateFile(VolumeName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    if (VolumeHandle == INVALID_HANDLE_VALUE) {
        exitWithLastError(_T("Failed opening the volume '%s'\n"), VolumeName);
    }
    printf("Volume '%s' is Open\n", VolumeName);
    DWORD bytesRead;
    NTFSBootSector bootSector;
    ReadFile(VolumeHandle, &bootSector, 512, &bytesRead, NULL);
    if (bytesRead != 512) {
        exitWithLastError(_T("Failed read the volume '%s' bootsector\n"), VolumeName);
    }
    if (strncmp((const char*) bootSector.Signature, NTFS_SIGNATURE, 8) == 0) {
        SectorSize = bootSector.BytesPerSector;
        printf("Sector Size = %u bytes\n", SectorSize);

        ClusterSize = SectorSize * bootSector.SectorsPerCluster;
        printf("Cluster Size = %u bytes\n", ClusterSize);

        int sz = (char) bootSector.ClustersPerFileRecord;
        if (sz > 0)
            FileRecordSize = ClusterSize * sz;
        else
            FileRecordSize = 1 << (-sz);
        printf("FileRecord Size = %u bytes\n", FileRecordSize);

        sz = (char) bootSector.ClustersPerIndexBlock;
        if (sz > 0)
            IndexBlockSize = ClusterSize * sz;
        else
            IndexBlockSize = 1 << (-sz);
        printf("IndexBlock Size = %u bytes\n", IndexBlockSize);

        MFTAddr = bootSector.LCN_MFT * ClusterSize;
        printf("MFT address = 0x%016I64X\n", MFTAddr);
        return TRUE;
    } else {
        exitWithLastError(_T("Volume '%s' is NOT NTFS\n"), VolumeName);
        return FALSE;
    }
}

BOOL CNTFSVolume::IsVolumeOK() const {
    return VolumeOK;
}

WORD CNTFSVolume::GetVersion() const {
    return Version;
}

DWORD CNTFSVolume::GetSectorSize() const {
    return SectorSize;
}

DWORD CNTFSVolume::GetClusterSize() const {
    return ClusterSize;
}

DWORD CNTFSVolume::GetFileRecordSize() const {
    return FileRecordSize;
}

DWORD CNTFSVolume::GetIndexBlockSize() const {
    return IndexBlockSize;
}

// Get MFT starting address
ULONGLONG CNTFSVolume::GetMFTAddr() const {
    return MFTAddr;
}

// Install Attribute CallBack routines for the whole Volume
BOOL CNTFSVolume::InstallAttrRawCB(DWORD attrType, ATTR_RAW_CALLBACK cb) {
    DWORD atIdx = ATTR_INDEX(attrType);
    if (atIdx < ATTR_NUMS) {
        AttrRawCallBack[atIdx] = cb;
        return TRUE;
    } else
        return FALSE;
}

// Clear all Attribute CallBack routines
void CNTFSVolume::ClearAttrRawCB() {
    for (int i = 0; i < ATTR_NUMS; i++)
        AttrRawCallBack[i] = NULL;
}
