/*
 * CAttrIndexRoot.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CATTRINDEXROOT_H_
#define CATTRINDEXROOT_H_

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
// CAttr_IndexRoot

#endif /* CATTRINDEXROOT_H_ */
