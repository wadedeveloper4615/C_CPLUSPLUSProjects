/*
 * CNTFSVolume.h
 *
 *  Created on: Nov 5, 2020
 *      Author: wade4
 */

#ifndef CNTFSVOLUME_H_
#define CNTFSVOLUME_H_

#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CAttrBase.h"
#include "CFileRecord.h"

class CNTFSVolume {
        friend class CFileRecord;
        friend class CAttrBase;

        WORD SectorSize;
        DWORD ClusterSize;
        DWORD FileRecordSize;
        DWORD IndexBlockSize;
        ULONGLONG MFTAddr;
        HANDLE VolumeHandle;
        BOOL VolumeOK;
        ATTR_RAW_CALLBACK AttrRawCallBack[ATTR_NUMS];
        WORD Version;
        CFileRecord *MFTRecord;     // $MFT File Record
        const CAttrBase *MFTData;   // $MFT Data Attribute
        BOOL OpenVolume(_TCHAR volume);

    public:
        CNTFSVolume(_TCHAR volume);
        virtual ~CNTFSVolume();
        BOOL IsVolumeOK() const;
        WORD GetVersion() const;
        ULONGLONG GetRecordsCount() const;
        DWORD GetSectorSize() const;
        DWORD GetClusterSize() const;
        DWORD GetFileRecordSize() const;
        DWORD GetIndexBlockSize() const;
        ULONGLONG GetMFTAddr() const;
        BOOL InstallAttrRawCB(DWORD attrType, ATTR_RAW_CALLBACK cb);
        void ClearAttrRawCB();
};
template struct NTSLIST_ENTRY<CNTFSVolume> ;
template class CSList<CNTFSVolume> ;

#endif /* CNTFSVOLUME_H_ */
