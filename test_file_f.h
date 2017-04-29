#ifndef TEST_FILE_F
#define TEST_FILE_F

#include<windows.h>		//Define HANDLE
#define FILE_N 5


typedef struct
{
	int iWroteTimes;  //  written times for a file
	int iReadedTimes; //  read times for a file
	int iCheckedTimes; // check times for a file
	char pFileName[100];
	HANDLE hMux;   // mutex
	FILE *fp;      // file point
	int iLenWrote; // The length of the content which write to file
	char pContenWrote[50]; // The content write to file
}FILELOG;


typedef struct
{
	int iThreadNum; // Write thread ID
	int iSleep;    // The sleep time for write thread 
}CONFIGWRITE;

typedef struct
{
	int iThreadNum; // Read thread ID
	int iSleep;     // The sleep time for read thread 
}CONFIGREAD;
FILELOG * fGenerateRandomWrite(FILELOG *pArrayHead); //Pick up one file randomly

void fThreadWrite(void *p);  // Write thread function

void fStartThreadWrite(void); // Start write thread


void fThreadRead(void *p); // Read thread function

void fStartThreadRead(void); // Start Read thread

void fCreateFiles(void); //Create 5 test files(file_0.txt£¬file_2.txt, ...file_4.txt) and CheckResult.txt
void fCloseFiles(void); //Close 5 test files(file_0.txt£¬file_2.txt, ...file_4.txt)
bool fCheckDone(void); //Return true after checked 2 times, or return false
bool fWriteDone(void); //Return true after the file is written
#endif