/*
 * CAttrNonResident.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "NTFS.h"
#include "CNTFSVolume.h"
#include "CFileRecord.h"
#include "CAttrNonResident.h"
#include "CAttrBase.h"

CAttrNonResident::CAttrNonResident(const AttrHeaderCommonPtr ahc, const CFileRecord *fr) : CAttrBase(ahc, fr) {
    AttrHeaderNR = (ATTR_HEADER_NON_RESIDENT*) ahc;

    UnalignedBuf = new BYTE[getClusterSize()];

    bDataRunOK = ParseDataRun();
}

CAttrNonResident::~CAttrNonResident() {
    delete UnalignedBuf;
    DataRunList.RemoveAll();
}

// Parse a single DataRun unit
BOOL CAttrNonResident::PickData(const BYTE **dataRun, LONGLONG *length, LONGLONG *LCNOffset) {
    BYTE size = **dataRun;
    (*dataRun)++;
    int lengthBytes = size & 0x0F;
    int offsetBytes = size >> 4;

    if (lengthBytes > 8 || offsetBytes > 8) {
        printf("DataRun decode error 1: 0x%02X\n", size);
        return FALSE;
    }

    *length = 0;
    memcpy(length, *dataRun, lengthBytes);
    if (*length < 0) {
        printf("DataRun length error: %I64d\n", *length);
        return FALSE;
    }

    (*dataRun) += lengthBytes;
    *LCNOffset = 0;
    if (offsetBytes)    // Not Sparse File
    {
        if ((*dataRun)[offsetBytes - 1] & 0x80)
            *LCNOffset = -1;
        memcpy(LCNOffset, *dataRun, offsetBytes);

        (*dataRun) += offsetBytes;
    }

    return TRUE;
}

// Travers DataRun and insert into a link list
BOOL CAttrNonResident::ParseDataRun() {
    printf("Parsing Non Resident DataRun\n");
    printf("Start VCN = %I64u, End VCN = %I64u\n", AttrHeaderNR->StartVCN, AttrHeaderNR->LastVCN);

    const BYTE *dataRun = (BYTE*) AttrHeaderNR + AttrHeaderNR->DataRunOffset;
    LONGLONG length;
    LONGLONG LCNOffset;
    LONGLONG LCN = 0;
    ULONGLONG VCN = 0;

    while (*dataRun) {
        if (PickData(&dataRun, &length, &LCNOffset)) {
            LCN += LCNOffset;
            if (LCN < 0) {
                printf("DataRun decode error 2\n");
                return FALSE;
            }

            printf("Data length = %I64d clusters, LCN = %I64d", length, LCN);
            printf(LCNOffset == 0 ? ", Sparse Data\n" : "\n");

            // Store LCN, Data size (clusters) into list
            DataRun_Entry *dr = new DataRun_Entry;
            dr->LCN = (LCNOffset == 0) ? -1 : LCN;
            dr->Clusters = length;
            dr->StartVCN = VCN;
            VCN += length;
            dr->LastVCN = VCN - 1;

            if (dr->LastVCN <= (AttrHeaderNR->LastVCN - AttrHeaderNR->StartVCN)) {
                DataRunList.InsertEntry(dr);
            } else {
                printf("DataRun decode error: VCN exceeds bound\n");

                // Remove entries
                DataRunList.RemoveAll();

                return FALSE;
            }
        } else
            break;
    }

    return TRUE;
}

// Read clusters from disk, or sparse data
// *actural = Clusters acturally read
BOOL CAttrNonResident::ReadClusters(void *buf, DWORD clusters, LONGLONG lcn) {
    if (lcn == -1)  // sparse data
            {
        printf("Sparse Data, Fill the buffer with 0\n");

        // Fill the buffer with 0
        memset(buf, 0, clusters * getClusterSize());

        return TRUE;
    }

    LARGE_INTEGER addr;
    DWORD len;

    addr.QuadPart = lcn * getClusterSize();
    len = SetFilePointer(getVolumeHandle(), addr.LowPart, &addr.HighPart, FILE_BEGIN);

    if (len == (DWORD) -1 && GetLastError() != NO_ERROR) {
        printf("Cannot locate cluster with LCN %I64d\n", lcn);
    } else {
        if (ReadFile(getVolumeHandle(), buf, clusters * getClusterSize(), &len, NULL) && len == clusters * getClusterSize()) {
            printf("Successfully read %u clusters from LCN %I64d\n", clusters, lcn);
            return TRUE;
        } else {
            printf("Cannot read cluster with LCN %I64d\n", lcn);
        }
    }

    return FALSE;
}

// Read Data, cluster based
// clusterNo: Begnning cluster Number
// clusters: Clusters to read
// bufv, bufLen: Returned data
// *actural = Number of bytes acturally read
BOOL CAttrNonResident::ReadVirtualClusters(ULONGLONG vcn, DWORD clusters, void *bufv, DWORD bufLen, DWORD *actural) {
    _ASSERT(bufv);
    _ASSERT(clusters);

    *actural = 0;
    BYTE *buf = (BYTE*) bufv;

    // Verify if clusters exceeds DataRun bounds
    if (vcn + clusters > (AttrHeaderNR->LastVCN - AttrHeaderNR->StartVCN + 1)) {
        printf("Cluster exceeds DataRun bounds\n");
        return FALSE;
    }

    // Verify buffer size
    if (bufLen < clusters * getClusterSize()) {
        printf("Buffer size too small\n");
        return FALSE;
    }

    // Traverse the DataRun List to find the according LCN
    const DataRun_Entry *dr = DataRunList.FindFirstEntry();
    while (dr) {
        if (vcn >= dr->StartVCN && vcn <= dr->LastVCN) {
            DWORD clustersToRead;

            ULONGLONG vcns = dr->LastVCN - vcn + 1; // Clusters from read pointer to the end

            if ((ULONGLONG) clusters > vcns) // Fragmented data, we must go on
                clustersToRead = (DWORD) vcns;
            else
                clustersToRead = clusters;
            if (ReadClusters(buf, clustersToRead, dr->LCN + (vcn - dr->StartVCN))) {
                buf += clustersToRead * getClusterSize();
                clusters -= clustersToRead;
                *actural += clustersToRead;
                vcn += clustersToRead;
            } else
                break;

            if (clusters == 0)
                break;
        }

        dr = DataRunList.FindNextEntry();
    }

    *actural *= getClusterSize();
    return TRUE;
}

// Judge if the DataRun is successfully parsed
__inline BOOL CAttrNonResident::IsDataRunOK() const {
    return bDataRunOK;
}

// Return Actural Data Size
// *allocSize = Allocated Size
__inline ULONGLONG CAttrNonResident::GetDataSize(ULONGLONG *allocSize) const {
    if (allocSize)
        *allocSize = AttrHeaderNR->AllocSize;

    return AttrHeaderNR->RealSize;
}

// Read "bufLen" bytes from "offset" into "bufv"
// Number of bytes acturally read is returned in "*actural"
BOOL CAttrNonResident::ReadData(const ULONGLONG &offset, void *bufv, DWORD bufLen, DWORD *actural) const {
    // Hard disks can only be accessed by sectors
    // To be simple and efficient, only implemented cluster based accessing
    // So cluster unaligned data address should be processed carefully here

    _ASSERT(bufv);

    *actural = 0;
    if (bufLen == 0)
        return TRUE;

    // Bounds check
    if (offset > AttrHeaderNR->RealSize)
        return FALSE;
    if ((offset + bufLen) > AttrHeaderNR->RealSize)
        bufLen = (DWORD) (AttrHeaderNR->RealSize - offset);

    DWORD len;
    BYTE *buf = (BYTE*) bufv;

    // First cluster Number
    ULONGLONG startVCN = offset / getClusterSize();
    // Bytes in first cluster
    DWORD startBytes = getClusterSize() - (DWORD) (offset % getClusterSize());
    // Read first cluster
    if (startBytes != getClusterSize()) {
        // First cluster, Unaligned
        if (((CAttrNonResident*) this)->ReadVirtualClusters(startVCN, 1, UnalignedBuf, getClusterSize(), &len) && len == getClusterSize()) {
            len = (startBytes < bufLen) ? startBytes : bufLen;
            memcpy(buf, UnalignedBuf + getClusterSize() - startBytes, len);
            buf += len;
            bufLen -= len;
            *actural += len;
            startVCN++;
        } else
            return FALSE;
    }
    if (bufLen == 0)
        return TRUE;

    DWORD alignedClusters = bufLen / getClusterSize();
    if (alignedClusters) {
        // Aligned clusters
        DWORD alignedSize = alignedClusters * getClusterSize();
        if (((CAttrNonResident*) this)->ReadVirtualClusters(startVCN, alignedClusters, buf, alignedSize, &len) && len == alignedSize) {
            startVCN += alignedClusters;
            buf += alignedSize;
            bufLen %= getClusterSize();
            *actural += len;

            if (bufLen == 0)
                return TRUE;
        } else
            return FALSE;
    }

    // Last cluster, Unaligned
    if (((CAttrNonResident*) this)->ReadVirtualClusters(startVCN, 1, UnalignedBuf, getClusterSize(), &len) && len == getClusterSize()) {
        memcpy(buf, UnalignedBuf, bufLen);
        *actural += bufLen;

        return TRUE;
    } else
        return FALSE;
}
