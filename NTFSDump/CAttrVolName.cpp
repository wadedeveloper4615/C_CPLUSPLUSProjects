/*
 * CAttrVolName.cpp
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */
#include "NTFS.h"
#include "CNTFSVolume.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CAttrStdInfo.h"
#include "CAttrVolInfo.h"
#include "CAttrVolName.h"
#include "CFileRecord.h"
#include "CAttrVolName.h"

CAttr_VolName::CAttr_VolName(const AttrHeaderCommonPtr ahc, const CFileRecord *fr) : CAttrResident(ahc, fr) {
    printf("Attribute: Volume Name\n");

    NameLength = AttrBodySize >> 1;
    VolNameU = new wchar_t[NameLength + 1];
    VolNameA = new char[NameLength + 1];

    memcpy(VolNameU, AttrBody, AttrBodySize);
    VolNameU[NameLength] = wchar_t('\0');

    int len = WideCharToMultiByte(CP_ACP, 0, VolNameU, NameLength, VolNameA, NameLength, NULL, NULL);
    VolNameA[NameLength] = '\0';
}

CAttr_VolName::~CAttr_VolName() {
    delete VolNameU;
    delete VolNameA;
}

int CAttr_VolName::GetName(wchar_t *buf, DWORD len) const {
    if (len < NameLength)
        return -1 * NameLength;   // buffer too small
    wcsncpy(buf, VolNameU, NameLength + 1);
    return NameLength;
}

// ANSI Name
int CAttr_VolName::GetName(char *buf, DWORD len) const {
    if (len < NameLength)
        return -1 * NameLength;   // buffer too small

    strncpy(buf, VolNameA, NameLength + 1);
    return NameLength;
}

