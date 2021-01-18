/*
 * CIndexEntry.cpp
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
#include "CIndexEntry.h"

CIndexEntry::CIndexEntry() {
}

CIndexEntry::CIndexEntry(INDEX_ENTRY *ie) {
}

CIndexEntry::~CIndexEntry() {
}

