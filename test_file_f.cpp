#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include "test_file_f.h"
#include<windows.h>
#include <process.h>		// For _beginthread
#include <Mmsystem.h>		// For timeGetTime()        

#pragma comment(lib, "Winmm.lib") 	//For timeGetTime()  





FILELOG filelogArray[FILE_N];
FILE * fpResult;

FILELOG * fGenerateRandomWrite(FILELOG *pArrayHead)
{
	
	int iPool[FILE_N];
	int index=0;		//
	int indexSel;
	for(int i=0; i<FILE_N; i++)
	{
		if(2>(pArrayHead+i)->iWroteTimes && (pArrayHead+i)->iWroteTimes==(pArrayHead+i)->iCheckedTimes)
		{
			iPool[index]=i;		// Pick up one file in iPool
			index++;
		}
	}
	if(0==index)
	{
		return NULL;
	}
	else
	{
		
		srand(timeGetTime());
		indexSel=rand()%index;
		return pArrayHead+iPool[indexSel];
	}	
}

FILELOG * fGenerateRandomRead(FILELOG *pArrayHead)
{
	
	int iPool[2*FILE_N];
	int index=0;		
	int iCntNeedCheck;
	int indexSel;
	for(int i=0; i<FILE_N; i++)
	{
		iCntNeedCheck=(pArrayHead+i)->iWroteTimes- (pArrayHead+i)->iCheckedTimes;		
		//printf("iCntNeedCheck= %d\n" ,iCntNeedCheck);
		int j=0;
		while(j<iCntNeedCheck)
		{
			iPool[index]=i;	
			index++;
			j++;
		}
	}
	if(0==index)
	{
		return NULL;
	}
	else
	{
		
		srand(timeGetTime());
		indexSel=rand()%index;
		return pArrayHead+iPool[indexSel];
	}	
}

extern bool bFileCreated;
void fThreadWrite(void *p)
{
	CONFIGWRITE *pConfig=(CONFIGWRITE*)p;
	while(0==bFileCreated)
	{
		Sleep(1);
	}
	while(1)
	{
		FILELOG *pSel=fGenerateRandomWrite(filelogArray);
		if(NULL==pSel)
		{
			if(1==fWriteDone())
			{
				printf("There is no file to write in %d thread,\n", pConfig->iThreadNum);
				return;
			}
			else
			{
				//printf("waiting in %d thread\n", pConfig->iThreadNum);
				//Sleep(10);
				continue;
			}
		}
		else
		{
			WaitForSingleObject(pSel->hMux,INFINITE); 
			if(pSel->iWroteTimes==pSel->iCheckedTimes)		
			{
				//write random content in the file
				fseek(pSel->fp,0L,SEEK_SET);		//write from the head of file
				
				int iRandomContent=rand();
				pSel->iLenWrote=fprintf(pSel->fp, "%d", iRandomContent);	//disk record
				sprintf(pSel->pContenWrote, "%d", iRandomContent);			//memeory reccond

				fflush(pSel->fp);

				pSel->iWroteTimes++;
				printf("the %d write thread ,file name: %s ,content: %s ,length:%d\n", pConfig->iThreadNum, pSel->pFileName, pSel->pContenWrote, pSel->iLenWrote);
			}
			ReleaseMutex(pSel->hMux);
		}
		Sleep(pConfig->iSleep);
	}

}


CONFIGWRITE configWrite1;
CONFIGWRITE configWrite2;
CONFIGWRITE configWrite3;
CONFIGWRITE configWrite4;
CONFIGWRITE configWrite5;
CONFIGWRITE configWrite6;
CONFIGWRITE configWrite7;
CONFIGWRITE configWrite8;
CONFIGWRITE configWrite9;
CONFIGWRITE configWrite10;

