#include <stdio.h>
#include <windows.h>

int main(int argc,char**argv)
{
	char res[4096];
	char res2[4096];
	char command[4096];
	char mshta[MAX_PATH];
	char curdir[MAX_PATH];
	char curdirBin[MAX_PATH];
	int i;
	int last;
	char s[MAX_PATH];
	
	SetEnvironmentVariable("SEE_MASK_NOZONECHECKS","1");
	
	//Путь до mshta.exe
	GetWindowsDirectory(mshta, MAX_PATH);
	sprintf(mshta,"%s\\System32\\mshta.exe",mshta);
	printf("mshtaPath: %s\n", mshta);
	
	
	//Полный путь до нашего EXE
	if (GetModuleFileName(NULL, s, sizeof(s) / sizeof(*s)) > 0)
	printf("GetModuleFileName=%s\n", s);
	
	
	//Находим последнее вхождение символа \ чтобы обрезать имя EXE
	last=sizeof(s);
	for (i=0; s[i]!='\0'; i++){
		if (s[i]=='\\'){ last = i; }
	}
	//printf("indexOf: %d\n", last);
	
	
	//Обрезаем имя EXE, чтобы получить именно текущую директорию
	strncpy(curdir,s,last);
	printf("curdir=%s\n", curdir);
	
	
	
	//Собираем все параметры полученные через командную строку
	strcpy(command,"");
	if (argc>1){
		int i;
		for(i=1; i<argc; ++i) {
			strcat(command," ");
			strcat(command,argv[i]);
		}
	}
	puts(command);

	
	
	//Устанавливаем текущую директорию
	sprintf(curdirBin,"%s\\bin",curdir);
	SetCurrentDirectory(curdirBin);
	printf("curdirBin: %s\n",curdirBin);
	
	//Собираем параметры, которые будем передавать в mshta.exe
	sprintf(res,"\"%s\\Tools\\run.hta\"%s",curdirBin,command);
	
	//Если запускаем не через SMB, а локально
	if(!(curdirBin[0]=='\\'&&curdirBin[1]=='\\'))
	{
		//Запускаем HTA-приложение
		ShellExecute(0,"open",mshta,res,0,SW_SHOWNORMAL);
		printf("Executed: %s %s\n",mshta,res);
		return 0;
	}
	
	
	//Подключаем сетевой диск
	SetCurrentDirectory(curdir);
	system("NET USE \"Z:\" /DELETE /YES");
	sprintf(res2,"SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION & FOR /F \"tokens=2\" \%\%D IN ('NET USE * \"%s\" /PERSISTENT:NO^|FINDSTR /I 'unidad') DO SET DRIVELETTER=\%\%D",curdir);
	printf("NetUse: %s\n",res2);
	system(res2);
	
	
	//Запускаем HTA-приложение
	SetCurrentDirectory("Z:\\bin\\");
	ShellExecute(0,"open",mshta,res,0,SW_SHOWNORMAL);
	printf("Executed: %s %s\n",mshta,res);
	return 0;
}



