/*
 * CIndexEntry.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "NTFS_Attribute.h"
#include "CNTFSVolume.h"
#include "CAttrBase.h"
#include "CFileRecord.h"
#include "CIndexEntry.h"

CIndexEntry::CIndexEntry() {
    //printf("Index Entry\n");

    IsDefault = TRUE;

    IndexEntry = NULL;
    SetFileName(NULL);
}

CIndexEntry::CIndexEntry(const INDEX_ENTRY *ie) {
    //printf("Index Entry\n");

    IsDefault = FALSE;

    _ASSERT(ie);
    IndexEntry = ie;

    if (IsSubNodePtr()) {
        printf("Points to sub-node\n");
    }

    if (ie->StreamSize) {
        SetFileName((ATTR_FILE_NAME*) (ie->Stream));
    } else {
        printf("No FileName stream found\n");
    }
}

CIndexEntry::~CIndexEntry() {
    // Never touch *IndexEntry here if IsCopy == FALSE !
    // As the memory have been deallocated by ~CIndexBlock()
    if (IsCopy && IndexEntry)
        delete (void*) IndexEntry;
}

CIndexEntry& CIndexEntry::operator =(const CIndexEntry &ieClass) {
    if (!IsDefault) {
        printf("Cannot call this routine\n");
        return *this;
    }

    //printf("Index Entry Copied\n");

    IsCopy = TRUE;

    if (IndexEntry) {
        delete (void*) IndexEntry;
        IndexEntry = NULL;
    }

    const INDEX_ENTRY *ie = ieClass.IndexEntry;
    _ASSERT(ie && (ie->Size > 0));

    IndexEntry = (INDEX_ENTRY*) new BYTE[ie->Size];
    memcpy((void*) IndexEntry, ie, ie->Size);
    CopyFileName(&ieClass, (ATTR_FILE_NAME*) (IndexEntry->Stream));

    return *this;
}

ULONGLONG CIndexEntry::GetFileReference() const {
    if (IndexEntry)
        return IndexEntry->FileReference & 0x0000FFFFFFFFFFFFUL;
    else
        return (ULONGLONG) -1;
}

BOOL CIndexEntry::IsSubNodePtr() const {
    if (IndexEntry)
        return (IndexEntry->Flags & INDEX_ENTRY_FLAG_SUBNODE);
    else
        return FALSE;
}

ULONGLONG CIndexEntry::GetSubNodeVCN() const {
    if (IndexEntry)
        return *(ULONGLONG*) ((BYTE*) IndexEntry + IndexEntry->Size - 8);
    else
        return (ULONGLONG) -1;
}
