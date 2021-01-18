/*
 * CAttrBase2.cpp
 *
 *  Created on: Nov 23, 2020
 *      Author: wade4
 */
#include "CAttrBase.h"

#include "NTFS.h"
#include "CFileRecord.h"
#include "CNTFSVolume.h"
#include "CPhysicalDrive.h"

CAttrBase::CAttrBase(const AttrHeaderCommonPtr attributeHeader, const CFileRecord *fileRecord) {
    AttrHeader = (AttrHeaderCommonPtr) attributeHeader;
    this->fileRecord = fileRecord;
    sectorSize = fileRecord->volume->getSectorSize();
    clusterSize = fileRecord->volume->getClusterSize();
    indexBlockSize = fileRecord->volume->getIndexBlockSize();
    volumeHandle = fileRecord->volume->getDrive()->getVolumeHandle();
}

CAttrBase::~CAttrBase() {
}

const AttrHeaderCommonPtr CAttrBase::GetAttrHeader() const {
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

const AttrHeaderCommonPtr CAttrBase::getAttrHeader() const {
    return AttrHeader;
}

WORD CAttrBase::getSectorSize() const {
    return sectorSize;
}

DWORD CAttrBase::getClusterSize() const {
    return clusterSize;
}

DWORD CAttrBase::getIndexBlockSize() const {
    return indexBlockSize;
}

HANDLE CAttrBase::getVolumeHandle() const {
    return volumeHandle;
}

const CFileRecord* CAttrBase::getFileRecord() const {
    return fileRecord;
}
