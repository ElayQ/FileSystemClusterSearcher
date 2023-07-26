#include "FileSystem.h"
int flag = 0;
UnicodeString Error;

std::vector<BYTE> FileSystem::ReadCluster(unsigned int ClSize, unsigned __int64 ClusterNumber, unsigned __int64 ReadClusterCount, ULONGLONG FileSystemSignature) {
    BYTE* outBuffer = new BYTE[ClSize * ReadClusterCount];
    LARGE_INTEGER sectorOffset;
    if (FileSystemSignature == 0x202020205346544E) {
        sectorOffset.QuadPart = ClSize * ClusterNumber; // Область данных с первого сектора диска
        unsigned long currentPosition = SetFilePointer(
            fileHandle,
            sectorOffset.LowPart,
            &sectorOffset.HighPart,
			FILE_BEGIN
		);
		if (currentPosition != sectorOffset.LowPart) {
			Error = L"Error with Offset when read current cluster";
			flag = 1;
		}
	}
	else if (FileSystemSignature == 0x2020203631544146) { // FAT16
		sectorOffset.QuadPart = BytesToSkip + ClSize * (ClusterNumber - 2); // Начало области данных FAT16
        unsigned long currentPosition = SetFilePointer(
            fileHandle,
			sectorOffset.LowPart,
            &sectorOffset.HighPart,
			FILE_BEGIN
        );
		if (currentPosition != sectorOffset.LowPart) {
			Error = L"Error with Offset when read current cluster";
			flag = 1;
		}
	}
	DWORD bytesToRead = ClSize * ReadClusterCount;
    DWORD bytesRead;
	bool readResult = ReadFile(
        fileHandle,
        outBuffer,
        bytesToRead,
        &bytesRead,
        NULL
    );
	if (readResult == false || bytesRead != bytesToRead) {
		if (flag != 1) {
			flag = 2;
			Error = L"Unable to read file/Mismatch in number of bytes read (ReadCluster)";
		}
	}
    std::vector<BYTE> ptr;
	for (unsigned __int64 i = 0; i < ClSize * ReadClusterCount; i++) {
		ptr.push_back(outBuffer[i]);
    }
    delete[] outBuffer;
    return ptr;
}

