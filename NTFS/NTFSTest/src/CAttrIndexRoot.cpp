/*
 * CAttrIndexRoot.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "CNTFSVolume.h"
#include "CAttrBase.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CAttrStdInfo.h"
#include "NTFS_Attribute.h"
#include "CAttrIndexAlloc.h"
#include "CAttrResident.h"
#include "CAttrIndexRoot.h"
#include "CIndexEntry.h"
#include "CFileRecord.h"

CAttr_IndexRoot::CAttr_IndexRoot(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) : CAttrResident(ahc, fr) {

    printf("Attribute: Index Root\n");

    IndexRoot = (ATTR_INDEX_ROOT*) AttrBody;

    if (IsFileName()) {
        ParseIndexEntries();
    } else {
        printf("Index View not supported\n");
    }
}

CAttr_IndexRoot::~CAttr_IndexRoot() {
}

// Get all the index entries
void CAttr_IndexRoot::ParseIndexEntries() {
    INDEX_ENTRY *ie;
    ie = (INDEX_ENTRY*) ((BYTE*) (&(IndexRoot->EntryOffset)) + IndexRoot->EntryOffset);

    DWORD ieTotal = ie->Size;

    while (ieTotal <= IndexRoot->TotalEntrySize) {
        CIndexEntry *ieClass = new CIndexEntry(ie);
        InsertEntry(ieClass);

        if (ie->Flags & INDEX_ENTRY_FLAG_LAST) {
            printf("Last Index Entry\n");
            break;
        }

        ie = (INDEX_ENTRY*) ((BYTE*) ie + ie->Size);  // Pick next
        ieTotal += ie->Size;
    }
}

// Check if this IndexRoot contains FileName or IndexView
BOOL CAttr_IndexRoot::IsFileName() const {
    return (IndexRoot->AttrType == ATTR_TYPE_FILE_NAME);
}

