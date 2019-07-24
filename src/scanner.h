#ifndef scanner_h
#define scanner_h

#include"project.h"
#include"reader.h"

class NScanner
{
private:
	NReader* m_file;
public:
	NScanner(char*);
	TOKEN* Scan();
	void Error(char*);
};

#endif