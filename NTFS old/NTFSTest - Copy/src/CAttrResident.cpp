/*
 * CAttrResident.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "NTFS_Common.h"
#include "CNTFSVolume.h"
#include "CAttrBase.h"
#include "CFileRecord.h"
#include "CAttrResident.h"

CAttrResident::CAttrResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) : CAttrBase(ahc, fr) {
    AttrHeaderR = (ATTR_HEADER_RESIDENT*) ahc;
    AttrBody = (void*) ((BYTE*) AttrHeaderR + AttrHeaderR->AttrOffset);
    AttrBodySize = AttrHeaderR->AttrSize;
}

CAttrResident::~CAttrResident() {
}

BOOL CAttrResident::IsDataRunOK() const {
    return TRUE;    // Always OK for a resident attribute
}

// Return Actural Data Size
// *allocSize = Allocated Size
ULONGLONG CAttrResident::GetDataSize(ULONGLONG *allocSize) const {
    if (allocSize)
        *allocSize = AttrBodySize;

    return (ULONGLONG) AttrBodySize;
}

// Read "bufLen" bytes from "offset" into "bufv"
// Number of bytes acturally read is returned in "*actural"
BOOL CAttrResident::ReadData(const ULONGLONG &offset, void *bufv, DWORD bufLen, DWORD *actural) const {
    _ASSERT(bufv);

    *actural = 0;
    if (bufLen == 0)
        return TRUE;

    DWORD offsetd = (DWORD) offset;
    if (offsetd >= AttrBodySize)
        return FALSE;   // offset parameter error

    if ((offsetd + bufLen) > AttrBodySize)
        *actural = AttrBodySize - offsetd;  // Beyond scope
    else
        *actural = bufLen;

    memcpy(bufv, (BYTE*) AttrBody + offsetd, *actural);

    return TRUE;
}
