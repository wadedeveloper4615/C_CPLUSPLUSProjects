/*
 * CAttrNonResident.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CATTRNONRESIDENT_H_
#define CATTRNONRESIDENT_H_

#include "CAttrBase.h"
#include "NTFS.h"
#include "CSList.h"

class CAttrNonResident: public CAttrBase {
    public:
        CAttrNonResident(const AttrHeaderCommonPtr ahc, const CFileRecord *fr);
        virtual ~CAttrNonResident();
    protected:
        const ATTR_HEADER_NON_RESIDENT *AttrHeaderNR;
        CDataRunList DataRunList;
    private:
        BOOL bDataRunOK;
        BYTE *UnalignedBuf; // Buffer to hold not cluster aligned data
        BOOL PickData(const BYTE **dataRun, LONGLONG *length, LONGLONG *LCNOffset);
        BOOL ParseDataRun();
        BOOL ReadClusters(void *buf, DWORD clusters, LONGLONG lcn);
        BOOL ReadVirtualClusters(ULONGLONG vcn, DWORD clusters, void *bufv, DWORD bufLen, DWORD *actural);
    protected:
        virtual BOOL IsDataRunOK() const;
    public:
        virtual ULONGLONG GetDataSize(ULONGLONG *allocSize = NULL) const;
        virtual BOOL ReadData(const ULONGLONG &offset, void *bufv, DWORD bufLen, DWORD *actural) const;
};
template struct NTSLIST_ENTRY<CAttrNonResident> ;
template class CSList<CAttrNonResident> ;
#endif /* CATTRNONRESIDENT_H_ */
