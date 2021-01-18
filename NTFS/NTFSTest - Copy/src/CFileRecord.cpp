/*
 * CFileRecord.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "NTFS_Common.h"
#include "CNTFSVolume.h"
#include "CAttrBase.h"
#include "CSList.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CAttrStdInfo.h"
#include "NTFS_Attribute.h"
#include "CAttrBitmapAttrNonResident.h"
#include "CAttrBitmapAttrResident.h"
#include "CAttrIndexAlloc.h"
#include "CIndexEntry.h"
#include "CAttrIndexRoot.h"
#include "CFileRecord.h"

template class CSList<CAttrBase> ;
template class CSList<DataRun_Entry> ;

CFileRecord::CFileRecord(const CNTFSVolume *Volume) {
    this->Volume = Volume;
    FileRecord = NULL;
    FileReference = (ULONGLONG) -1;
    ClearAttrRawCB();
    AttrMask = MASK_ALL;
}

CFileRecord::~CFileRecord() {
}

void CFileRecord::ClearAttrRawCB() {
    for (int i = 0; i < ATTR_NUMS; i++)
        AttrRawCallBack[i] = NULL;
}

void CFileRecord::SetAttrMask(DWORD mask) {
    AttrMask = mask | MASK_STANDARD_INFORMATION | MASK_ATTRIBUTE_LIST;
}

void CFileRecord::ClearAttrs() {
    for (int i = 0; i < ATTR_NUMS; i++) {
        AttrList[i].RemoveAll();
    }
}

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

BOOL CFileRecord::ParseFileRecord(ULONGLONG fileRef) {
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

CAttrBase* CFileRecord::AllocAttr(ATTR_HEADER_COMMON *ahc, BOOL *bUnhandled) {
    switch (ahc->Type) {
        case ATTR_TYPE_STANDARD_INFORMATION:
//            return new CAttr_StdInfo(ahc, this);

        case ATTR_TYPE_ATTRIBUTE_LIST:
//            if (ahc->NonResident)
//                return new CAttr_AttrList<CAttrNonResident>(ahc, this);
//            else
//                return new CAttr_AttrList<CAttrResident>(ahc, this);

        case ATTR_TYPE_FILE_NAME:
//            return new CAttr_FileName(ahc, this);

        case ATTR_TYPE_VOLUME_NAME:
//            return new CAttr_VolName(ahc, this);

        case ATTR_TYPE_VOLUME_INFORMATION:
//            return new CAttr_VolInfo(ahc, this);

        case ATTR_TYPE_DATA:
//            if (ahc->NonResident)
//                return new CAttr_Data<CAttrNonResident>(ahc, this);
//            else
//                return new CAttr_Data<CAttrResident>(ahc, this);
        case ATTR_TYPE_INDEX_ROOT:
            return new CAttr_IndexRoot(ahc, this);
        case ATTR_TYPE_INDEX_ALLOCATION:
            return new CAttr_IndexAlloc(ahc, this);
        case ATTR_TYPE_BITMAP:
            if (ahc->NonResident)
                return new CAttrBitmapAttrNonResident(ahc, this);
            else
                return new CAttrBitmapAttrResident(ahc, this);
        default:
            *bUnhandled = TRUE;
            if (ahc->NonResident)
                return new CAttrNonResident(ahc, this);
            else
                return new CAttrResident(ahc, this);
    }
}

void CFileRecord::UserCallBack(DWORD attType, ATTR_HEADER_COMMON *ahc, BOOL *bDiscard) {
    *bDiscard = FALSE;

    if (AttrRawCallBack[attType])
        AttrRawCallBack[attType](ahc, bDiscard);
    else if (Volume->AttrRawCallBack[attType])
        Volume->AttrRawCallBack[attType](ahc, bDiscard);
}

BOOL CFileRecord::IsCompressed() const {
    const CAttr_StdInfo *si = (CAttr_StdInfo*) AttrList[ATTR_INDEX(ATTR_TYPE_STANDARD_INFORMATION)].FindFirstEntry();
    return si ? si->IsCompressed() : FALSE;
}

BOOL CFileRecord::IsEncrypted() const {
    const CAttr_StdInfo *si = (CAttr_StdInfo*) AttrList[ATTR_INDEX(ATTR_TYPE_STANDARD_INFORMATION)].FindFirstEntry();
    return si ? si->IsEncrypted() : FALSE;
}

