// CEStarterTellnet.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

_TCHAR *exeToStart;
_TCHAR *logToStart;

int  InitConsole();


int _tmain(int argc, _TCHAR* argv[])
{
	exeToStart=argv[1];
	logToStart=argv[2];
	InitConsole();
	while(1);
	return 0;
}

