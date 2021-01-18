/*
 * CFileRecord.cpp
 *
 *  Created on: Nov 5, 2020
 *      Author: wade4
 */
#include "NTFS_Attribute.h"
#include "CNTFSVolume.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CAttrIndexAlloc.h"
#include "CIndexBlock.h"
#include "CIndexEntry.h"
#include "CAttrFileName.h"
#include "CAttrStdInfo.h"
#include "CAttrIndexRoot.h"
#include "CAttrBitmapAttrResident.h"
#include "CAttrBitmapAttrNonResident.h"
#include "CAttrIndexAlloc.h"
#include "CAttrIndexRoot.h"
#include "CAttrVolInfo.h"
#include "CAttrVolName.h"
#include "CAttrFileName.h"
#include "CAttrStdInfo.h"
#include "CAttrDataAttrNonResident.h"
#include "CAttrDataAttrResident.h"
#include "CAttrAttrListAttrResident.h"
#include "CAttrAttrListAttrNonResident.h"
#include "CFileRecord.h"

CFileRecord::CFileRecord(const CNTFSVolume *volume) {
    _ASSERT(volume);
    Volume = volume;
    FileRecord = NULL;
    FileReference = (ULONGLONG) -1;

    ClearAttrRawCB();

    // Default to parse all attributes
    AttrMask = MASK_ALL;
}

CFileRecord::~CFileRecord() {
    ClearAttrs();
    if (FileRecord)
        delete FileRecord;
}

// Free all CAttr_xxx
void CFileRecord::ClearAttrs() {
    for (int i = 0; i < ATTR_NUMS; i++) {
        AttrList[i].RemoveAll();
    }
}

// Verify US and update sectors
BOOL CFileRecord::PatchUS(WORD *sector, int sectors, WORD usn, WORD *usarray) {
    int i;

    for (i = 0; i < sectors; i++) {
        sector += ((Volume->SectorSize >> 1) - 1);
        if (*sector != usn)
            return FALSE;   // USN error
        *sector = usarray[i];   // Write back correct data
        sector++;
    }
    return TRUE;
}

// Call user defined Callback routines for an attribute
void CFileRecord::UserCallBack(DWORD attType, ATTR_HEADER_COMMON *ahc, BOOL *bDiscard) {
    *bDiscard = FALSE;

    if (AttrRawCallBack[attType])
        AttrRawCallBack[attType](ahc, bDiscard);
    else if (Volume->AttrRawCallBack[attType])
        Volume->AttrRawCallBack[attType](ahc, bDiscard);
}

CAttrBase* CFileRecord::AllocAttr(ATTR_HEADER_COMMON *ahc, BOOL *bUnhandled) {
    switch (ahc->Type) {
        case ATTR_TYPE_STANDARD_INFORMATION:
            return new CAttr_StdInfo(ahc, this);

        case ATTR_TYPE_ATTRIBUTE_LIST:
            if (ahc->NonResident)
                return new CAttr_AttrList_AttrNonResident(ahc, this);
            else
                return new CAttr_AttrList_AttrResident(ahc, this);

        case ATTR_TYPE_FILE_NAME:
            return new CAttr_FileName(ahc, this);

        case ATTR_TYPE_VOLUME_NAME:
            return new CAttr_VolName(ahc, this);

        case ATTR_TYPE_VOLUME_INFORMATION:
            return new CAttr_VolInfo(ahc, this);

        case ATTR_TYPE_DATA:
            if (ahc->NonResident)
                return new CAttr_Data_AttrNonResident(ahc, this);
            else
                return new CAttr_Data_AttrNonResident(ahc, this);

        case ATTR_TYPE_INDEX_ROOT:
            return new CAttr_IndexRoot(ahc, this);

        case ATTR_TYPE_INDEX_ALLOCATION:
            return new CAttr_IndexAlloc(ahc, this);

        case ATTR_TYPE_BITMAP:
            if (ahc->NonResident)
                return new CAttrBitmapAttrNonResident(ahc, this);
            else
                // Resident Bitmap may exist in a directory's FileRecord
                // or in $MFT for a very small volume in theory
                return new CAttrBitmapAttrResident(ahc, this);

            // Unhandled Attributes
        default:
            *bUnhandled = TRUE;
            if (ahc->NonResident)
                return new CAttrNonResident(ahc, this);
            else
                return new CAttrResident(ahc, this);
    }
}

