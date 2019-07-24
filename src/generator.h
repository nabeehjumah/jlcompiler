#ifndef generator_h
#define generator_h

#include "project.h"
#include "parser.h"

class NGenerator
{
private:
	NParser *parser;
	ast_list*tree;
	FILE* f;
	int offset;
	int retOff;
public:
	NGenerator(char*);
	~NGenerator();
	void Generate();
	void GenerateNode(AST *n);
	char *GenVar(symbol_table_entry *);
	char* Label();
};




#endif