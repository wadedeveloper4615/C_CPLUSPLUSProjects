/*
 * CFileName.cpp
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */
#include "NTFS_Attribute.h"
#include "CNTFSVolume.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CAttrIndexAlloc.h"
#include "CIndexBlock.h"
#include "CIndexEntry.h"
#include "CFileRecord.h"
#include "CAttrStdInfo.h"
#include "CFileName.h"

CFileName::CFileName(ATTR_FILE_NAME *fn) {
    IsCopy = FALSE;

    FileName = fn;

    FileNameWUC = NULL;
    FileNameLength = 0;

    if (fn)
        GetFileNameWUC();
}

CFileName::~CFileName() {
    if (FileNameWUC)
        delete FileNameWUC;
}

void CFileName::SetFileName(ATTR_FILE_NAME *fn) {
    FileName = fn;

    GetFileNameWUC();
}

// Copy pointer buffers
void CFileName::CopyFileName(const CFileName *fn, const ATTR_FILE_NAME *afn) {
    if (!IsCopy) {
        printf("Cannot call this routine\n");
        return;
    }

    _ASSERT(fn && afn);

    printf("FileName Copied\n");

    if (FileNameWUC)
        delete FileNameWUC;

    FileNameLength = fn->FileNameLength;
    FileName = afn;

    if (fn->FileNameWUC) {
        FileNameWUC = new wchar_t[FileNameLength + 1];
        wcsncpy(FileNameWUC, fn->FileNameWUC, FileNameLength);
        FileNameWUC[FileNameLength] = wchar_t('\0');
    } else
        FileNameWUC = NULL;
}

// Get uppercase unicode filename and store it in a buffer
void CFileName::GetFileNameWUC() {
#ifdef  _DEBUG
    char fna[MAX_PATH];
    GetFileName(fna, MAX_PATH); // Just show filename in debug window
#endif

    if (FileNameWUC) {
        delete FileNameWUC;
        FileNameWUC = NULL;
        FileNameLength = 0;
    }

    wchar_t fns[MAX_PATH];
    FileNameLength = GetFileName(fns, MAX_PATH);

    if (FileNameLength > 0) {
        FileNameWUC = new wchar_t[FileNameLength + 1];
        for (int i = 0; i < FileNameLength; i++)
            FileNameWUC[i] = towupper(fns[i]);
        FileNameWUC[FileNameLength] = wchar_t('\0');
    } else {
        FileNameLength = 0;
        FileNameWUC = NULL;
    }
}

// Compare Unicode file name
int CFileName::Compare(const wchar_t *fn) const {
    // Change fn to upper case
    int len = wcslen(fn);
    if (len > MAX_PATH)
        return 1;   // Assume bigger

    wchar_t fns[MAX_PATH];

    for (int i = 0; i < len; i++)
        fns[i] = towupper(fn[i]);
    fns[len] = wchar_t('\0');

    return wcscmp(fns, FileNameWUC);
}

// Compare ANSI file name
int CFileName::Compare(const char *fn) const {
    wchar_t fnw[MAX_PATH];

    int len = MultiByteToWideChar(CP_ACP, 0, fn, -1, fnw, MAX_PATH);
    if (len)
        return Compare(fnw);
    else
        return 1;   // Assume bigger
}

ULONGLONG CFileName::GetFileSize() const {
    return FileName ? FileName->RealSize : 0;
}

DWORD CFileName::GetFilePermission() const {
    return FileName ? FileName->Flags : 0;
}

BOOL CFileName::IsReadOnly() const {
    return FileName ? ((FileName->Flags) & ATTR_FILENAME_FLAG_READONLY) : FALSE;
}

BOOL CFileName::IsHidden() const {
    return FileName ? ((FileName->Flags) & ATTR_FILENAME_FLAG_HIDDEN) : FALSE;
}

BOOL CFileName::IsSystem() const {
    return FileName ? ((FileName->Flags) & ATTR_FILENAME_FLAG_SYSTEM) : FALSE;
}

BOOL CFileName::IsDirectory() const {
    return FileName ? ((FileName->Flags) & ATTR_FILENAME_FLAG_DIRECTORY) : FALSE;
}

BOOL CFileName::IsCompressed() const {
    return FileName ? ((FileName->Flags) & ATTR_FILENAME_FLAG_COMPRESSED) : FALSE;
}

BOOL CFileName::IsEncrypted() const {
    return FileName ? ((FileName->Flags) & ATTR_FILENAME_FLAG_ENCRYPTED) : FALSE;
}

BOOL CFileName::IsSparse() const {
    return FileName ? ((FileName->Flags) & ATTR_FILENAME_FLAG_SPARSE) : FALSE;
}

// Get ANSI File Name
// Return 0: Unnamed, <0: buffer too small, -buffersize, >0 Name length
int CFileName::GetFileName(char *buf, DWORD bufLen) const {
    if (FileName == NULL)
        return 0;

    int len = 0;

    if (FileName->NameLength) {
        if (bufLen < FileName->NameLength)
            return -1 * FileName->NameLength; // buffer too small

        len = WideCharToMultiByte(CP_ACP, 0, (wchar_t*) FileName->Name, FileName->NameLength, buf, bufLen, NULL, NULL);
        if (len) {
            buf[len] = '\0';
            //printf("File Name: %s\n", buf);
            //printf("File Permission: %s\t%c%c%c\n", IsDirectory() ? "Directory" : "File", IsReadOnly() ? 'R' : ' ', IsHidden() ? 'H' : ' ', IsSystem() ? 'S' : ' ');
        } else {
            printf("Unrecognized File Name or FileName buffer too small\n");
        }
    }

    return len;
}

// Get Unicode File Name
// Return 0: Unnamed, <0: buffer too small, -buffersize, >0 Name length
int CFileName::GetFileName(wchar_t *buf, DWORD bufLen) const {
    if (FileName == NULL)
        return 0;

    if (FileName->NameLength) {
        if (bufLen < FileName->NameLength)
            return -1 * FileName->NameLength; // buffer too small

        bufLen = FileName->NameLength;
        wcsncpy(buf, (wchar_t*) FileName->Name, bufLen);
        buf[bufLen] = wchar_t('\0');

        return bufLen;
    }

    return 0;
}

BOOL CFileName::HasName() const {
    return FileNameLength > 0;
}

BOOL CFileName::IsWin32Name() const {
    if (FileName == NULL || FileNameLength <= 0)
        return FALSE;

    return (FileName->NameSpace != ATTR_FILENAME_NAMESPACE_DOS);    // POSIX, WIN32, WIN32_DOS
}

// Change from UTC time to local time
void CFileName::GetFileTime(FILETIME *writeTm, FILETIME *createTm, FILETIME *accessTm) const {
    CAttr_StdInfo::UTC2Local(FileName ? FileName->AlterTime : 0, writeTm);

    if (createTm)
        CAttr_StdInfo::UTC2Local(FileName ? FileName->CreateTime : 0, createTm);

    if (accessTm)
        CAttr_StdInfo::UTC2Local(FileName ? FileName->ReadTime : 0, accessTm);
}
