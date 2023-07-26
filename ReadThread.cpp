//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "ReadThread.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
extern ULONGLONG FileSystemSignature;
extern FileSystem* pFileSystem;
extern char IterOrDecorSignature;
unsigned __int64 ReadClustersCount = 0;
unsigned int count = 0;
unsigned __int64 FileCluster = 0;
std::string fileFormat;
extern int flag;
extern UnicodeString Error;
__fastcall ReadThread::ReadThread(TVirtualStringTree *VirtualStringTree1, bool CreateSuspended)
	: TThread(CreateSuspended), VirtualStringTree_RT(VirtualStringTree1)
{
	Event_RT = new TEvent(NULL, true, false, "", false);
	DBThreadPtr = new DBThread(VirtualStringTree_RT, Event_RT, true);
}
//---------------------------------------------------------------------------
void __fastcall ReadThread::Execute()
{
	FreeOnTerminate = true;

	DBThreadPtr->FreeOnTerminate=true;
	DBThreadPtr->Start();


	Iterator* iter;
	// Создаем в зависимости от нажатой кнопки итератор всех кластеров/декоратор непустых кластеров
	if (IterOrDecorSignature == 'i')
	{
		iter = new ConcreteIterator(pFileSystem, FileSystemSignature);
	}
	else if (IterOrDecorSignature == 'd')
	{
		iter = new ConcreteDecorator(new ConcreteIterator(pFileSystem, FileSystemSignature));
	}

	if (FileSystemSignature == 0x2020203631544146) {
		Form1->ProgressBar1->Min = 2;
	}
	else {
		Form1->ProgressBar1->Min = 0;
    }

	Form1->ProgressBar1->Max = iter->end();


	for (iter->begin(); iter->currentindex() <= iter->end(); iter->next())
	{
			while(Event_RT->WaitFor(1) == wrSignaled) Sleep(1);
			std::vector<BYTE> ReadCluster = iter->GetCurrentCluster();
			Synchronize(EndOfProgramm);
			if (!ReadCluster.empty()) {
				fileFormat = GetFileSignature(ReadCluster);
				if (fileFormat != "None") {
					count++;
					FileCluster = iter->currentindex();
					Event_RT->SetEvent();
				}
				ReadClustersCount++;
			}
			Form1->ProgressBar1->Position++;
			Synchronize(UpdateCaption);
			if (Terminated) break;
	}
	Form1->Label7->Visible = true;
	Form1->Button3->Enabled = false;
    Form1->GroupBox1->Enabled = false;
	DBThreadPtr->Terminate();
	delete DBThreadPtr;
	Terminate();
}
//---------------------------------------------------------------------------
void __fastcall ReadThread::UpdateCaption()
{
	Form1->Label5->Caption = ReadClustersCount;
	Form1->Label6->Caption = count;
	Application->ProcessMessages();
}
//---------------------------------------------------------------------------
void __fastcall ReadThread::EndOfProgramm() {
	switch (flag)
	{
	case 1:
		Application->MessageBox(Error.c_str(), L"Error", MB_OK);
		Form1->Close();
		break;
	case 2:
		Application->MessageBox(Error.c_str(), L"Error", MB_OK);
		Form1->Close();
		break;
	default:
		break;
	}
}

