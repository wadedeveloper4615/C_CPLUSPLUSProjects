/*
 * CAttrAttrListAttrNonResident.cpp
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */
#include "NTFS_Attribute.h"
#include "CFileRecord.h"
#include "CAttrNonResident.h"
#include "CAttrAttrListAttrNonResident.h"

CAttr_AttrList_AttrNonResident::CAttr_AttrList_AttrNonResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr) : CAttrNonResident(ahc, fr) {
    printf("Attribute: Attribute List\n");
    if (fr->FileReference == (ULONGLONG) -1)
        return;

    ULONGLONG offset = 0;
    DWORD len;
    ATTR_ATTRIBUTE_LIST alRecord;

    while (ReadData(offset, &alRecord, sizeof(ATTR_ATTRIBUTE_LIST), &len) && len == sizeof(ATTR_ATTRIBUTE_LIST)) {
        if (ATTR_INDEX(alRecord.AttrType) > ATTR_NUMS) {
            printf("Attribute List parse error1\n");
            break;
        }

        printf("Attribute List: 0x%04x\n", alRecord.AttrType);

        ULONGLONG recordRef = alRecord.BaseRef & 0x0000FFFFFFFFFFFFUL;
        if (recordRef != fr->FileReference) // Skip contained attributes
                {
            DWORD am = ATTR_MASK(alRecord.AttrType);
            if (am & fr->AttrMask)  // Skip unwanted attributes
                    {
                CFileRecord *frnew = new CFileRecord(fr->Volume);
                FileRecordList.InsertEntry(frnew);

                frnew->AttrMask = am;
                if (!frnew->ParseFileRecord(recordRef)) {
                    printf("Attribute List parse error2\n");
                    break;
                }
                frnew->ParseAttrs();

                // Insert new found AttrList to fr->AttrList
                const CAttrBase *ab = (CAttrBase*) frnew->FindFirstAttr(alRecord.AttrType);
                while (ab) {
                    CAttrList *al = (CAttrList*) &fr->AttrList[ATTR_INDEX(alRecord.AttrType)];
                    al->InsertEntry((CAttrBase*) ab);
                    ab = frnew->FindNextAttr(alRecord.AttrType);
                }

                // Throw away frnew->AttrList entries to prevent free twice (fr will delete them)
                frnew->AttrList[ATTR_INDEX(alRecord.AttrType)].ThrowAll();
            }
        }

        offset += alRecord.RecordSize;
    }
}

CAttr_AttrList_AttrNonResident::~CAttr_AttrList_AttrNonResident() {
}

