/*
 * CAttrStdInfo.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CATTRSTDINFO_H_
#define CATTRSTDINFO_H_

#include "NTFS.h"
#include "CSList.h"
#include "CAttrResident.h"

class CAttr_StdInfo: public CAttrResident {
    public:
        CAttr_StdInfo(const AttrHeaderCommonPtr ahc, const CFileRecord *fr);
        virtual ~CAttr_StdInfo();
    private:
        const ATTR_STANDARD_INFORMATION *StdInfo;

    public:
        void GetFileTime(FILETIME *writeTm, FILETIME *createTm = NULL, FILETIME *accessTm = NULL) const;
        DWORD GetFilePermission() const;
        BOOL IsReadOnly() const;
        BOOL IsHidden() const;
        BOOL IsSystem() const;
        BOOL IsCompressed() const;
        BOOL IsEncrypted() const;
        BOOL IsSparse() const;

        static void UTC2Local(const ULONGLONG &ultm, FILETIME *lftm);
};
template struct NTSLIST_ENTRY<CAttr_StdInfo> ;
template class CSList<CAttr_StdInfo> ;

#endif /* CATTRSTDINFO_H_ */
