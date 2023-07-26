//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include "FileSystem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
char IterOrDecorSignature;
sqlite3* Database;
ULONGLONG FileSystemSignature;
FileSystem* pFileSystem;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	ReadThreadPtr = new ReadThread(VirtualStringTree1, true);
    VirtualStringTree1->NodeDataSize = sizeof(TreeNodeStruct);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	IterOrDecorSignature = 'i';
	VirtualStringTree1->Clear();
	// Если не нужно заполнение формы в реальном времени
	//VirtualStringTree1->BeginUpdate();
	ReadThreadPtr->Start();
	Button1->Enabled = false;
	Button2->Enabled = false;
	Label2->Visible = true;
	Label3->Visible = true;
	Label4->Visible = true;
	Label5->Visible = true;
	Label6->Visible = true;
	Button4->Visible = true;
	Button5->Visible = true;
	ProgressBar1->Visible = true;
	Button3->Enabled = true;
	VirtualStringTree1->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
	IterOrDecorSignature = 'd';
	VirtualStringTree1->Clear();
	//VirtualStringTree1->BeginUpdate();
    ReadThreadPtr->Priority = tpTimeCritical;
	ReadThreadPtr->Start();
	Button1->Enabled = false;
	Button2->Enabled = false;
	Label2->Visible = true;
	Label3->Visible = true;
	Label4->Visible = true;
	Label5->Visible = true;
	Label6->Visible = true;
	Button4->Visible = true;
	Button5->Visible = true;
	ProgressBar1->Visible = true;
	Button3->Enabled = true;
    VirtualStringTree1->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button3Click(TObject *Sender)
{
	// Заполнение после завершения потоков
	//VirtualStringTree1->EndUpdate();
	ReadThreadPtr->Terminate();
	Button3->Enabled = false;
	GroupBox1->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
	// Удаление из БД
	int SelectedNodesCount = VirtualStringTree1->SelectedCount;

	PVirtualNode SelectedNode = VirtualStringTree1->GetFirstSelected();
	TreeNodeStruct *nodeData = (TreeNodeStruct*)VirtualStringTree1->GetNodeData(SelectedNode);
	char sql_begin[] = "DELETE FROM SignatureSearch WHERE ClusterNumber=";
	char buf[sizeof(nodeData->ClusterNumber)];
	sprintf(buf, "%I64d", nodeData->ClusterNumber);
	char sql[sizeof(sql_begin)+sizeof(buf)+1];
	strcpy(sql, sql_begin);
	strcat(sql, buf);
	strcat(sql, ";");

	sqlite3_stmt *pStatement;
	int result = sqlite3_prepare_v2(Database, sql, -1, &pStatement, NULL);
	if(result != SQLITE_OK)
	{
		Application->MessageBox(L"Error while preparing request!", L"Error", MB_OK);
	}
	result = sqlite3_step(pStatement);
	if(result != SQLITE_DONE)
	{
		Application->MessageBox(L"Error while executing query!", L"Error", MB_OK);
	}
	sqlite3_finalize(pStatement);

	if (SelectedNodesCount != 1) {
		for (int i = 1; i < SelectedNodesCount; i++) {
			SelectedNode = VirtualStringTree1->GetNextSelected(SelectedNode);
			nodeData = (TreeNodeStruct*)VirtualStringTree1->GetNodeData(SelectedNode);
			char sql_begin[] = "DELETE FROM SignatureSearch WHERE ClusterNumber=";
			char buf[sizeof(nodeData->ClusterNumber)];
			sprintf(buf, "%I64d", nodeData->ClusterNumber);
			char sql[sizeof(sql_begin)+sizeof(buf)+1];
			strcpy(sql, sql_begin);
			strcat(sql, buf);
			strcat(sql, ";");

			result = sqlite3_prepare_v2(Database, sql, -1, &pStatement, NULL);
			if(result != SQLITE_OK)
			{
				Application->MessageBox(L"Error while preparing request!", L"Error", MB_OK);
			}
			result = sqlite3_step(pStatement);
			if(result != SQLITE_DONE)
			{
				Application->MessageBox(L"Error while executing query!", L"Error", MB_OK);
			}
			sqlite3_finalize(pStatement);
		}
	}

	// Удаление только из ГУИ (в том числе и нескольких выделенных строк)
	VirtualStringTree1->DeleteSelectedNodes();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::VirtualStringTree1GetText(TBaseVirtualTree *Sender, PVirtualNode Node,
		  TColumnIndex Column, TVSTTextType TextType, UnicodeString &CellText)

{
	if(Node == NULL) return;
	TreeNodeStruct *nodeData = (TreeNodeStruct*)VirtualStringTree1->GetNodeData(Node);
	switch(Column)
	{
		case 0:
		{
			CellText = UnicodeString(nodeData->ClusterNumber); break;
		}
		case 1:
		{
			CellText = nodeData->Signature; break;
		}

	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button5Click(TObject *Sender)
{
	// Удаление из БД всех строк, которые сейчас являются активными Nodes
    // И отображаются в форме VirtualStringTree
	int TotalCount = VirtualStringTree1->TotalCount;
	PVirtualNode Node = VirtualStringTree1->GetFirst();
	TreeNodeStruct *nodeData = (TreeNodeStruct*)VirtualStringTree1->GetNodeData(Node);
    char sql_begin[] = "DELETE FROM SignatureSearch WHERE ClusterNumber=";
	char buf[sizeof(nodeData->ClusterNumber)];
	sprintf(buf, "%I64d", nodeData->ClusterNumber);
	char sql[sizeof(sql_begin)+sizeof(buf)+1];
	strcpy(sql, sql_begin);
	strcat(sql, buf);
	strcat(sql, ";");

	sqlite3_stmt *pStatement;
	int result = sqlite3_prepare_v2(Database, sql, -1, &pStatement, NULL);
	if(result != SQLITE_OK)
	{
		Application->MessageBox(L"Error while preparing request!", L"Error", MB_OK);
	}
	result = sqlite3_step(pStatement);
	if(result != SQLITE_DONE)
	{
		Application->MessageBox(L"Error while executing query!", L"Error", MB_OK);
	}
	sqlite3_finalize(pStatement);

		if (TotalCount != 1) {
		for (int i = 1; i < TotalCount; i++) {
			Node = VirtualStringTree1->GetNext(Node);
			nodeData = (TreeNodeStruct*)VirtualStringTree1->GetNodeData(Node);
			char sql_begin[] = "DELETE FROM SignatureSearch WHERE ClusterNumber=";
			char buf[sizeof(nodeData->ClusterNumber)];
			sprintf(buf, "%I64d", nodeData->ClusterNumber);
			char sql[sizeof(sql_begin)+sizeof(buf)+1];
			strcpy(sql, sql_begin);
			strcat(sql, buf);
			strcat(sql, ";");

			result = sqlite3_prepare_v2(Database, sql, -1, &pStatement, NULL);
			if(result != SQLITE_OK)
			{
				Application->MessageBox(L"Error while preparing request!", L"Error", MB_OK);
			}
			result = sqlite3_step(pStatement);
			if(result != SQLITE_DONE)
			{
				Application->MessageBox(L"Error while executing query!", L"Error", MB_OK);
			}
			sqlite3_finalize(pStatement);
		}
	}

	// Очистка компонента VirtualStringTree
	VirtualStringTree1->Clear();

	// Закрытие БД
	sqlite3_close(Database);

	// Код ниже для удаления всех строк из БД, не только тех, которые
	// отображаются на форме
	/*
	const char *sql = "DELETE FROM SignatureSearch;";
    sqlite3_stmt *pStatement;
	int result = sqlite3_prepare_v2(Database, sql, -1, &pStatement, NULL);
    if(result != SQLITE_OK)
	{
		throw Exception("Error while preparing request!");
	}
	result = sqlite3_step(pStatement);
	if(result != SQLITE_DONE)
	{
		throw Exception("Error while executing query!");
	}
	sqlite3_finalize(pStatement); */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit1KeyPress(TObject *Sender, System::WideChar &Key)
{
    // Разрешаем ввод только от A до Z (в верхнем регистре) и Backspace
	if ((Key >= 65) && (Key <= 90)) return;
	if (Key == VK_BACK) return;
	Beep();
    Key = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button6Click(TObject *Sender)
{
	FileSystemSignature = GetFileSystemType(Form1->Edit1->Text);
	pFileSystem = CreateFileSystem(FileSystemSignature, Form1->Edit1->Text);
	unsigned int pClustersSize = pFileSystem->ClusterSize();
	unsigned __int64 pClustersCount = pFileSystem->ClustersCount();
	unsigned __int64 pFileSystemSize = pFileSystem->FileSystemSize();
	Form1->Label9->Caption += pClustersSize;
	Form1->Label10->Caption += pClustersCount;
	Form1->Label11->Caption += pFileSystemSize;
	Button6->Enabled = false;
	Button1->Enabled = true;
	Button2->Enabled = true;
	Label13->Visible = true;
	Label11->Visible = true;
	Label10->Visible = true;
	Label9->Visible = true;
	Label8->Visible = true;
	CheckBox1->Enabled = true;
	CheckBox2->Enabled = true;
	CheckBox3->Enabled = true;
	CheckBox4->Enabled = true;
	CheckBox5->Enabled = true;
	CheckBox6->Enabled = true;
	CheckBox7->Enabled = true;
	Edit1->Enabled = false;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CheckBox1Click(TObject *Sender)
{
	if (CheckBox1->Checked==true) {
		CheckBox2->Checked=true;
		CheckBox3->Checked=true;
		CheckBox4->Checked=true;
		CheckBox5->Checked=true;
		CheckBox6->Checked=true;
		CheckBox7->Checked=true;
	}
	else {
		CheckBox2->Checked=false;
		CheckBox3->Checked=false;
		CheckBox4->Checked=false;
		CheckBox5->Checked=false;
		CheckBox6->Checked=false;
		CheckBox7->Checked=false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit1MouseLeave(TObject *Sender)
{
    Button6->Enabled=true;
}
//---------------------------------------------------------------------------

