/*
 * CFileRecord.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CFILERECORD_H_
#define CFILERECORD_H_

#include "CSList.h"
#include "CNTFSVolume.h"

class CFileRecord {
        friend class CAttrBase;
    public:
        const CNTFSVolume *Volume;
        FILE_RECORD_HEADER *FileRecord;
        ULONGLONG FileReference;
        ATTR_RAW_CALLBACK AttrRawCallBack[ATTR_NUMS];
        DWORD AttrMask;
        CSList<CAttrBase> AttrList[ATTR_NUMS];
    public:
        CFileRecord(const CNTFSVolume *Volume);
        virtual ~CFileRecord();
        void ClearAttrRawCB();
        void SetAttrMask(DWORD mask);
        BOOL ParseFileRecord(ULONGLONG fileRef);
        void ClearAttrs();
        FILE_RECORD_HEADER* ReadFileRecord(ULONGLONG &fileRef);
        BOOL PatchUS(WORD *sector, int sectors, WORD usn, WORD *usarray);
        BOOL ParseAttrs();
        BOOL ParseAttr(ATTR_HEADER_COMMON *ahc);
        CAttrBase* AllocAttr(ATTR_HEADER_COMMON *ahc, BOOL *bUnhandled);
        void UserCallBack(DWORD attType, ATTR_HEADER_COMMON *ahc, BOOL *bDiscard);
        BOOL IsCompressed() const;
        BOOL IsEncrypted() const;
};

#endif /* CFILERECORD_H_ */
