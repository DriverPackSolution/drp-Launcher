#include <stdio.h>
#include <windows.h>
int main(int argc,char**argv)
{
	char buf[4096];
	char res[4096];
	char res2[4096];
	char command[4096];
	char *p;
	
	strcpy(command,"");
	if (argc>1){
		int i;
		for(i=1; i<argc; ++i) {
			strcat(command," ");
			strcat(command,argv[i]);
		}
	}
	puts(command);

	sprintf(buf,"%s",argv[0]);
	p=buf;
	while(*p)p++;
	while(*p!='\\'&&p!=buf)p--;
	*p=0;
	
	sprintf(buf,"%s\\bin",buf);
	printf(buf);
	SetCurrentDirectory(buf);
	GetCurrentDirectory(4096,buf);
	printf("(%s)",buf);

	if(!(buf[0]=='\\'&&buf[1]=='\\'))
	{
		sprintf(res,"\"%s\\Tools\\run.hta\"%s",buf,command);
		printf(res);
		ShellExecute(0,"open","Tools\\mshta.exe",res,0,SW_SHOWNORMAL);
		return 0;
	}

	sprintf(res,"\"%s\\Tools\\run.hta\"",buf);
	system("NET USE \"Z:\" /DELETE /YES");
	sprintf(res2,"SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION & FOR /F \"tokens=2\" \%\%D IN ('NET USE * \"%s\" /PERSISTENT:NO^|FINDSTR /I 'unidad') DO SET DRIVELETTER=\%\%D",buf);
	system(res2);
	printf(res2);

	SetEnvironmentVariable("SEE_MASK_NOZONECHECKS","1");
	SetCurrentDirectory("Z:\\");
	ShellExecute(0,"open","Z:\\Tools\\mshta.exe","Z:\\Tools\\run.hta",0,SW_SHOWNORMAL);
	return 0;
}