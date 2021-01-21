/*
 * CAttrBase.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "NTFS_Common.h"
#include "CFileRecord.h"
#include "CAttrBase.h"

CAttrBase::CAttrBase(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) {
    _ASSERT(ahc);
    _ASSERT(fr);

    AttrHeader = ahc;
    FileRecord = fr;

    _SectorSize = fr->Volume->SectorSize;
    _ClusterSize = fr->Volume->ClusterSize;
    _IndexBlockSize = fr->Volume->IndexBlockSize;
    _hVolume = fr->Volume->VolumeHandle;
}

CAttrBase::~CAttrBase() {
}

const ATTR_HEADER_COMMON* CAttrBase::GetAttrHeader() const {
    return AttrHeader;
}

DWORD CAttrBase::GetAttrType() const {
    return AttrHeader->Type;
}

DWORD CAttrBase::GetAttrTotalSize() const {
    return AttrHeader->TotalSize;
}

BOOL CAttrBase::IsNonResident() const {
    return AttrHeader->NonResident;
}

WORD CAttrBase::GetAttrFlags() const {
    return AttrHeader->Flags;
}

// Get ANSI Attribute name
// Return 0: Unnamed, <0: buffer too small, -buffersize, >0 Name length
int CAttrBase::GetAttrName(char *buf, DWORD bufLen) const {
    if (AttrHeader->NameLength) {
        if (bufLen < AttrHeader->NameLength)
            return -1 * AttrHeader->NameLength;   // buffer too small

        wchar_t *namePtr = (wchar_t*) ((BYTE*) AttrHeader + AttrHeader->NameOffset);
        int len = WideCharToMultiByte(CP_ACP, 0, namePtr, AttrHeader->NameLength, buf, bufLen, NULL, NULL);
        if (len) {
            buf[len] = '\0';
            printf("Attribute name: %s\n", buf);
            return len;
        } else {
            printf("Unrecognized attribute name or Name buffer too small\n");
            return -1 * AttrHeader->NameLength;
        }
    } else {
        printf("Attribute is unnamed\n");
        return 0;
    }
}

// Get UNICODE Attribute name
// Return 0: Unnamed, <0: buffer too small, -buffersize, >0 Name length
int CAttrBase::GetAttrName(wchar_t *buf, DWORD bufLen) const {
    if (AttrHeader->NameLength) {
        if (bufLen < AttrHeader->NameLength)
            return -1 * AttrHeader->NameLength;   // buffer too small

        bufLen = AttrHeader->NameLength;
        wchar_t *namePtr = (wchar_t*) ((BYTE*) AttrHeader + AttrHeader->NameOffset);
        wcsncpy(buf, namePtr, bufLen);
        buf[bufLen] = '\0\0';

        printf("Unicode Attribute Name\n");
        return bufLen;
    } else {
        printf("Attribute is unnamed\n");
        return 0;
    }
}

// Verify if this attribute is unnamed
// Useful in analyzing MultiStream files
BOOL CAttrBase::IsUnNamed() const {
    return (AttrHeader->NameLength == 0);
}
