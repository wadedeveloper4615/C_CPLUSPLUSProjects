/*
 * CAttrIndexAlloc.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CATTRINDEXALLOC_H_
#define CATTRINDEXALLOC_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CAttrNonResident.h"
class CIndexBlock;

class CAttr_IndexAlloc: public CAttrNonResident {

    public:
        CAttr_IndexAlloc(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttr_IndexAlloc();

    private:
        ULONGLONG IndexBlockCount;

        BOOL PatchUS(WORD *sector, int sectors, WORD usn, WORD *usarray);

    public:
        ULONGLONG GetIndexBlockCount();
        BOOL ParseIndexBlock(const ULONGLONG &vcn, CIndexBlock &ibClass);
};
template struct NTSLIST_ENTRY<CAttr_IndexAlloc> ;
template class CSList<CAttr_IndexAlloc> ;

#endif /* CATTRINDEXALLOC_H_ */
