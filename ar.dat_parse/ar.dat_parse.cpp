// ar.dat_parse.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <shlobj.h>
#include <string.h>
#include "atlstr.h"
#include "Shlwapi.h"
#include "memmem.h"

#pragma comment(lib, "Shlwapi.lib")

#define TEXT_SCRIPT	("init_common_script")
#define TEXT_VOICE	("chatWaitVoice")

#define SCRIPT_OFFSET (sizeof(TEXT_SCRIPT))
#define VOICE_OFFSET  (sizeof(TEXT_VOICE)+11)

int g_nSuccessed = 0;

int find_all_files(const char * lpPath);
bool parse(const char * lpPath);
char* memstr(char* full_data, int full_data_len, char* substr);

int _tmain(int argc, _TCHAR* argv[])
{
	CHAR szPath[MAX_PATH] = "" ;
	// 选择待分析的文件夹
	BROWSEINFO stInfo = {NULL};
	LPCITEMIDLIST pIdlst;
	stInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	stInfo.lpszTitle= _T("ChoosePath");  
	pIdlst = SHBrowseForFolder(&stInfo);
	if(!pIdlst) 
		return -1;
	if(!SHGetPathFromIDList(pIdlst, szPath)) 
		return -1;

	find_all_files(szPath);

	std::cout << "successed parse file: " << g_nSuccessed << std::endl;

	return 0;
}

bool parse(const char * lpPath)
{
	int nFileSize = 0;
	int nEmptySize = 0;
	int nWirteSize = 0;
	BYTE* pFile = NULL;
	char *pPtr_text = NULL;
	const unsigned char *pPtr = NULL;

	HANDLE hFileHandle = CreateFile(lpPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileHandle == INVALID_HANDLE_VALUE)
	{
		DWORD dwErr = GetLastError();
		return false;
	}

	//获取文件大小
	LARGE_INTEGER tInt2;
	GetFileSizeEx(hFileHandle, &tInt2);
	nFileSize =  tInt2.QuadPart;
	pFile	  =	 new BYTE[nFileSize];

	//设置文件指针
	LARGE_INTEGER li;
	li.QuadPart = 0;
	SetFilePointer(hFileHandle, li.LowPart, &li.HighPart, FILE_BEGIN);

	//填充第一块缓冲区
	DWORD retlen;
	BOOL bRet = ReadFile(hFileHandle, pFile, nFileSize, &retlen, NULL);
	
	if (bRet)
	{
		//处理
		if((pPtr_text = memstr((CHAR*)pFile, nFileSize, TEXT_SCRIPT)) != NULL)
		{
			pPtr = (const unsigned char *)(pPtr_text + SCRIPT_OFFSET);
		}
		else if((pPtr_text = memstr((CHAR*)pFile, nFileSize, TEXT_VOICE)) != NULL)
		{
			pPtr = (const unsigned char *)(pPtr_text + VOICE_OFFSET);
		}
		else
		{
			goto END;
		}
		nEmptySize = pPtr - (const unsigned char *)pFile;
		nWirteSize = nFileSize - nEmptySize;

		DWORD writenum;
		char* pEmpty = new char[nEmptySize];
		memset(pEmpty, 0, nEmptySize);

		//无用数据置空
		SetFilePointer(hFileHandle, li.LowPart, &li.HighPart, FILE_BEGIN);
		WriteFile(hFileHandle, pEmpty, nEmptySize, &writenum, NULL);
		delete[] pEmpty;
		g_nSuccessed ++;

		//保存
#if 0
		SetFilePointer(hFileHandle, li.LowPart, &li.HighPart, FILE_BEGIN);
		WriteFile(hFileHandle, pPtr, nWirteSize, &writenum, NULL);
#endif
	}

END:
	CloseHandle(hFileHandle);
	hFileHandle = NULL;
	delete[] pFile;
	return false;
}

//find 'substr' from a fixed-length buffer   
//('full_data' will be treated as binary data buffer)  
//return NULL if not found  
char* memstr(char* full_data, int full_data_len, char* substr)  
{  
	if (full_data == NULL || full_data_len <= 0 || substr == NULL) {  
		return NULL;  
	}  

	if (*substr == '\0') {  
		return NULL;  
	}  

	int sublen = strlen(substr);  

	int i;  
	char* cur = full_data;  
	int last_possible = full_data_len - sublen + 1;  
	for (i = 0; i < last_possible; i++) {  
		if (*cur == *substr) {  
			//assert(full_data_len - i >= sublen);  
			if (memcmp(cur, substr, sublen) == 0) {  
				//found  
				return cur;  
			}  
		}  
		cur++;  
	}  

	return NULL;  
} 

int find_all_files(const char * lpPath)
{
	char szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	strcpy(szFind, lpPath);
	strcat(szFind, "\\*.*");
	HANDLE hFind=::FindFirstFile(szFind, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)   
		return -1;

	do
	{
		char szFile[MAX_PATH] = {0};
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(strcmp(FindFileData.cFileName,".") !=0 && strcmp(FindFileData.cFileName, "..")!=0)
			{
				//发现子目录，递归之
				strcpy(szFile, lpPath);
				strcat(szFile, "\\");
				strcat(szFile, FindFileData.cFileName);
				find_all_files(szFile);
			}
		}
		else
		{
			//找到文件，处理之
			std::cout << lpPath << "\\" << FindFileData.cFileName << std::endl;
			strcpy(szFile, lpPath);
			strcat(szFile, "\\");
			strcat(szFile, FindFileData.cFileName);
			parse(szFile);
		}
	}while(::FindNextFile(hFind, &FindFileData));

	::FindClose(hFind);

	return 0;
}


