/*
 * CAttrResident.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CATTRRESIDENT_H_
#define CATTRRESIDENT_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CAttrBase.h"

class CAttrResident: public CAttrBase {
    public:
        CAttrResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttrResident();
    protected:
        const ATTR_HEADER_RESIDENT *AttrHeaderR;
        const void *AttrBody;   // Points to Resident Data
        DWORD AttrBodySize;     // Attribute Data Size

        virtual BOOL IsDataRunOK() const;

    public:
        virtual ULONGLONG GetDataSize(ULONGLONG *allocSize = NULL) const;
        virtual BOOL ReadData(const ULONGLONG &offset, void *bufv, DWORD bufLen, DWORD *actural) const;
};
template struct NTSLIST_ENTRY<CAttrResident> ;
template class CSList<CAttrResident> ;

#endif /* CATTRRESIDENT_H_ */