FileSystem::FileSystem(UnicodeString letter) {
    BootSector = new BYTE[512];
	UnicodeString diskname = "\\\\.\\";
	diskname += letter.c_str();
	diskname += ":";
    fileHandle = CreateFileW(
		diskname.c_str(),
		GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
		OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
	if (fileHandle == INVALID_HANDLE_VALUE) {
		Application->MessageBox(L"Unable to open disk (Don't forget: Administrator privileges are required)", L"Error", MB_OK);
		exit(1);
	}
	LARGE_INTEGER sectorOffset;
    sectorOffset.QuadPart = 0;
    unsigned long currentPosition = SetFilePointer(
        fileHandle,
        sectorOffset.LowPart,
		&sectorOffset.HighPart,
        FILE_BEGIN
    );
	if (currentPosition != sectorOffset.LowPart) {
		Application->MessageBox(L"Error with Offset when read boot sector", L"Error", MB_OK);
		exit(1);
	}
    DWORD bytesToRead = 512;
	DWORD bytesRead;
    bool readResult = ReadFile(
        fileHandle,
        BootSector,
        bytesToRead,
        &bytesRead,
        NULL
    );
	if (readResult == false || bytesRead != bytesToRead) {
		Application->MessageBox(L"Unable to read file/Mismatch in number of bytes read (Boot read)", L"Error", MB_OK);
		exit(1);
    }

    // Для FAT16 сразу определяем по загрузочной записи количество байт, которые нужно пропустить, чтобы дойти до области данных
    FAT16_BootRecord* pFAT16_BootRecord = reinterpret_cast<FAT16_BootRecord*>(BootSector);
    if (pFAT16_BootRecord->FAT16Sign == 0x2020203631544146) {
		WORD SizeofCopyFAT = pFAT16_BootRecord->SizeofCopyFAT;
		BYTE CopyFATCount = pFAT16_BootRecord->CopyFATCount;
		WORD ReservedArea = pFAT16_BootRecord->ReservedArea;
		BytesToSkip = ((SizeofCopyFAT * CopyFATCount + ReservedArea) + 32) * 512;
    }
	else {
		BytesToSkip = 0;
	}
}

FileSystem::~FileSystem()
{
    if (!BootSector) delete[] BootSector;
	CloseHandle(fileHandle);
}

NTFS::NTFS(UnicodeString letter) : FileSystem(letter) {};
FAT16::FAT16(UnicodeString letter) : FileSystem(letter) {};

unsigned __int64 NTFS::ClustersCount() const {
    NTFS_BootRecord* pNTFS_BootRecord = reinterpret_cast<NTFS_BootRecord*>(BootSector);
    BYTE SectorsPerCluster = pNTFS_BootRecord->SectorsPerCluster;
    ULONGLONG SectorsCount = pNTFS_BootRecord->SectorsCount;
    return SectorsCount / SectorsPerCluster;
}
unsigned int NTFS::ClusterSize() const {
    NTFS_BootRecord* pNTFS_BootRecord = reinterpret_cast<NTFS_BootRecord*>(BootSector);
    WORD BytesPerSector = pNTFS_BootRecord->BytesPerSector;
    BYTE SectorsPerCluster = pNTFS_BootRecord->SectorsPerCluster;
    return BytesPerSector * SectorsPerCluster;
}
unsigned __int64 NTFS::FileSystemSize() const {
    return ClusterSize() * ClustersCount();
}


unsigned __int64 FAT16::ClustersCount() const {
    FAT16_BootRecord* pFAT16_BootRecord = reinterpret_cast<FAT16_BootRecord*>(BootSector);
    DWORD SectorsCount = pFAT16_BootRecord->SectorsCount;
    WORD SizeofCopyFAT = pFAT16_BootRecord->SizeofCopyFAT;
    BYTE CopyFATCount = pFAT16_BootRecord->CopyFATCount;
    WORD ReservedArea = pFAT16_BootRecord->ReservedArea;
    BYTE SectorsPerCluster = pFAT16_BootRecord->SectorsPerCluster;
    return ((SectorsCount - ((SizeofCopyFAT * CopyFATCount + ReservedArea) + 32)) / SectorsPerCluster);
}
unsigned int FAT16::ClusterSize() const {
    FAT16_BootRecord* pFAT16_BootRecord = reinterpret_cast<FAT16_BootRecord*>(BootSector);
    WORD BytesPerSector = pFAT16_BootRecord->BytesPerSector;
    BYTE SectorsPerCluster = pFAT16_BootRecord->SectorsPerCluster;
    return BytesPerSector * SectorsPerCluster;
}
unsigned __int64 FAT16::FileSystemSize() const {
    FAT16_BootRecord* pFAT16_BootRecord = reinterpret_cast<FAT16_BootRecord*>(BootSector);
    DWORD SectorsCount = pFAT16_BootRecord->SectorsCount;
    WORD BytesPerSector = pFAT16_BootRecord->BytesPerSector;
    return SectorsCount * BytesPerSector;
}

ULONGLONG GetFileSystemType(UnicodeString letter) {
	FileSystem* loadboot = new NTFS(letter);
	BYTE* BootSector = loadboot->GetBootSector();
	NTFS_BootRecord* pNTFS_BootRecord = reinterpret_cast<NTFS_BootRecord*>(BootSector);
	FAT16_BootRecord* pFAT16_BootRecord = reinterpret_cast<FAT16_BootRecord*>(BootSector);
	if (pNTFS_BootRecord->NTFSSign == 0x202020205346544E) return pNTFS_BootRecord->NTFSSign;
	else if (pFAT16_BootRecord->FAT16Sign == 0x2020203631544146) return pFAT16_BootRecord->FAT16Sign;
	else {
		Application->MessageBox(L"The file system of your disk is not NTFS or FAT16/32", L"Error", MB_OK);
		exit(1);
	}
}

FileSystem* CreateFileSystem(ULONGLONG FileSystemType, UnicodeString letter)
{
	switch (FileSystemType)
	{
	case 0x202020205346544E:
		Form1->Label8->Caption += "NTFS";
		return new NTFS(letter);
		break;
	case 0x2020203631544146:
		Form1->Label8->Caption += "FAT16";
		return new FAT16(letter);
		break;
	default:
		return NULL;
		break;
	}
}

std::string GetFileSignature(std::vector<BYTE> ReadCluster) {
	BYTE* ReadCluster_ptr = &ReadCluster.front();
	FileSignature* pFileSignature = reinterpret_cast<FileSignature*>(ReadCluster_ptr);
	std::string Signature = reinterpret_cast<char*>(pFileSignature->Signature);


	BYTE png[8] = { 0x89, 0x50, 0x4e, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
	BYTE jpg[3][12] = {{0xFF, 0xD8, 0xFF, 0xDB},
	{0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x00, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01},
	{0xFF, 0xD8, 0xFF, 0xE1, 0x00, 0x00, 0x45, 0x78, 0x69, 0x66, 0x00, 0x00}};
	BYTE bmp[2] = { 0x42, 0x4D };
	BYTE gif[2][6] = {{0x47, 0x49, 0x46, 0x38, 0x37, 0x61},
						{0x47, 0x49, 0x46, 0x38, 0x39, 0x61}};
	BYTE pdf[5] = { 0x25, 0x50, 0x44, 0x46, 0x2D };
	BYTE exe[2] = { 0x4D, 0x5A };

	std::string ptr;

	if (Form1->CheckBox2->Checked) {
		ptr.clear();
		for(int i = 0; i < 8; i++) {
			ptr.push_back(char(png[i]));
		}
		if (ptr == std::string(Signature, 0, 8)) {
			return "PNG";
		}
	}
	if (Form1->CheckBox3->Checked) {
		UnicodeString Signature_copy = Signature.c_str();
		for (int j = 0; j < 3; j++) {
			ptr.clear();
			for(int i = 0; i < 12; i++) {
				ptr.push_back(char(jpg[j][i]));
			}
			UnicodeString ptr_copy = ptr.c_str();
			if (j != 0) {
				if (ptr_copy.SubString(0,4) == Signature_copy.SubString(0, 4) &&
				ptr_copy.SubString(6,6) == Signature_copy.SubString(6,6)) return "JPG";
			}
			else if (j == 0) {
				if ( ptr_copy.SubString(0,4) == Signature_copy.SubString(0, 4)) return "JPG";
			}
		}
	}
	if (Form1->CheckBox4->Checked) {
		ptr.clear();
		for(int i = 0; i < 2; i++) {
			ptr.push_back(char(bmp[i]));
		}
		if (ptr == std::string(Signature, 0, 2)) {
			return "BMP";
		}
	}
	if (Form1->CheckBox5->Checked) {
		for (int j = 0; j < 2; j++) {
			ptr.clear();
			for(int i = 0; i < 6; i++) {
				ptr.push_back(char(gif[j][i]));
			}
			if (ptr == std::string(Signature, 0, 6)) {
				return "GIF";
			}
		}
	}
	if (Form1->CheckBox6->Checked) {
		ptr.clear();
		for(int i = 0; i < 5; i++) {
			ptr.push_back(char(pdf[i]));
		}
		if (ptr == std::string(Signature, 0, 5)) {
			return "PDF";
		}
	}
	if (Form1->CheckBox7->Checked) {
		ptr.clear();
		for(int i = 0; i < 2; i++) {
			ptr.push_back(char(exe[i]));
		}
		if (ptr == std::string(Signature, 0, 2)) {
			return "EXE";
		}
	}
	return "None";
}
