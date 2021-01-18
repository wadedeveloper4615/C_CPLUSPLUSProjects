/*
 * CAttrDataAttrNonResident.cpp
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */
#include "NTFS_Attribute.h"
#include "CFileRecord.h"
#include "CAttrDataAttrNonResident.h"

CAttr_Data_AttrNonResident::CAttr_Data_AttrNonResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) : CAttrNonResident(ahc, fr) {
    printf("Attribute: Data (%sResident)\n", IsNonResident() ? "Non" : "");
}

CAttr_Data_AttrNonResident::~CAttr_Data_AttrNonResident() {
}
