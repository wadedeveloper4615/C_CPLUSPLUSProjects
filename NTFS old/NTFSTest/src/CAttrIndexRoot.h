/*
 * CAttrIndexRoot.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CATTRINDEXROOT_H_
#define CATTRINDEXROOT_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CIndexEntry.h"
#include "CAttrResident.h"

class CAttr_IndexRoot: public CAttrResident, public CIndexEntryList {

    public:
        CAttr_IndexRoot(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttr_IndexRoot();

    private:
        const ATTR_INDEX_ROOT *IndexRoot;

        void ParseIndexEntries();

    public:
        BOOL IsFileName() const;
};
template struct NTSLIST_ENTRY<CAttr_IndexRoot> ;
template class CSList<CAttr_IndexRoot> ;

#endif /* CATTRINDEXROOT_H_ */
