//---------------------------------------------------------------------------

#ifndef DBThreadH
#define DBThreadH
#include "main.h"
#include "FileSystem.h"
#include "ReadThread.h"
#include "sqlite3.h"
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class DBThread : public TThread
{
private:
	TEvent *Event_DB;
	TVirtualStringTree *VirtualStringTree_DB;
protected:
	void __fastcall Execute();
	void __fastcall WriteInForm();
public:
	__fastcall DBThread(TVirtualStringTree *VirtualStringTree_RT, TEvent *Event_RT, bool CreateSuspended);
    PVirtualNode entryNode;
};
//---------------------------------------------------------------------------
#endif
