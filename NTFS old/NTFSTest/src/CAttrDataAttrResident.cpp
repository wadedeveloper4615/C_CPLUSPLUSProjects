/*
 * CAttrDataAttrResident.cpp
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */
#include "NTFS_Attribute.h"
#include "CFileRecord.h"
#include "CAttrDataAttrResident.h"

CAttr_Data_AttrResident::CAttr_Data_AttrResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) : CAttrResident(ahc, fr) {
    printf("Attribute: Data (%sResident)\n", IsNonResident() ? "Non" : "");
}

CAttr_Data_AttrResident::~CAttr_Data_AttrResident() {
}

