/*
 * CAttrAttrListAttrResident.h
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */

#ifndef CATTRATTRLISTATTRRESIDENT_H_
#define CATTRATTRLISTATTRRESIDENT_H_

#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CAttrResident.h"
class CFileRecord;

class CAttr_AttrList_AttrResident: public CAttrResident {
    public:
        CAttr_AttrList_AttrResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttr_AttrList_AttrResident();
    private:
        CFileRecordList FileRecordList;
};
template struct NTSLIST_ENTRY<CAttr_AttrList_AttrResident> ;
template class CSList<CAttr_AttrList_AttrResident> ;
#endif /* CATTRATTRLISTATTRRESIDENT_H_ */
