//---------------------------------------------------------------------------

#ifndef readthreadH
#define readthreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "main.h"
#include "FileSystem.h"
#include "DBThread.h"
//---------------------------------------------------------------------------
class ReadThread : public TThread
{
private:
	class DBThread *DBThreadPtr;
	TVirtualStringTree *VirtualStringTree_RT;
	int i = 0;
protected:
	void __fastcall Execute();
	void __fastcall UpdateCaption();
	void __fastcall EndOfProgramm();
public:
	__fastcall ReadThread(TVirtualStringTree *VirtualStringTree1, bool CreateSuspended);
	TEvent *Event_RT;
};
//---------------------------------------------------------------------------
#endif

