//**********************************************************
//** Deduce copyright (c) 2020 Christopher D. Wade        **
//** File: format.h                                       **
//**********************************************************
#ifndef FORMAT_H_
#define FORMAT_H_

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <locale.h>
#include <tchar.h>
#include <shellapi.h>
#include "List.h"

#define LO(x)   ((x) & 0xFF)
#define HI(x)   (((x)>>8) & 0xFF)

#pragma pack(push, 1)
typedef struct _PartitionEntry {
        BYTE bootType;
        BYTE beginHead;
        WORD beginSectorCylinder;
        BYTE fileSystem;
        BYTE endHead;
        WORD endSectorCylinder;
        DWORD lbaStart;
        DWORD partitionSize;
} PartitionEntry, *PartitionEntryPtr;

typedef struct _MasterBootRecord {
        BYTE initialProgramLoader[446];
        PartitionEntry partionTable[4];
        WORD signature;
} MasterBootRecord, *MasterBootRecordPtr;

typedef struct _NTFSBootSector {
        BYTE Jmp[3];
        BYTE Signature[8];
        WORD BytesPerSector;
        BYTE SectorsPerCluster;
        WORD ReservedSectors;
        BYTE Zeros1[3];
        WORD NotUsed1;
        BYTE MediaDescriptor;
        WORD Zeros2;
        WORD SectorsPerTrack;
        WORD NumberOfHeads;
        DWORD HiddenSectors;
        DWORD NotUsed2;
        DWORD NotUsed3;
        ULONGLONG TotalSectors;
        ULONGLONG LCN_MFT;
        ULONGLONG LCN_MFTMirr;
        DWORD ClustersPerFileRecord;
        DWORD ClustersPerIndexBlock;
        BYTE VolumeSN[8];
        BYTE Code[430];
        WORD endMarker;
} NTFSBootSector, *NTFSBootSectorPtr, **NTFSBootSectorPtrPtr;

typedef struct _FAT32BootSector {
        BYTE jump[3];
        BYTE oemID[8];
        WORD bytePerSector;
        BYTE sectorPerCluster;
        WORD reserved1;
        BYTE numberOfFATS;
        WORD maxRootDirEntries;
        WORD totalSectors16;
        BYTE bMedia;
        WORD totalSectors32;
        WORD sectorPerTrack;
        WORD headNumber;
        DWORD hiddenSector;
        DWORD totalSectors;
        DWORD sectorsPerFAT;
        WORD flags;
        WORD fat32Version;
        DWORD rootCluster;
        WORD fsInfoCluster;
        WORD backupBootSector;
        BYTE Reserved2[12];
        BYTE driveNumber;
        BYTE Reserved1;
        BYTE extendedSignature;
        DWORD dBS_VolID;
        BYTE volumeName[11];
        BYTE FATName[8];
        BYTE code[420];
        WORD endMarker;
} FAT32BootSector, *FAT32BootSectorPtr;

typedef struct _RecordHeader {
        BYTE signature[4];
        WORD updateOffset;
        WORD updateNumber;
        LONGLONG logFile;
        WORD sequenceNumber;
        WORD hardLinkCount;
        WORD attributeOffset;
        WORD flag;
        DWORD usedSize;
        DWORD allocatedSize;
        LONGLONG baseRecord;
        WORD nextAttributeID;
        BYTE unsed[2];
        DWORD MFTRecord;
} RecordHeader, *RecordHeaderPtr;

typedef struct _AttributeHeaderR {
        DWORD typeID;
        DWORD length;
        BYTE formCode;
        BYTE nameLength;
        WORD nameOffset;
        WORD flag;
        WORD attributeID;
        DWORD contentLength;
        WORD contentOffset;
        WORD unused;
} AttributeHeaderR, *AttributeHeaderRPtr;

typedef struct _AttributeHeaderNR {
        DWORD typeID;
        DWORD length;
        BYTE formCode;
        BYTE nameLength;
        WORD nameOffset;
        WORD flag;
        WORD attributeID;
        LONGLONG startVCN;
        LONGLONG endVCN;
        WORD runListOffset;
        WORD compressSize;
        DWORD zero;
        LONGLONG contentDiskSize;
        LONGLONG contentSize;
        LONGLONG initialContentSize;
} AttributeHeaderNR, *AttributeHeaderNRPtr;

typedef struct _FileName {
        LONGLONG parentDirectory;
        LONGLONG dateCreated;
        LONGLONG dateModified;
        LONGLONG dateMFTModified;
        LONGLONG dateAccessed;
        LONGLONG logicalSize;
        LONGLONG diskSize;
        DWORD flag;
        DWORD reparseValue;
        BYTE nameLength;
        BYTE nameType;
        BYTE name[1];
} FileName, *FileNamePtr;

typedef struct _AttributeRecord {
        DWORD typeID;
        WORD recordLength;
        BYTE nameLength;
        BYTE nameOffset;
        LONGLONG lowestVCN;
        LONGLONG recordNumber;
        WORD sequenceNumber;
        WORD reserved;
} AttributeRecord, *AttributeRecordPtr;
#pragma pack(pop)

#endif /* FORMAT_H_ */
