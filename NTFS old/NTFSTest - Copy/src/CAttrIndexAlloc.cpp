/*
 * CAttrIndexAlloc.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "NTFS_Common.h"
#include "CNTFSVolume.h"
#include "CAttrBase.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CAttrStdInfo.h"
#include "NTFS_Attribute.h"
#include "CFileRecord.h"
#include "CAttrBitmapAttrNonResident.h"
#include "CAttrBitmapAttrResident.h"
#include "CIndexBlock.h"
#include "CIndexBlock.h"
#include "CIndexEntry.h"
#include "CAttrIndexAlloc.h"

CAttr_IndexAlloc::CAttr_IndexAlloc(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) : CAttrNonResident(ahc, fr) {
    NTFS_TRACE("Attribute: Index Allocation\n");

    IndexBlockCount = 0;

    if (IsDataRunOK()) {
        // Get total number of Index Blocks
        ULONGLONG ibTotalSize;
        ibTotalSize = GetDataSize();
        if (ibTotalSize % _IndexBlockSize) {
            NTFS_TRACE2("Cannot calulate number of IndexBlocks, total size = %I64u, unit = %u\n",
                    ibTotalSize, _IndexBlockSize);
            return;
        }
        IndexBlockCount = ibTotalSize / _IndexBlockSize;
    } else {
        NTFS_TRACE("Index Allocation DataRun parse error\n");
    }
}

CAttr_IndexAlloc::~CAttr_IndexAlloc() {
    NTFS_TRACE("CAttr_IndexAlloc deleted\n");
}

// Verify US and update sectors
BOOL CAttr_IndexAlloc::PatchUS(WORD *sector, int sectors, WORD usn, WORD *usarray) {
    int i;

    for (i = 0; i < sectors; i++) {
        sector += ((_SectorSize >> 1) - 1);
        if (*sector != usn)
            return FALSE;       // USN error
        *sector = usarray[i];   // Write back correct data
        sector++;
    }
    return TRUE;
}

ULONGLONG CAttr_IndexAlloc::GetIndexBlockCount() {
    return IndexBlockCount;
}

// Parse a single Index Block
// vcn = Index Block VCN in Index Allocation Data Attributes
// ibClass holds the parsed Index Entries
BOOL CAttr_IndexAlloc::ParseIndexBlock(const ULONGLONG &vcn, CIndexBlock &ibClass) {
    if (vcn >= IndexBlockCount) // Bounds check
        return FALSE;

    // Allocate buffer for a single Index Block
    INDEX_BLOCK *ibBuf = ibClass.AllocIndexBlock(_IndexBlockSize);

    // Sectors Per Index Block
    DWORD sectors = _IndexBlockSize / _SectorSize;

    // Read one Index Block
    DWORD len;
    if (ReadData(vcn * _IndexBlockSize, ibBuf, _IndexBlockSize, &len) && len == _IndexBlockSize) {
        if (ibBuf->Magic != INDEX_BLOCK_MAGIC) {
            NTFS_TRACE("Index Block parse error: Magic mismatch\n");
            return FALSE;
        }

        // Patch US
        WORD *usnaddr = (WORD*) ((BYTE*) ibBuf + ibBuf->OffsetOfUS);
        WORD usn = *usnaddr;
        WORD *usarray = usnaddr + 1;
        if (!PatchUS((WORD*) ibBuf, sectors, usn, usarray)) {
            NTFS_TRACE("Index Block parse error: Update Sequence Number\n");
            return FALSE;
        }

        INDEX_ENTRY *ie;
        ie = (INDEX_ENTRY*) ((BYTE*) (&(ibBuf->EntryOffset)) + ibBuf->EntryOffset);

        DWORD ieTotal = ie->Size;

        while (ieTotal <= ibBuf->TotalEntrySize) {
            CIndexEntry *ieClass = new CIndexEntry(ie);
            ibClass.InsertEntry(ieClass);

            if (ie->Flags & INDEX_ENTRY_FLAG_LAST) {
                NTFS_TRACE("Last Index Entry\n");
                break;
            }

            ie = (INDEX_ENTRY*) ((BYTE*) ie + ie->Size);  // Pick next
            ieTotal += ie->Size;
        }

        return TRUE;
    } else
        return FALSE;
}
