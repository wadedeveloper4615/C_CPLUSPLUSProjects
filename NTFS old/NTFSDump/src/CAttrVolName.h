/*
 * CAttrVolName.h
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */

#ifndef CATTRVOLNAME_H_
#define CATTRVOLNAME_H_

#include "NTFS.h"
#include "CSList.h"
#include "CAttrResident.h"
#include "CFileRecord.h"

class CAttr_VolName: public CAttrResident {
    public:
        CAttr_VolName(const AttrHeaderCommonPtr ahc, const CFileRecord *fr);
        virtual ~CAttr_VolName();

    private:
        wchar_t *VolNameU;
        char *VolNameA;
        DWORD NameLength;

    public:
        int GetName(wchar_t *buf, DWORD len) const;
        int GetName(char *buf, DWORD len) const;
};
template struct NTSLIST_ENTRY<CAttr_VolName> ;
template class CSList<CAttr_VolName> ;
#endif /* CATTRVOLNAME_H_ */
