#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <wchar.h>
#include <shellapi.h>
#include "shlwapi.h"

#define _O_U16TEXT  0x20000

HINSTANCE ShellExecuteSync(HWND hwnd, LPCTSTR lpVerb, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShow)
{
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI | SEE_MASK_NO_CONSOLE;
	ShExecInfo.hwnd = hwnd;
	ShExecInfo.lpVerb = lpVerb;
	ShExecInfo.lpFile = lpFile;
	ShExecInfo.lpParameters = lpParameters;
	ShExecInfo.lpDirectory = lpDirectory;
	ShExecInfo.nShow = nShow;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	CloseHandle(ShExecInfo.hProcess);
	wprintf(L"ShellExecute %s: %s %s\n",lpVerb,lpFile,lpParameters);
	return ShExecInfo.hInstApp;
}

void run(WCHAR* mshta, WCHAR* res)
{
	WCHAR cmd[MAX_PATH];
	WCHAR init[MAX_PATH];
	WCHAR failover[MAX_PATH];

	//Путь до cmd.exe
	GetWindowsDirectory(cmd, MAX_PATH);
	wsprintf(cmd,L"%s\\System32\\cmd.exe", cmd);

	//Скрипт инициализации
	wsprintf(init,L"/c Tools\\init.cmd %s",res);

	//failover-скрипт
	wsprintf(failover,L"/c Tools\\failover.cmd %s",res);

	ShellExecuteSync(0,L"open",cmd,init,0,SW_HIDE);
	ShellExecuteSync(0,L"open",mshta,res,0,SW_SHOWNORMAL);
	ShellExecuteSync(0,L"open",cmd,failover,0,SW_HIDE);
}

int main(int argc,char**argv)
{
	WCHAR res[4096];
	WCHAR res2[4096];
	WCHAR command[4096];
	WCHAR mshta[MAX_PATH];
	WCHAR curdir[MAX_PATH];
	WCHAR curdirBin[MAX_PATH];
	WCHAR s[MAX_PATH];

	_setmode(_fileno(stdout), _O_U16TEXT);

	int nargs;
	WCHAR **arglist = CommandLineToArgvW(GetCommandLineW(), &nargs);

	SetEnvironmentVariable(L"SEE_MASK_NOZONECHECKS",L"1");

	//Путь до mshta.exe
	GetWindowsDirectory(mshta, MAX_PATH);
	wsprintf(mshta,L"%s\\System32\\mshta.exe",mshta);
	wprintf(L"mshtaPath: %s\n", mshta);


	//Полный путь до нашего EXE
	GetModuleFileNameW(GetModuleHandleW(NULL), s, MAX_PATH);
	wprintf(L"GetModuleFileName=%s\n", s);

	//Обрезаем имя EXE, чтобы получить именно текущую директорию
	PathRemoveFileSpec(s);
	wcscpy(curdir,s);
	wprintf(L"curdir=%s\n", curdir);



	//Собираем все параметры полученные через командную строку
	wcscpy(command,L"");
	if (argc>1){
		int i;
		for(i=1; i<argc; ++i) {
			wcscat(command,L" ");
			wcscat(command,L"\"");
			wcscat(command,arglist[i]);
			wcscat(command,L"\"");
		}
	}
	_putws(command);



	//Устанавливаем текущую директорию
	wsprintf(curdirBin,L"%s"_CURR_DIR_,curdir);
	SetCurrentDirectory(curdirBin);
	wprintf(L"curdirBin: %s\n",curdirBin);

	//Собираем параметры, которые будем передавать в mshta.exe
	wsprintf(res,L"\"%s"_HTA_PATH_"\"%s",curdirBin,command);

	//Если запускаем не через SMB, а локально
	if(!(curdirBin[0]=='\\'&&curdirBin[1]=='\\'))
	{
		//Запускаем HTA-приложение
		run(mshta, res);
		return 0;
	}


	//Подключаем сетевой диск
	SetCurrentDirectory(curdir);
	_wsystem(L"NET USE \"Z:\" /DELETE /YES");
	wsprintf(res2,L"SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION & FOR /F \"tokens=2\" \%\%D IN ('NET USE * \"%s\" /PERSISTENT:NO^|FINDSTR /I 'unidad') DO SET DRIVELETTER=\%\%D",curdir);
	wprintf(L"NetUse: %s\n",res2);
	_wsystem(res2);


	//Запускаем HTA-приложение
	SetCurrentDirectory(L"Z:"_CURR_DIR_"\\");
	run(mshta, res);
	return 0;
}
