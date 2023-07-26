#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include "main.h"

#pragma pack(push, 1)
typedef struct {
	BYTE Padding1[3];
	ULONGLONG NTFSSign;
	WORD BytesPerSector;
	BYTE SectorsPerCluster;
	BYTE Padding2[0x1A];
	ULONGLONG SectorsCount;
	ULONGLONG MFTinitCluster;
	ULONGLONG DataAttrMFTCopy;
	BYTE MTFSize;
	BYTE Padding3[3];
	BYTE IndexSize;
	BYTE Padding4[0x1B9];
	WORD Signature;
} NTFS_BootRecord;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
	BYTE Padding1[3];
	ULONGLONG OEM_Name;
	WORD BytesPerSector;
	BYTE SectorsPerCluster;
	WORD ReservedArea;
	BYTE CopyFATCount;
	WORD NumberOfFilesInRoot;
	BYTE Padding2[0x03];
	WORD SizeofCopyFAT;
	BYTE Padding3[0x08];
	DWORD SectorsCount;
	BYTE Padding4[0x12];
	ULONGLONG FAT16Sign;
	BYTE Padding5[0x1C0];
	WORD Signature;
} FAT16_BootRecord;
#pragma pack(pop)

class FileSystem
{
protected:
	HANDLE fileHandle;
	BYTE* BootSector;
	unsigned __int64 BytesToSkip;
public:
	virtual unsigned __int64 ClustersCount() const = 0;
	virtual unsigned int ClusterSize() const = 0;
	virtual unsigned __int64 FileSystemSize() const = 0;
	std::vector<BYTE> ReadCluster(unsigned int ClSize, unsigned __int64 ClusterNumber, unsigned __int64 ReadClusterCount, ULONGLONG FileSystemSignature);
	BYTE* GetBootSector();
	FileSystem(UnicodeString letter);
	~FileSystem();
};

class NTFS : public FileSystem
{
public:
	NTFS(UnicodeString letter);
	unsigned __int64 ClustersCount() const;
	unsigned int ClusterSize() const;
	unsigned __int64 FileSystemSize() const;
};

class FAT16 : public FileSystem
{
public:
	FAT16(UnicodeString letter);
	unsigned __int64 ClustersCount() const;
	unsigned int ClusterSize() const;
	unsigned __int64 FileSystemSize() const;
};

BYTE* FileSystem::GetBootSector() {
	return this->BootSector;
};

ULONGLONG GetFileSystemType(UnicodeString letter);
FileSystem* CreateFileSystem(ULONGLONG FileSystemType, UnicodeString letter);

class Iterator
{
protected:
	FileSystem* FS;
	ULONGLONG FStype;
	unsigned __int64 index;
public:
	virtual void begin() = 0;
	virtual unsigned __int64 currentindex() = 0;
	virtual unsigned __int64 end() = 0;
	virtual void next() = 0;
	virtual std::vector<BYTE> GetCurrentCluster() = 0;
};

class ConcreteIterator : public Iterator
{
protected:
public:
	ConcreteIterator(FileSystem* fs, ULONGLONG fst) {
		FS = fs;
		FStype = fst;
	}
	void begin() {
		if (FStype == 0x2020203631544146) index = 2;
		else index = 0;
	}
	unsigned __int64 currentindex() {
		return index;
	}
	unsigned __int64 end() {
		if (FStype == 0x2020203631544146) return FS->ClustersCount() + 1;
		else return FS->ClustersCount();
	}
	void next() {
		index++;
	}
	std::vector<BYTE> GetCurrentCluster() {
		return FS->ReadCluster(FS->ClusterSize(), index, 1, FStype);
	}
};

class Decorator : public Iterator
{
protected:
	Iterator* iteratorObj;
public:
	Decorator(Iterator* iterator) {
		iteratorObj = iterator;
	}
	void begin() {
		iteratorObj->begin();
	}
	unsigned __int64 currentindex() {
		return iteratorObj->currentindex();
	}
	unsigned __int64 end() {
		return iteratorObj->end();
	}
	void next() {
		iteratorObj->next();
	}
	std::vector<BYTE> GetCurrentCluster() {
		return iteratorObj->GetCurrentCluster();
	}
};

class ConcreteDecorator : public Decorator
{
protected:
public:
	ConcreteDecorator(Iterator* iterator) : Decorator(iterator) {};
	void begin() {
		Decorator::begin();
	}
	unsigned __int64 currentindex() {
		return Decorator::currentindex();
	}
	unsigned __int64 end() {
		return Decorator::end();
	}
	void next() {
		Decorator::next();
	}
	std::vector<BYTE> GetCurrentCluster() {
		bool flag = true;
		std::vector<BYTE> buf = Decorator::GetCurrentCluster();
		for (unsigned __int64 i = 0; i < buf.size(); i++) {
			if (buf[i] != 0x00) {
				flag = false;
				break;
			}
		}
		if (flag == false) return buf;
		else {
			std::vector<BYTE> emptybuf;
			return emptybuf;
		}
	}
};
#pragma pack(push, 1)
typedef struct {
	BYTE Signature[12];
} FileSignature;
#pragma pack(pop)

std::string GetFileSignature(std::vector<BYTE> ReadCluster);
