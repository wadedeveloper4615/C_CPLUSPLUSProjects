/*
 * CNTFSVolume.cpp
 *
 *  Created on: Nov 5, 2020
 *      Author: wade4
 */
#include "NTFS_Attribute.h"
#include "CAttrVolInfo.h"
#include "CNTFSVolume.h"

CNTFSVolume::CNTFSVolume(_TCHAR volume) {
    VolumeHandle = INVALID_HANDLE_VALUE;
    VolumeOK = FALSE;
    MFTRecord = NULL;
    MFTData = NULL;
    Version = 0;
    ClearAttrRawCB();
    if (!OpenVolume(volume)) {
        return;
    }

    // Verify NTFS volume version (must >= 3.0)
    CFileRecord vol(this);
    vol.SetAttrMask(MASK_VOLUME_NAME | MASK_VOLUME_INFORMATION);
    if (!vol.ParseFileRecord(MFT_IDX_VOLUME)) {
        return;
    }

    vol.ParseAttrs();
    CAttr_VolInfo *vi = (CAttr_VolInfo*) vol.FindFirstAttr(ATTR_TYPE_VOLUME_INFORMATION);
    if (!vi) {
        return;
    }

    Version = vi->GetVersion();
    printf("NTFS volume version: %u.%u\n", HIBYTE(Version), LOBYTE(Version));
    if (Version < 0x0300)   // NT4 ?
        return;

#ifdef  _DEBUG
    CAttr_VolName *vn = (CAttr_VolName*)vol.FindFirstAttr(ATTR_TYPE_VOLUME_NAME);
    if (vn)
    {
        char volname[MAX_PATH];
        if (vn->GetName(volname, MAX_PATH) > 0)
        {
            NTFS_TRACE1("NTFS volume name: %s\n", volname);
        }
    }
#endif

    VolumeOK = TRUE;

    MFTRecord = new CFileRecord(this);
    MFTRecord->SetAttrMask(MASK_DATA);
    if (MFTRecord->ParseFileRecord(MFT_IDX_MFT)) {
        MFTRecord->ParseAttrs();
        MFTData = MFTRecord->FindFirstAttr(ATTR_TYPE_DATA);
        if (MFTData == NULL) {
            delete MFTRecord;
            MFTRecord = NULL;
        }
    }
}

CNTFSVolume::~CNTFSVolume() {
    if (VolumeHandle != INVALID_HANDLE_VALUE)
        CloseHandle(VolumeHandle);
    if (MFTRecord)
        delete MFTRecord;
}

BOOL CNTFSVolume::OpenVolume(_TCHAR volume) {
    // Verify parameter
    if (!_istalpha(volume)) {
        printf("Volume name error, should be like 'C', 'D'\n");
        return FALSE;
    }
    _TCHAR volumePath[7];
    _sntprintf(volumePath, 6, _T("\\\\.\\%c:"), volume);
    volumePath[6] = _T('\0');

    VolumeHandle = CreateFile(volumePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    if (VolumeHandle == INVALID_HANDLE_VALUE) {
        printf("Cannnot open volume %c\n", (char) volume);
        if (VolumeHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(VolumeHandle);
            VolumeHandle = INVALID_HANDLE_VALUE;
        }
        return FALSE;
    }
    DWORD num;
    NTFS_BPB bpb;
    BOOL bRet = ReadFile(VolumeHandle, &bpb, 512, &num, NULL);
    if (!bRet || num != 512) {
        printf("Cannnot load boot sector for volume %c\n", (char) volume);
        if (VolumeHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(VolumeHandle);
            VolumeHandle = INVALID_HANDLE_VALUE;
        }
        return FALSE;
    }
    if (strncmp((const char*) bpb.Signature, NTFS_SIGNATURE, 8) != 0) {
        printf("Wrong file system for volume %c\n", (char) volume);
        if (VolumeHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(VolumeHandle);
            VolumeHandle = INVALID_HANDLE_VALUE;
        }
        return FALSE;
    }

    SectorSize = bpb.BytesPerSector;
    printf("Sector Size = %u bytes\n", SectorSize);

    ClusterSize = SectorSize * bpb.SectorsPerCluster;
    printf("Cluster Size = %u bytes\n", ClusterSize);

    int sz = (char) bpb.ClustersPerFileRecord;
    if (sz > 0)
        FileRecordSize = ClusterSize * sz;
    else
        FileRecordSize = 1 << (-sz);
    printf("FileRecord Size = %u bytes\n", FileRecordSize);

    sz = (char) bpb.ClustersPerIndexBlock;
    if (sz > 0)
        IndexBlockSize = ClusterSize * sz;
    else
        IndexBlockSize = 1 << (-sz);
    printf("IndexBlock Size = %u bytes\n", IndexBlockSize);

    MFTAddr = bpb.LCN_MFT * ClusterSize;
    printf("MFT address = 0x%016I64X\n", MFTAddr);

    return TRUE;
}

BOOL CNTFSVolume::IsVolumeOK() const {
    return VolumeOK;
}

WORD CNTFSVolume::GetVersion() const {
    return Version;
}

ULONGLONG CNTFSVolume::GetRecordsCount() const {
    return (MFTData->GetDataSize() / FileRecordSize);
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

ULONGLONG CNTFSVolume::GetMFTAddr() const {
    return MFTAddr;
}

BOOL CNTFSVolume::InstallAttrRawCB(DWORD attrType, ATTR_RAW_CALLBACK cb) {
    DWORD atIdx = ATTR_INDEX(attrType);
    if (atIdx < ATTR_NUMS) {
        AttrRawCallBack[atIdx] = cb;
        return TRUE;
    } else
        return FALSE;
}

void CNTFSVolume::ClearAttrRawCB() {
    for (int i = 0; i < ATTR_NUMS; i++)
        AttrRawCallBack[i] = NULL;
}
