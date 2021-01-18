/*
 * CAttrVolInfo.cpp
 *
 *  Created on: Nov 5, 2020
 *      Author: wade4
 */
#include "NTFS_Attribute.h"
#include "CFileRecord.h"
#include "CAttrVolInfo.h"

CAttr_VolInfo::CAttr_VolInfo(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) : CAttrResident(ahc, fr) {
    printf("Attribute: Volume Information\n");

    VolInfo = (ATTR_VOLUME_INFORMATION*) AttrBody;
}

CAttr_VolInfo::~CAttr_VolInfo() {
}

// Get NTFS Volume Version
WORD CAttr_VolInfo::GetVersion() {
    return MAKEWORD(VolInfo->MinorVersion, VolInfo->MajorVersion);
}
