/*
 * CAttrBitmapAttrNonResident.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "CNTFSVolume.h"
#include "CAttrBase.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CAttrStdInfo.h"
#include "NTFS_Attribute.h"
#include "CFileRecord.h"
#include "CAttrBitmapAttrNonResident.h"

CAttrBitmapAttrNonResident::CAttrBitmapAttrNonResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) : CAttrNonResident(ahc, fr) {
    printf("Attribute: Bitmap (%sResident)\n", IsNonResident() ? "Non" : "");

    CurrentCluster = -1;

    if (IsDataRunOK()) {
        BitmapSize = GetDataSize();

        if (IsNonResident())
            BitmapBuf = new BYTE[_ClusterSize];
        else {
            BitmapBuf = new BYTE[(DWORD) BitmapSize];

            DWORD len;
            if (!(ReadData(0, BitmapBuf, (DWORD) BitmapSize, &len) && len == (DWORD) BitmapSize)) {
                BitmapBuf = NULL;
                printf("Read Resident Bitmap data failed\n");
            } else {
                printf("%u bytes of resident Bitmap data read\n", len);
            }
        }
    } else {
        BitmapSize = 0;
        BitmapBuf = 0;
    }
}

CAttrBitmapAttrNonResident::~CAttrBitmapAttrNonResident() {
    if (BitmapBuf)
        delete BitmapBuf;
}

BOOL CAttrBitmapAttrNonResident::IsClusterFree(ULONGLONG &cluster) {
    if (!IsDataRunOK() || !BitmapBuf)
        return FALSE;

    if (IsNonResident()) {
        LONGLONG idx = (LONGLONG) cluster >> 3;
        DWORD clusterSize = ((CNTFSVolume*) this->FileRecord->Volume)->GetClusterSize();

        LONGLONG clusterOffset = idx / clusterSize;
        cluster -= (clusterOffset * clusterSize * 8);

        // Read one cluster of data if buffer mismatch
        if (CurrentCluster != clusterOffset) {
            DWORD len;
            if (ReadData(clusterOffset, BitmapBuf, clusterSize, &len) && len == clusterSize) {
                CurrentCluster = clusterOffset;
            } else {
                CurrentCluster = -1;
                return FALSE;
            }
        }
    }

    // All the Bitmap data is already in BitmapBuf
    DWORD idx = (DWORD) (cluster >> 3);
    if (IsNonResident() == FALSE) {
        if (idx >= BitmapSize)
            return TRUE;    // Resident data bounds check error
    }

    BYTE fac = (BYTE) (cluster % 8);

    return ((BitmapBuf[idx] & (1 << fac)) == 0);
}