// Parse a single Attribute
// Return False on error
BOOL CFileRecord::ParseAttr(ATTR_HEADER_COMMON *ahc) {
    DWORD attrIndex = ATTR_INDEX(ahc->Type);
    if (attrIndex < ATTR_NUMS) {
        BOOL bDiscard = FALSE;
        UserCallBack(attrIndex, ahc, &bDiscard);

        if (!bDiscard) {
            BOOL bUnhandled = FALSE;
            CAttrBase *attr = AllocAttr(ahc, &bUnhandled);
            if (attr) {
                if (bUnhandled) {
                    printf("Unhandled attribute: 0x%04X\n", ahc->Type);
                }
                AttrList[attrIndex].InsertEntry(attr);
                return TRUE;
            } else {
                printf("Attribute Parse error: 0x%04X\n", ahc->Type);
                return FALSE;
            }
        } else {
            printf("User Callback has processed this Attribute: 0x%04X\n", ahc->Type);
            return TRUE;
        }
    } else {
        printf("Invalid Attribute Type: 0x%04X\n", ahc->Type);
        return FALSE;
    }
}

// Read File Record
FILE_RECORD_HEADER* CFileRecord::ReadFileRecord(ULONGLONG &fileRef) {
    FILE_RECORD_HEADER *fr = NULL;
    DWORD len;

    if (fileRef < MFT_IDX_USER || Volume->MFTData == NULL) {
        // Take as continuous disk allocation
        LARGE_INTEGER frAddr;
        frAddr.QuadPart = Volume->MFTAddr + (Volume->FileRecordSize) * fileRef;
        frAddr.LowPart = SetFilePointer(Volume->VolumeHandle, frAddr.LowPart, &frAddr.HighPart, FILE_BEGIN);

        if (frAddr.LowPart == DWORD(-1) && GetLastError() != NO_ERROR)
            return FALSE;
        else {
            ULONGLONG offset = Volume->MFTAddr + (Volume->FileRecordSize) * fileRef;
            printf("FileRecord offset = %016llX\n", offset);

            fr = (FILE_RECORD_HEADER*) new BYTE[Volume->FileRecordSize];

            if (ReadFile(Volume->VolumeHandle, fr, Volume->FileRecordSize, &len, NULL) && len == Volume->FileRecordSize)
                return fr;
            else {
                delete fr;
                return NULL;
            }
        }
    } else {
        // May be fragmented $MFT
        ULONGLONG frAddr;
        frAddr = (Volume->FileRecordSize) * fileRef;

        fr = (FILE_RECORD_HEADER*) new BYTE[Volume->FileRecordSize];

        if (Volume->MFTData->ReadData(frAddr, fr, Volume->FileRecordSize, &len) && len == Volume->FileRecordSize)
            return fr;
        else {
            delete fr;
            return NULL;
        }
    }
}

// Read File Record, verify and patch the US (update sequence)
BOOL CFileRecord::ParseFileRecord(ULONGLONG fileRef) {
    // Clear previous data
    ClearAttrs();
    if (FileRecord) {
        delete FileRecord;
        FileRecord = NULL;
    }

    FILE_RECORD_HEADER *fr = ReadFileRecord(fileRef);
    if (fr == NULL) {
        printf("Cannot read file record %I64u\n", fileRef);

        FileReference = (ULONGLONG) -1;
    } else {
        FileReference = fileRef;

        if (fr->Magic == FILE_RECORD_MAGIC) {
            printf("File Record Magic found (0x%08lX)\n", fr->Magic);
            // Patch US
            WORD *usnaddr = (WORD*) ((BYTE*) fr + fr->OffsetOfUS);
            WORD usn = *usnaddr;
            WORD *usarray = usnaddr + 1;
            if (PatchUS((WORD*) fr, Volume->FileRecordSize / Volume->SectorSize, usn, usarray)) {
                printf("File Record %I64u Found\n", fileRef);
                FileRecord = fr;

                return TRUE;
            } else {
                printf("Update Sequence Number error\n");
            }
        } else {
            printf("Invalid file record\n");
        }

        delete fr;
    }

    return FALSE;
}

