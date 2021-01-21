/*
 * CFileRecord.cpp
 *
 *  Created on: Nov 19, 2020
 *      Author: wade4
 */
#include "NTFS.h"
#include "CPhysicalDrive.h"
#include "CNTFSVolume.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CAttrVolInfo.h"
#include "CAttrStdInfo.h"
#include "CAttrVolName.h"
#include "CFileRecord.h"
#include "CAttrBase.h"

CFileRecord::CFileRecord(CNTFSVolume *volume) {
    this->volume = volume;
    ClearAttrRawCB();
    AttrMask = -1;
}

CFileRecord::~CFileRecord() {
}

void CFileRecord::ClearAttrRawCB() {
    for (int i = 0; i < ATTR_NUMS; i++)
        AttrRawCallBack[i] = NULL;
}

void CFileRecord::ClearAttrs() {
    for (int i = 0; i < ATTR_NUMS; i++) {
        AttrList[i].RemoveAll();
    }
}

void CFileRecord::SetAttrMask(DWORD mask) {
    // Standard Information and Attribute List is needed always
    AttrMask = mask | MASK_STANDARD_INFORMATION | MASK_ATTRIBUTE_LIST;
}

void CFileRecord::UserCallBack(DWORD attType, AttrHeaderCommonPtr ahc, BOOL *bDiscard) {
    *bDiscard = FALSE;

    if (AttrRawCallBack[attType])
        AttrRawCallBack[attType](ahc, bDiscard);
//    else if (Volume->AttrRawCallBack[attType])
//        Volume->AttrRawCallBack[attType](ahc, bDiscard);
}

CAttrBase* CFileRecord::AllocAttr(AttrHeaderCommonPtr ahc, BOOL *bUnhandled) {
    switch (ahc->Type) {
        case ATTR_TYPE_STANDARD_INFORMATION:
            return new CAttr_StdInfo(ahc, this);
        case ATTR_TYPE_ATTRIBUTE_LIST:
            printf("ATTR_TYPE_ATTRIBUTE_LIST\n");
            break;
        case ATTR_TYPE_FILE_NAME:
            printf("ATTR_TYPE_FILE_NAME\n");
            break;
        case ATTR_TYPE_VOLUME_NAME:
            return new CAttr_VolName(ahc, this);
        case ATTR_TYPE_VOLUME_INFORMATION:
            return new CAttr_VolInfo(ahc, this);
        case ATTR_TYPE_DATA:
            printf("ATTR_TYPE_DATA\n");
            break;
        case ATTR_TYPE_INDEX_ROOT:
            printf("ATTR_TYPE_INDEX_ROOT\n");
            break;
        case ATTR_TYPE_INDEX_ALLOCATION:
            printf("ATTR_TYPE_INDEX_ALLOCATION\n");
            break;
        case ATTR_TYPE_BITMAP:
            printf("ATTR_TYPE_BITMAP\n");
            break;
        default:
            *bUnhandled = TRUE;
            if (ahc->NonResident) {
                printf("Attribute: NonResident\n");
                return new CAttrNonResident(ahc, this);
            } else {
                printf("Attribute: Resident\n");
                return new CAttrResident(ahc, this);
            }
    }
    return NULL;
}

BOOL CFileRecord::ParseAttr(AttrHeaderCommonPtr ahc) {
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
    return TRUE;
}

BOOL CFileRecord::ParseAttrs() {
    ClearAttrs();
    AttrHeaderCommonPtr ahc = (AttrHeaderCommonPtr) ((BYTE*) FileRecord + FileRecord->OffsetOfAttr);
    DWORD dataPtr = FileRecord->OffsetOfAttr;
    while (ahc->Type != (DWORD) -1 && (dataPtr + ahc->TotalSize) <= volume->getFileRecordSize()) {
        if (ATTR_MASK(ahc->Type) & AttrMask) {
            if (!ParseAttr(ahc))
                return FALSE;

//        if (IsEncrypted() || IsCompressed()) {
//            printf("Compressed and Encrypted file not supported yet !\n");
//            return FALSE;
//        }
        }
        dataPtr += ahc->TotalSize;
        ahc = (AttrHeaderCommonPtr) ((BYTE*) ahc + ahc->TotalSize);   // next attribute
    }
    return TRUE;
}

