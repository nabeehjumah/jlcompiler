#include "reader.h"

NReader::NReader()
{
	m_file_name[0]=0;
	m_index=0;
	m_line=1;
	m_char=1;
	m_data=0;
}

NReader::NReader(char* name)
{
	strcpy(m_file_name,name);
	m_index=0;
	m_line=1;
	m_char=1;
	
	HANDLE file;
	DWORD file_size;
	DWORD read;
	file=CreateFile(name,GENERIC_READ,0,0,OPEN_EXISTING,0,0);
	file_size=GetFileSize(file,0);
	m_data=(char*)LocalAlloc(LPTR,file_size+1);
	ReadFile(file,m_data,file_size,&read,0);
	m_data[read]=0;
	CloseHandle(file);
}

NReader::~NReader()
{
	LocalFree(m_data);
}

//*****************************************
char* NReader::GetFileName()
{
	return m_file_name;
}

void NReader::Open(char* name)
{
	strcpy(m_file_name,name);
	m_index=0;
	m_line=1;
	m_char=1;

	HANDLE file;
	DWORD file_size;
	DWORD read;
	file=CreateFile(name,GENERIC_READ,0,0,OPEN_EXISTING,0,0);
	file_size=GetFileSize(file,0);
	m_data=(char*)LocalAlloc(LPTR,file_size);
	ReadFile(file,m_data,file_size+1,&read,0);
	m_data[read]=0;
	CloseHandle(file);
}

void NReader::Close()
{
	LocalFree(m_data);
}

char NReader::GetChar()
{
	char ch = m_data[m_index];
	m_index++;
	m_char++;
	if(ch=='\n'){
		m_line++;
		m_char=1;
	}
	else if(ch==0){
//		m_index--;
	}
	return ch;
}

void NReader::UngetChar()
{
	m_index--;
	m_char--;
}

//******************************************
int NReader::LineNo()
{
	return m_line;
}

int NReader::CharNo()
{
	return m_char;
}

void NReader::Error(char* error)
{
	char line[1024];
	char* ptr=&m_data[m_index];
	while((*ptr!='\n') && (ptr!=m_data))ptr--;
	if(*ptr == '\n')ptr++;
	int i=0;
	char c;
	do{
		c=line[i]=ptr[i];
		i++;
	}while((c!='\n') && (c!=0));
	line[i-1]=0;
	printf("\n");
	printf(line);
	printf("\n");
	for(i=2;i<m_char;i++){
		if(line[i-2]=='\t')
			printf("\t");
		else
			printf(" ");
	}
	printf("^");
	printf("\nerror at line(%d): %s\n",m_line,error);
	exit(0);
}