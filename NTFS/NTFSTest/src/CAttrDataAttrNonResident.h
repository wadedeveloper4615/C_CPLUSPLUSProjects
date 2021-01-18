/*
 * CAttrDataAttrNonResident.h
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */

#ifndef CATTRDATAATTRNONRESIDENT_H_
#define CATTRDATAATTRNONRESIDENT_H_

#include "CSList.h"
#include "CAttrNonResident.h"
class CFileRecord;

class CAttr_Data_AttrNonResident: public CAttrNonResident {
    public:
        CAttr_Data_AttrNonResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttr_Data_AttrNonResident();
};
template struct NTSLIST_ENTRY<CAttr_Data_AttrNonResident> ;
template class CSList<CAttr_Data_AttrNonResident> ;
#endif /* CATTRDATAATTRNONRESIDENT_H_ */
