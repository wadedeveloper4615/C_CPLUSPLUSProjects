/*
 * CAttrDataAttrResident.h
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */

#ifndef CATTRDATAATTRRESIDENT_H_
#define CATTRDATAATTRRESIDENT_H_

#include "CSList.h"
#include "CAttrResident.h"
class CFileRecord;

class CAttr_Data_AttrResident: public CAttrResident {
    public:
        CAttr_Data_AttrResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttr_Data_AttrResident();
};
template struct NTSLIST_ENTRY<CAttr_Data_AttrResident> ;
template class CSList<CAttr_Data_AttrResident> ;
#endif /* CATTRDATAATTRRESIDENT_H_ */
