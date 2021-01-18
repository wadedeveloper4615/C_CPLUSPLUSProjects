/*
 * CAttrStdInfo.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "CNTFSVolume.h"
#include "CAttrBase.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CFileRecord.h"
#include "CAttrStdInfo.h"

CAttr_StdInfo::CAttr_StdInfo(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) : CAttrResident(ahc, fr) {
    printf("Attribute: Standard Information\n");
    StdInfo = (ATTR_STANDARD_INFORMATION*) AttrBody;
}

CAttr_StdInfo::~CAttr_StdInfo() {
}

// Change from UTC time to local time
void CAttr_StdInfo::GetFileTime(FILETIME *writeTm, FILETIME *createTm, FILETIME *accessTm) const {
    UTC2Local(StdInfo->AlterTime, writeTm);

    if (createTm)
        UTC2Local(StdInfo->CreateTime, createTm);

    if (accessTm)
        UTC2Local(StdInfo->ReadTime, accessTm);
}

DWORD CAttr_StdInfo::GetFilePermission() const {
    return StdInfo->Permission;
}

BOOL CAttr_StdInfo::IsReadOnly() const {
    return ((StdInfo->Permission) & ATTR_STDINFO_PERMISSION_READONLY);
}

BOOL CAttr_StdInfo::IsHidden() const {
    return ((StdInfo->Permission) & ATTR_STDINFO_PERMISSION_HIDDEN);
}

BOOL CAttr_StdInfo::IsSystem() const {
    return ((StdInfo->Permission) & ATTR_STDINFO_PERMISSION_SYSTEM);
}

BOOL CAttr_StdInfo::IsCompressed() const {
    return ((StdInfo->Permission) & ATTR_STDINFO_PERMISSION_COMPRESSED);
}

BOOL CAttr_StdInfo::IsEncrypted() const {
    return ((StdInfo->Permission) & ATTR_STDINFO_PERMISSION_ENCRYPTED);
}

BOOL CAttr_StdInfo::IsSparse() const {
    return ((StdInfo->Permission) & ATTR_STDINFO_PERMISSION_SPARSE);
}

// UTC filetime to Local filetime
void CAttr_StdInfo::UTC2Local(const ULONGLONG &ultm, FILETIME *lftm) {
    LARGE_INTEGER fti;
    FILETIME ftt;

    fti.QuadPart = ultm;
    ftt.dwHighDateTime = fti.HighPart;
    ftt.dwLowDateTime = fti.LowPart;

    if (!FileTimeToLocalFileTime(&ftt, lftm))
        *lftm = ftt;
}
