#include<stdio.h>
#include<string.h>
#include<windows.h>
#include<time.h>
#include "test_file_f.h"


extern FILELOG filelogArray[FILE_N];
void fInitialVariableGlobal(void)
{

	
	for(int i=0; i<FILE_N; i++)
	{
		filelogArray[i].iReadedTimes=0;	
		filelogArray[i].iWroteTimes=0;	
		sprintf(filelogArray[i].pFileName, "%s", "file_");
		sprintf(filelogArray[i].pFileName+5, "%d", i);
		sprintf(filelogArray[i].pFileName+6, "%s", ".txt");
		filelogArray[i].hMux=CreateMutex(NULL, FALSE, "DD");	
	}
	fStartThreadWrite();
	fStartThreadRead();
	
}

extern FILE *fpArray[5];
bool bFileCreated;
void main(void)
{

	fInitialVariableGlobal();
	fCreateFiles();
	bFileCreated=1;
	while(1)
	{
		Sleep(5000);
		if(fCheckDone()==1)
			break;
	}
	fCloseFiles();

}