void fStartThreadWrite(void)
{
	configWrite1.iSleep=5;
	configWrite1.iThreadNum=1;
	configWrite2.iSleep=12;
	configWrite2.iThreadNum=2;
	configWrite3.iSleep=3;
	configWrite3.iThreadNum=3;
	configWrite4.iSleep=4;
	configWrite4.iThreadNum=4;
	configWrite5.iSleep=7;
	configWrite5.iThreadNum=5;
	configWrite6.iSleep=5;
	configWrite6.iThreadNum=6;
	configWrite7.iSleep=12;
	configWrite7.iThreadNum=7;
	configWrite8.iSleep=3;
	configWrite8.iThreadNum=8;
	configWrite9.iSleep=4;
	configWrite9.iThreadNum=9;
	configWrite10.iSleep=7;
	configWrite10.iThreadNum=10;
	_beginthread( fThreadWrite, 0, (void *)&configWrite1 );
	_beginthread( fThreadWrite, 0, (void *)&configWrite2 );
	_beginthread( fThreadWrite, 0, (void *)&configWrite3 );
	_beginthread( fThreadWrite, 0, (void *)&configWrite4 );
	_beginthread( fThreadWrite, 0, (void *)&configWrite5 );
	_beginthread( fThreadWrite, 0, (void *)&configWrite6 );
	_beginthread( fThreadWrite, 0, (void *)&configWrite7 );
	_beginthread( fThreadWrite, 0, (void *)&configWrite8 );
	_beginthread( fThreadWrite, 0, (void *)&configWrite9 );
	_beginthread( fThreadWrite, 0, (void *)&configWrite10 );
}



void fThreadRead(void *p)
{
	CONFIGREAD *pConfig=(CONFIGREAD*)p;
	while(0==bFileCreated)
	{
		Sleep(1);
	}
	while(1)
	{
		FILELOG *pSel=fGenerateRandomRead(filelogArray);
		if(NULL==pSel)
		{
			if(1==fCheckDone())
			{
				printf("in %d read thread check is done\n", pConfig->iThreadNum);
				return;
			}
			else
			{
				//printf("in %d read thread waiting for check\n", pConfig->iThreadNum);
				//Sleep(10);
				continue;
			}
		}
		else
		{
			WaitForSingleObject(pSel->hMux,INFINITE); 
			if(pSel->iCheckedTimes<pSel->iWroteTimes)		
			{
				//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
				char pContentReaded[50];
				memset(pContentReaded, 0, 50);
				fseek(pSel->fp,0L,SEEK_SET);
				int iReaded=fread(pContentReaded, 1, pSel->iLenWrote, pSel->fp);
				//iReaded=fread(pContentReaded, 1, 20, pSel->fp);
				pSel->iCheckedTimes++;

				printf("in %d thread;Checking filename: %s ,content in disk: %s, length:%d\n", pConfig->iThreadNum, pSel->pFileName, pContentReaded, iReaded);
				char pResultDisplay[20];
				memset(pResultDisplay, 0, 20);
				if(0==strcmp(pSel->pContenWrote, pContentReaded))
				{
					memcpy(pResultDisplay, "Pass", 4);
				}
				else
				{
					memcpy(pResultDisplay, "Fail", 4);
				}

				fprintf(fpResult, "FileName: %s\n", pSel->pFileName);
				fprintf(fpResult, "Expected Content: %s\n", pSel->pContenWrote);
				fprintf(fpResult, "Actual Content: %s\n", pContentReaded);	
				fprintf(fpResult, "Test Result: %s\n", pResultDisplay);	
				fflush(fpResult);
			}
			ReleaseMutex(pSel->hMux);
		}
		Sleep(pConfig->iSleep);
	}

}

CONFIGREAD configRead1;
CONFIGREAD configRead2;
void fStartThreadRead(void)
{
	configRead1.iSleep=1;
	configRead1.iThreadNum=1;
	configRead2.iSleep=1;
	configRead2.iThreadNum=2;
	_beginthread( fThreadRead, 0, (void *)&configRead1 );
	_beginthread( fThreadRead, 0, (void *)&configRead2 );
}



void fCreateFiles(void)
{
	for(int i=0; i<FILE_N; i++)
	{
		filelogArray[i].fp=fopen(filelogArray[i].pFileName, "w+");
		printf("Created: %s\n",
			filelogArray[i].pFileName);
	}
	fpResult=fopen("CheckReuslt.txt", "w+");
}


void fCloseFiles(void)
{
	for(int i=0; i<FILE_N; i++)
	{
		fclose(filelogArray[i].fp);
		printf("Closed: %s\n",
			filelogArray[i].pFileName);
	}
}

bool fCheckDone(void)
{	
	for(int i=0; i<FILE_N; i++)
	{
		if(2!=filelogArray[i].iCheckedTimes)
			return 0;
	}
	return 1;
}

bool fWriteDone(void)
{	
	for(int i=0; i<FILE_N; i++)
	{
		if(2!=filelogArray[i].iWroteTimes)
			return 0;
	}
	return 1;
}