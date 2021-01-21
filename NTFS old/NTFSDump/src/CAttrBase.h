/*
 * CAttrBase2.h
 *
 *  Created on: Nov 23, 2020
 *      Author: wade4
 */

#ifndef CATTRBASE_H_
#define CATTRBASE_H_

class CFileRecord;
#include "NTFS.h"
#include "CSList.h"

class CAttrBase {
        AttrHeaderCommonPtr AttrHeader;
        WORD sectorSize;
        DWORD clusterSize;
        DWORD indexBlockSize;
        HANDLE volumeHandle;
        const CFileRecord *fileRecord;
    public:
        CAttrBase(const AttrHeaderCommonPtr attributeHeader, const CFileRecord *fileRecord);
        virtual ~CAttrBase();
        const AttrHeaderCommonPtr GetAttrHeader() const;
        DWORD GetAttrType() const;
        DWORD GetAttrTotalSize() const;
        BOOL IsNonResident() const;
        WORD GetAttrFlags() const;
        int GetAttrName(char *buf, DWORD bufLen) const;
        int GetAttrName(wchar_t *buf, DWORD bufLen) const;
        BOOL IsUnNamed() const;
        const AttrHeaderCommonPtr getAttrHeader() const;
        WORD getSectorSize() const;
        DWORD getClusterSize() const;
        DWORD getIndexBlockSize() const;
        HANDLE getVolumeHandle() const;
        const CFileRecord* getFileRecord() const;

    protected:
        virtual BOOL IsDataRunOK() const = 0;
    public:
        virtual ULONGLONG GetDataSize(ULONGLONG *allocSize = NULL) const = 0;
        virtual BOOL ReadData(const ULONGLONG &offset, void *bufv, DWORD bufLen, DWORD *actural) const = 0;
};
template struct NTSLIST_ENTRY<CAttrBase> ;
template class CSList<CAttrBase> ;
typedef class CSList<CAttrBase> CAttrList;

#endif /* CATTRBASE_H_ */