// Visit IndexBlocks recursivly to find a specific FileName
BOOL CFileRecord::VisitIndexBlock(const ULONGLONG &vcn, const _TCHAR *fileName, CIndexEntry &ieFound) const {
    CAttr_IndexAlloc *ia = (CAttr_IndexAlloc*) FindFirstAttr(ATTR_TYPE_INDEX_ALLOCATION);
    if (ia == NULL)
        return FALSE;

    CIndexBlock ib;
    if (ia->ParseIndexBlock(vcn, ib)) {
        CIndexEntry *ie = ib.FindFirstEntry();
        while (ie) {
            if (ie->HasName()) {
                // Compare name
                int i = ie->Compare(fileName);
                if (i == 0) {
                    ieFound = *ie;
                    return TRUE;
                } else if (i < 0)     // fileName is smaller than IndexEntry
                        {
                    // Visit SubNode
                    if (ie->IsSubNodePtr()) {
                        // Search in SubNode (IndexBlock), recursive call
                        if (VisitIndexBlock(ie->GetSubNodeVCN(), fileName, ieFound))
                            return TRUE;
                    } else
                        return FALSE;   // not found
                }
                // Just step forward if fileName is bigger than IndexEntry
            } else if (ie->IsSubNodePtr()) {
                // Search in SubNode (IndexBlock), recursive call
                if (VisitIndexBlock(ie->GetSubNodeVCN(), fileName, ieFound))
                    return TRUE;
            }

            ie = ib.FindNextEntry();
        }
    }

    return FALSE;
}

// Traverse SubNode recursivly in ascending order
// Call user defined callback routine once found an subentry
void CFileRecord::TraverseSubNode(const ULONGLONG &vcn, SUBENTRY_CALLBACK seCallBack) const {
    CAttr_IndexAlloc *ia = (CAttr_IndexAlloc*) FindFirstAttr(ATTR_TYPE_INDEX_ALLOCATION);
    if (ia == NULL)
        return;

    CIndexBlock ib;
    if (ia->ParseIndexBlock(vcn, ib)) {
        CIndexEntry *ie = ib.FindFirstEntry();
        while (ie) {
            if (ie->IsSubNodePtr())
                TraverseSubNode(ie->GetSubNodeVCN(), seCallBack);   // recursive call

            if (ie->HasName())
                seCallBack(ie);

            ie = ib.FindNextEntry();
        }
    }
}

// Parse all the attributes in a File Record
// And insert them into a link list
BOOL CFileRecord::ParseAttrs() {
    _ASSERT(FileRecord);

    // Clear previous data
    ClearAttrs();

    // Visit all attributes

    DWORD dataPtr = 0;  // guard if data exceeds FileRecordSize bounds
    ATTR_HEADER_COMMON *ahc = (ATTR_HEADER_COMMON*) ((BYTE*) FileRecord + FileRecord->OffsetOfAttr);
    dataPtr += FileRecord->OffsetOfAttr;

    while (ahc->Type != (DWORD) -1 && (dataPtr + ahc->TotalSize) <= Volume->FileRecordSize) {
        if (ATTR_MASK(ahc->Type) & AttrMask)    // Skip unwanted attributes
                {
            if (!ParseAttr(ahc))    // Parse error
                return FALSE;

            if (IsEncrypted() || IsCompressed()) {
                printf("Compressed and Encrypted file not supported yet !\n");
                return FALSE;
            }
        }

        dataPtr += ahc->TotalSize;
        ahc = (ATTR_HEADER_COMMON*) ((BYTE*) ahc + ahc->TotalSize);   // next attribute
    }

    return TRUE;
}

// Install Attribute raw data CallBack routines for a single File Record
BOOL CFileRecord::InstallAttrRawCB(DWORD attrType, ATTR_RAW_CALLBACK cb) {
    DWORD atIdx = ATTR_INDEX(attrType);
    if (atIdx < ATTR_NUMS) {
        AttrRawCallBack[atIdx] = cb;
        return TRUE;
    } else
        return FALSE;
}

// Clear all Attribute CallBack routines
void CFileRecord::ClearAttrRawCB() {
    for (int i = 0; i < ATTR_NUMS; i++)
        AttrRawCallBack[i] = NULL;
}

