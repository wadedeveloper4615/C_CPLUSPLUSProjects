/*
 * CFileRecord.h
 *
 *  Created on: Nov 19, 2020
 *      Author: wade4
 */

#ifndef CFILERECORD_H_
#define CFILERECORD_H_

#include "CAttrBase.h"
#include "NTFS.h"
#include "CSList.h"
class CNTFSVolume;

class CFileRecord {
    public:
        CNTFSVolume *volume;
        FileRecordHeaderPtr FileRecord;
        ATTR_RAW_CALLBACK AttrRawCallBack[ATTR_NUMS];
        CAttrList AttrList[ATTR_NUMS];
        DWORD AttrMask;
    public:
        CFileRecord(CNTFSVolume *volume);
        virtual ~CFileRecord();
        BOOL ParseFileRecord(ULONGLONG fileRef);
        FileRecordHeaderPtr ReadFileRecord(ULONGLONG &fileRef);
        void printRecordHeader(FileRecordHeaderPtr fr);
        BOOL PatchUpdateSequence(WORD *sector, int sectors, WORD usn, WORD *usarray);
        BOOL ParseAttrs();
        const CAttrBase* FindFirstAttr(DWORD attrType) const;
        void ClearAttrs();
        BOOL ParseAttr(AttrHeaderCommonPtr ahc);
        void UserCallBack(DWORD attType, AttrHeaderCommonPtr ahc, BOOL *bDiscard);
        void ClearAttrRawCB();
        CAttrBase* AllocAttr(AttrHeaderCommonPtr ahc, BOOL *bUnhandled);
        void SetAttrMask(DWORD mask);
};

#endif /* CFILERECORD_H_ */
