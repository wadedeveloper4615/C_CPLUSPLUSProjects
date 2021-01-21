/*
 * CAttrAttrListAttrNonResident.h
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */

#ifndef CATTRATTRLISTATTRNONRESIDENT_H_
#define CATTRATTRLISTATTRNONRESIDENT_H_

#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CAttrNonResident.h"
class CFileRecord;

class CAttr_AttrList_AttrNonResident: public CAttrNonResident {
    public:
        CAttr_AttrList_AttrNonResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttr_AttrList_AttrNonResident();
    private:
        CFileRecordList FileRecordList;
};
template struct NTSLIST_ENTRY<CAttr_AttrList_AttrNonResident> ;
template class CSList<CAttr_AttrList_AttrNonResident> ;
#endif /* CATTRATTRLISTATTRNONRESIDENT_H_ */