const CAttrBase* CFileRecord::FindFirstAttr(DWORD attrType) const {
    DWORD attrIdx = ATTR_INDEX(attrType);
    return attrIdx < ATTR_NUMS ? AttrList[attrIdx].FindFirstEntry() : NULL;
}

BOOL CFileRecord::ParseFileRecord(ULONGLONG fileRef) {
    FileRecordHeaderPtr fr = ReadFileRecord(fileRef);
    printRecordHeader(fr);
    if (fr->Magic == FILE_RECORD_MAGIC) {
        printf("File Record Magic found (0x%08lX)\n", fr->Magic);
        WORD *usnaddr = (WORD*) ((BYTE*) fr + fr->OffsetOfUS);
        WORD usn = *usnaddr;
        WORD *usarray = usnaddr + 1;
        if (PatchUpdateSequence((WORD*) fr, volume->getFileRecordSize() / volume->getSectorSize(), usn, usarray)) {
            printf("File Record %I64u Found\n", fileRef);
            FileRecord = fr;
        } else {
            printf("Update Sequence Number error\n");
        }
    } else {
        printf("Invalid file record\n");
    }
    return TRUE;
}

BOOL CFileRecord::PatchUpdateSequence(WORD *sector, int sectors, WORD usn, WORD *usarray) {
    int i;

    for (i = 0; i < sectors; i++) {
        sector += ((volume->getSectorSize() >> 1) - 1);
        if (*sector != usn)
            return FALSE;   // USN error
        *sector = usarray[i];   // Write back correct data
        sector++;
    }
    return TRUE;
}

void CFileRecord::printRecordHeader(FileRecordHeaderPtr fr) {
    printf("Magic:         0x%lX\n", fr->Magic); //DWORD Magic;
    printf("OffsetOfUS:    %d\n", fr->OffsetOfUS); //WORD OffsetOfUS;
    printf("SizeOfUS:      %d\n", fr->SizeOfUS); //WORD SizeOfUS;
    printf("LSN:           %lld\n", fr->LSN); //ULONGLONG LSN;
    printf("SeqNo:         %d\n", fr->SeqNo); //WORD SeqNo;
    printf("Hardlinks:     %d\n", fr->Hardlinks); //WORD Hardlinks;
    printf("OffsetOfAttr:  %d\n", fr->OffsetOfAttr); //WORD OffsetOfAttr;
    printf("Flags:         0x%04X\n", fr->Flags); //WORD Flags;
    printf("RealSize:      %ld\n", fr->RealSize); //DWORD RealSize;
    printf("AllocSize:     %ld\n", fr->AllocSize); //DWORD AllocSize;
    printf("RefToBase:     %lld\n", fr->RefToBase); //ULONGLONG RefToBase;
    printf("NextAttrId:    %d\n", fr->NextAttrId); //WORD NextAttrId;
    printf("Align:         %d\n", fr->Align); //WORD Align;
    printf("RecordNo:      %ld\n", fr->RecordNo); //DWORD RecordNo;
}

FileRecordHeaderPtr CFileRecord::ReadFileRecord(ULONGLONG &fileRef) {
    FileRecordHeaderPtr fr = NULL;
    DWORD len;

    if (fileRef < MFT_IDX_USER) {
        ULONGLONG offset = volume->getMFTAddr() + volume->getFileRecordSize() * fileRef;
        printf("FileRecord offset = %016llX\n", offset);
        volume->getDrive()->seek(volume->getPartitionOffset() + offset, FILE_BEGIN);

        fr = (FileRecordHeaderPtr) new BYTE[volume->getFileRecordSize()];
        volume->getDrive()->read(fr, volume->getFileRecordSize(), &len);
    }
    return fr;
}
