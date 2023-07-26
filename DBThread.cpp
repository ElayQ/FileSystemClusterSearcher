//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "DBThread.h"
#pragma package(smart_init)

//---------------------------------------------------------------------------
extern sqlite3* Database;
extern unsigned __int64 FileCluster;
extern std::string fileFormat;
__fastcall DBThread::DBThread(TVirtualStringTree *VirtualStringTree_RT, TEvent *Event_RT, bool CreateSuspended)
	: TThread(CreateSuspended), Event_DB(Event_RT), VirtualStringTree_DB(VirtualStringTree_RT)
{

}
//---------------------------------------------------------------------------
void __fastcall DBThread::Execute()
{
	FreeOnTerminate = true;
	int openResult = sqlite3_open("DB.db", &Database);
	sqlite3_stmt *pStatement;

	while(true) {
		if (Event_DB->WaitFor(1) == wrSignaled) {

            // Заполнение формы VirtualStringTree1
			Synchronize(WriteInForm);
			TreeNodeStruct *nodeData = (TreeNodeStruct*)VirtualStringTree_DB->GetNodeData(entryNode);
			nodeData->ClusterNumber = FileCluster;
			nodeData->Signature = fileFormat.c_str();

			// Добавление в БД
			char sql1_begin[] = "INSERT INTO SignatureSearch (ClusterNumber, Signature) VALUES (";
			char buf1[sizeof(FileCluster)];
			sprintf(buf1, "%I64d", FileCluster);
			char sql1[sizeof(sql1_begin)+sizeof(FileCluster)+10];
			strcpy(sql1, sql1_begin);
			strcat(sql1, buf1);
			strcat(sql1, ", '");
			strcat(sql1, fileFormat.c_str());
			strcat(sql1, "');");

			int result = sqlite3_prepare_v2(Database, sql1, -1, &pStatement, NULL);
			if(result != SQLITE_OK)
			{
				Application->MessageBox(L"Error while preparing request to insert in DB!", L"Error", MB_OK);
			}

			result = sqlite3_step(pStatement);
            if(result != SQLITE_DONE)
			{
				Application->MessageBox(L"Error while executing query to insert in DB!", L"Error", MB_OK);
			}

			sqlite3_finalize(pStatement);

			Event_DB->ResetEvent();

		}
		if(Terminated) break;
	}
}
//---------------------------------------------------------------------------
void __fastcall DBThread::WriteInForm()
{
	entryNode = VirtualStringTree_DB->AddChild(VirtualStringTree_DB->RootNode);
}