// Choose attributes to handle, unwanted attributes will be discarded silently
void CFileRecord::SetAttrMask(DWORD mask) {
    // Standard Information and Attribute List is needed always
    AttrMask = mask | MASK_STANDARD_INFORMATION | MASK_ATTRIBUTE_LIST;
}

// Traverse all Attribute and return CAttr_xxx classes to User Callback routine
void CFileRecord::TraverseAttrs(ATTRS_CALLBACK attrCallBack, void *context) {
    _ASSERT(attrCallBack);

    for (int i = 0; i < ATTR_NUMS; i++) {
        if (AttrMask & (((DWORD) 1) << i)) // skip masked attributes
                {
            const CAttrBase *ab = AttrList[i].FindFirstEntry();
            while (ab) {
                BOOL bStop;
                bStop = FALSE;
                attrCallBack(ab, context, &bStop);
                if (bStop)
                    return;

                ab = AttrList[i].FindNextEntry();
            }
        }
    }
}

// Find Attributes
const CAttrBase* CFileRecord::FindFirstAttr(DWORD attrType) const {
    DWORD attrIdx = ATTR_INDEX(attrType);

    return attrIdx < ATTR_NUMS ? AttrList[attrIdx].FindFirstEntry() : NULL;
}

const CAttrBase* CFileRecord::FindNextAttr(DWORD attrType) const {
    DWORD attrIdx = ATTR_INDEX(attrType);

    return attrIdx < ATTR_NUMS ? AttrList[attrIdx].FindNextEntry() : NULL;
}

// Get File Name (First Win32 name)
int CFileRecord::GetFileName(_TCHAR *buf, DWORD bufLen) const {
    // A file may have several filenames
    // Return the first Win32 filename
    CAttr_FileName *fn = (CAttr_FileName*) AttrList[ATTR_INDEX(ATTR_TYPE_FILE_NAME)].FindFirstEntry();
    while (fn) {
        if (fn->IsWin32Name()) {
            int len = fn->GetFileName(buf, bufLen);
            if (len != 0)
                return len; // success or fail
        }

        fn = (CAttr_FileName*) AttrList[ATTR_INDEX(ATTR_TYPE_FILE_NAME)].FindNextEntry();
    }

    return 0;
}

// Get File Size
ULONGLONG CFileRecord::GetFileSize() const {
    CAttr_FileName *fn = (CAttr_FileName*) AttrList[ATTR_INDEX(ATTR_TYPE_FILE_NAME)].FindFirstEntry();
    return fn ? fn->GetFileSize() : 0;
}

// Get File Times
void CFileRecord::GetFileTime(FILETIME *writeTm, FILETIME *createTm, FILETIME *accessTm) const {
    // Standard Information attribute hold the most updated file time
    CAttr_StdInfo *si = (CAttr_StdInfo*) AttrList[ATTR_INDEX(ATTR_TYPE_STANDARD_INFORMATION)].FindFirstEntry();
    if (si)
        si->GetFileTime(writeTm, createTm, accessTm);
    else {
        writeTm->dwHighDateTime = 0;
        writeTm->dwLowDateTime = 0;
        if (createTm) {
            createTm->dwHighDateTime = 0;
            createTm->dwLowDateTime = 0;
        }
        if (accessTm) {
            accessTm->dwHighDateTime = 0;
            accessTm->dwLowDateTime = 0;
        }
    }
}

// Traverse all sub directories and files contained
// Call user defined callback routine once found an entry
void CFileRecord::TraverseSubEntries(SUBENTRY_CALLBACK seCallBack) const {
    _ASSERT(seCallBack);

    // Start traversing from IndexRoot (B+ tree root node)

    CAttr_IndexRoot *ir = (CAttr_IndexRoot*) FindFirstAttr(ATTR_TYPE_INDEX_ROOT);
    if (ir == NULL || !ir->IsFileName())
        return;

    CIndexEntryList *ieList = (CIndexEntryList*) ir;
    CIndexEntry *ie = ieList->FindFirstEntry();
    while (ie) {
        // Visit subnode first
        if (ie->IsSubNodePtr())
            TraverseSubNode(ie->GetSubNodeVCN(), seCallBack);

        if (ie->HasName())
            seCallBack(ie);

        ie = ieList->FindNextEntry();
    }
}

