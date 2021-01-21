/*
 * CAttrBase.h
 *
 *  Created on: Nov 5, 2020
 *      Author: wade4
 */

#ifndef CATTRBASE_H_
#define CATTRBASE_H_

class CFileRecord;
#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"

class CAttrBase {
    public:
        CAttrBase(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttrBase();
    protected:
        const ATTR_HEADER_COMMON *AttrHeader;
        WORD _SectorSize;
        DWORD _ClusterSize;
        DWORD _IndexBlockSize;
        HANDLE _hVolume;
        const CFileRecord *FileRecord;
    public:
        const ATTR_HEADER_COMMON* GetAttrHeader() const;
        DWORD GetAttrType() const;
        DWORD GetAttrTotalSize() const;
        BOOL IsNonResident() const;
        WORD GetAttrFlags() const;
        int GetAttrName(char *buf, DWORD bufLen) const;
        int GetAttrName(wchar_t *buf, DWORD bufLen) const;
        BOOL IsUnNamed() const;

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
