/*
 * CIndexEntry.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CINDEXENTRY_H_
#define CINDEXENTRY_H_

class CIndexEntry {
    public:
        CIndexEntry();
        CIndexEntry(INDEX_ENTRY *ie);
        virtual ~CIndexEntry();
};

#endif /* CINDEXENTRY_H_ */
