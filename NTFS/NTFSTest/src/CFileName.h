/*
 * CFileName.h
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */

#ifndef CFILENAME_H_
#define CFILENAME_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"

class CFileName {
    public:
        CFileName(ATTR_FILE_NAME *fn = NULL);
        virtual ~CFileName();

    protected:
        const ATTR_FILE_NAME *FileName; // May be NULL for an IndexEntry
        wchar_t *FileNameWUC;   // Uppercase Unicode File Name, used to compare file names
        int FileNameLength;
        BOOL IsCopy;
        void SetFileName(ATTR_FILE_NAME *fn);
        void CopyFileName(const CFileName *fn, const ATTR_FILE_NAME *afn);

    private:
        void GetFileNameWUC();

    public:
        int Compare(const wchar_t *fn) const;
        int Compare(const char *fn) const;

        ULONGLONG GetFileSize() const;
        DWORD GetFilePermission() const;
        BOOL IsReadOnly() const;
        BOOL IsHidden() const;
        BOOL IsSystem() const;
        BOOL IsDirectory() const;
        BOOL IsCompressed() const;
        BOOL IsEncrypted() const;
        BOOL IsSparse() const;

        int GetFileName(char *buf, DWORD bufLen) const;
        int GetFileName(wchar_t *buf, DWORD bufLen) const;
        BOOL HasName() const;
        BOOL IsWin32Name() const;

        void GetFileTime(FILETIME *writeTm, FILETIME *createTm = NULL, FILETIME *accessTm = NULL) const;
};
template struct NTSLIST_ENTRY<CFileName> ;
template class CSList<CFileName> ;

#endif /* CFILENAME_H_ */
