/*
 * CAttrFileName.cpp
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */
#include "NTFS_Attribute.h"
#include "CNTFSVolume.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CAttrIndexAlloc.h"
#include "CIndexBlock.h"
#include "CIndexEntry.h"
#include "CFileRecord.h"
#include "CAttrFileName.h"

CAttr_FileName::CAttr_FileName(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) : CAttrResident(ahc, fr) {
    printf("Attribute: File Name\n");
    SetFileName((ATTR_FILE_NAME*) AttrBody);
}

CAttr_FileName::~CAttr_FileName() {
}

void CAttr_FileName::GetFileTime(FILETIME *writeTm, FILETIME *createTm, FILETIME *accessTm) const {
}

DWORD CAttr_FileName::GetFilePermission() {
    return 0;
}

BOOL CAttr_FileName::IsReadOnly() const {
    return false;
}

BOOL CAttr_FileName::IsHidden() const {
    return false;
}

BOOL CAttr_FileName::IsSystem() const {
    return false;
}

BOOL CAttr_FileName::IsCompressed() const {
    return false;
}

BOOL CAttr_FileName::IsEncrypted() const {
    return false;
}

BOOL CAttr_FileName::IsSparse() const {
    return false;
}