// Find a specific FileName from InexRoot described B+ tree
const BOOL CFileRecord::FindSubEntry(const _TCHAR *fileName, CIndexEntry &ieFound) const {
    // Start searching from IndexRoot (B+ tree root node)
    CAttr_IndexRoot *ir = (CAttr_IndexRoot*) FindFirstAttr(ATTR_TYPE_INDEX_ROOT);
    if (ir == NULL || !ir->IsFileName())
        return FALSE;

    CIndexEntryList *ieList = (CIndexEntryList*) ir;
    CIndexEntry *ie = ieList->FindFirstEntry();
    while (ie) {
        if (ie->HasName()) {
            // Compare name
            int i = ie->Compare(fileName);
            if (i == 0) {
                ieFound = *ie;
                return TRUE;
            } else if (i < 0)     // fileName is smaller than IndexEntry
                    {
                // Visit SubNode
                if (ie->IsSubNodePtr()) {
                    // Search in SubNode (IndexBlock)
                    if (VisitIndexBlock(ie->GetSubNodeVCN(), fileName, ieFound))
                        return TRUE;
                } else
                    return FALSE;   // not found
            }
            // Just step forward if fileName is bigger than IndexEntry
        } else if (ie->IsSubNodePtr()) {
            // Search in SubNode (IndexBlock)
            if (VisitIndexBlock(ie->GetSubNodeVCN(), fileName, ieFound))
                return TRUE;
        }

        ie = ieList->FindNextEntry();
    }

    return FALSE;
}

// Find Data attribute class of
const CAttrBase* CFileRecord::FindStream(_TCHAR *name) {
    const CAttrBase *data = FindFirstAttr(ATTR_TYPE_DATA);
    while (data) {
        if (data->IsUnNamed() && name == NULL)  // Unnamed stream
            break;
        if ((!data->IsUnNamed()) && name)   // Named stream
                {
            _TCHAR an[MAX_PATH];
            if (data->GetAttrName(an, MAX_PATH)) {
                if (_tcscmp(an, name) == 0)
                    break;
            }
        }

        data = FindNextAttr(ATTR_TYPE_DATA);
    }

    return data;
}

// Check if it's deleted or in use
BOOL CFileRecord::IsDeleted() const {
    return !(FileRecord->Flags & FILE_RECORD_FLAG_INUSE);
}

// Check if it's a directory
BOOL CFileRecord::IsDirectory() const {
    return FileRecord->Flags & FILE_RECORD_FLAG_DIR;
}

BOOL CFileRecord::IsReadOnly() const {
    // Standard Information attribute holds the most updated file time
    const CAttr_StdInfo *si = (CAttr_StdInfo*) AttrList[ATTR_INDEX(ATTR_TYPE_STANDARD_INFORMATION)].FindFirstEntry();
    return si ? si->IsReadOnly() : FALSE;
}

BOOL CFileRecord::IsHidden() const {
    const CAttr_StdInfo *si = (CAttr_StdInfo*) AttrList[ATTR_INDEX(ATTR_TYPE_STANDARD_INFORMATION)].FindFirstEntry();
    return si ? si->IsHidden() : FALSE;
}

BOOL CFileRecord::IsSystem() const {
    const CAttr_StdInfo *si = (CAttr_StdInfo*) AttrList[ATTR_INDEX(ATTR_TYPE_STANDARD_INFORMATION)].FindFirstEntry();
    return si ? si->IsSystem() : FALSE;
}

BOOL CFileRecord::IsCompressed() const {
    const CAttr_StdInfo *si = (CAttr_StdInfo*) AttrList[ATTR_INDEX(ATTR_TYPE_STANDARD_INFORMATION)].FindFirstEntry();
    return si ? si->IsCompressed() : FALSE;
}

BOOL CFileRecord::IsEncrypted() const {
    const CAttr_StdInfo *si = (CAttr_StdInfo*) AttrList[ATTR_INDEX(ATTR_TYPE_STANDARD_INFORMATION)].FindFirstEntry();
    return si ? si->IsEncrypted() : FALSE;
}

BOOL CFileRecord::IsSparse() const {
    const CAttr_StdInfo *si = (CAttr_StdInfo*) AttrList[ATTR_INDEX(ATTR_TYPE_STANDARD_INFORMATION)].FindFirstEntry();
    return si ? si->IsSparse() : FALSE;
}
