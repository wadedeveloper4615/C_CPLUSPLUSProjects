/*
 * CFileRecord.h
 *
 *  Created on: Nov 5, 2020
 *      Author: wade4
 */

#ifndef CFILERECORD_H_
#define CFILERECORD_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CAttrBase.h"

class CNTFSVolume;

class CFileRecord {
        friend class CAttrBase;
    public:
        const CNTFSVolume *Volume;
    public:
        FILE_RECORD_HEADER *FileRecord;
        ULONGLONG FileReference;
        ATTR_RAW_CALLBACK AttrRawCallBack[ATTR_NUMS];
        DWORD AttrMask;
        CAttrList AttrList[ATTR_NUMS];  // Attributes

        void ClearAttrs();
        BOOL PatchUS(WORD *sector, int sectors, WORD usn, WORD *usarray);
        void UserCallBack(DWORD attType, ATTR_HEADER_COMMON *ahc, BOOL *bDiscard);
        CAttrBase* AllocAttr(ATTR_HEADER_COMMON *ahc, BOOL *bUnhandled);
        BOOL ParseAttr(ATTR_HEADER_COMMON *ahc);
        FILE_RECORD_HEADER* ReadFileRecord(ULONGLONG &fileRef);
        BOOL VisitIndexBlock(const ULONGLONG &vcn, const _TCHAR *fileName, CIndexEntry &ieFound) const;
        void TraverseSubNode(const ULONGLONG &vcn, SUBENTRY_CALLBACK seCallBack) const;
    public:
        CFileRecord(const CNTFSVolume *volume);
        virtual ~CFileRecord();
    public:
        BOOL ParseFileRecord(ULONGLONG fileRef);
        BOOL ParseAttrs();
        BOOL InstallAttrRawCB(DWORD attrType, ATTR_RAW_CALLBACK cb);
        void ClearAttrRawCB();
        void SetAttrMask(DWORD mask);
        void TraverseAttrs(ATTRS_CALLBACK attrCallBack, void *context);
        const CAttrBase* FindFirstAttr(DWORD attrType) const;
        const CAttrBase* FindNextAttr(DWORD attrType) const;
        int GetFileName(_TCHAR *buf, DWORD bufLen) const;
        ULONGLONG GetFileSize() const;
        void GetFileTime(FILETIME *writeTm, FILETIME *createTm = NULL, FILETIME *accessTm = NULL) const;
        void TraverseSubEntries(SUBENTRY_CALLBACK seCallBack) const;
        const BOOL FindSubEntry(const _TCHAR *fileName, CIndexEntry &ieFound) const;
        const CAttrBase* FindStream(_TCHAR *name = NULL);
        BOOL IsDeleted() const;
        BOOL IsDirectory() const;
        BOOL IsReadOnly() const;
        BOOL IsHidden() const;
        BOOL IsSystem() const;
        BOOL IsCompressed() const;
        BOOL IsEncrypted() const;
        BOOL IsSparse() const;
        void printAHC(ATTR_HEADER_COMMON *ahc);
};
template struct NTSLIST_ENTRY<CFileRecord> ;
template class CSList<CFileRecord> ;
typedef CSList<CFileRecord> CFileRecordList;

#endif /* CFILERECORD_H_ */
