// NDX_extracter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CAB_define.h"
#include "..\common\md5.h"

using namespace std;

#define OPEN_TITLE	_T("Select the ndx's main data file.")
#define OPEN_FILTER	TEXT("NDX's data Files(*.dat;)\0*.dat\0\0")
#define MAX_BUF		40960

#define FILE_MD5	"c629b4341fd2872071bf4f6cd3d9bdf7"

CString OpenNDXFile();
bool	CheckFileMd5(CString strFile);

int _tmain(int argc, _TCHAR* argv[])
{
	CString strDataFile;
	strDataFile = OpenNDXFile();

	if (false == CheckFileMd5(strDataFile))
	{
		exit(-1);
	}

	return 0;
}

CString OpenNDXFile()
{
	TCHAR			szBuf[MAX_BUF]; 
	OPENFILENAME	ofn;  

	ZeroMemory(szBuf, sizeof(szBuf));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szBuf;
	ofn.lpstrFile[0] = _T('\0');
	ofn.nMaxFile = sizeof(szBuf);
	ofn.lpstrFilter = OPEN_FILTER;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = OPEN_TITLE;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

	if ( GetOpenFileName(&ofn) )
	{
		TCHAR szPath [MAX_PATH] = _T("");
		TCHAR szFile [MAX_PATH] = _T("");
		_tcsncpy( szPath, szBuf, ofn.nFileOffset ) ;
		szPath [ofn.nFileOffset] = _T('\0');

		int nLength = _tcslen(szPath) ;
		if (szPath[nLength - 1] != _T('\\'))
		{
			_tcscat(szPath, _T("\\"));
		}

		LPTSTR p = szBuf + ofn.nFileOffset ;
		while(*p != _T('\0'))
		{
			_tcscpy(szFile, szPath) ;
			_tcscat(szFile, p);
			p += _tcslen(p) + 1 ;
		}
		return CString(szFile) ;
	}

	return CString() ;
}

bool CheckFileMd5(CString strFile)
{
	MD5 md5;
	md5.update(ifstream(strFile.GetBuffer(0)));
	strFile.ReleaseBuffer(0);
	string strMD5 = md5.toString();

	if (strMD5 != FILE_MD5)
	{
		return false;
	}

	return true;
}