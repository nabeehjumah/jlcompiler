#ifndef reader_h
#define reader_h

#include "project.h"

#define FILENAME 256
class NReader
{
private:
	//file name, handle and state
	char m_file_name[FILENAME];
	//current location
	int m_index;
	int m_line;
	int m_char;
	//data read
	char* m_data;

public:
	NReader();
	NReader(char*);
	~NReader();

	//file op.s
	char* GetFileName();
	void Open(char*);
	void Close();
	//get a new line of data
	char GetChar();
	void UngetChar();
	//get the current location
	int LineNo();
	int CharNo();
	void Error(char*);
};



#endif