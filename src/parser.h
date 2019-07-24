#ifndef parser_h
#define parser_h

#include"project.h"
#include"scanner.h"
#include "SymbolTable.h"
#include "ast.h"

class NParser 
{
private:
	NScanner * scanner;
	TOKEN * lookahead;
	TOKEN* current;
public:
	void Error(char*);
	ast_list * root;
	NStack* st;
	NParser(char*);
	~NParser();
	ast_list* Parse();
	int match(int);
	int decl();
	int type();
	ste_list* formal_list();
	ste_list* formals();
	AST* stmt();
	AST* block();
	ste_list* var_decl_list();
	ast_list* stmt_list();
	ast_list * arg_list();
	ast_list * args();
	AST* expr();
	AST* E1();
	AST* E2();
	AST* E3();
	AST* E4();
	AST* E5();
	AST* E6();
	//semantic checking
	j_type expr_type(AST* e);
};



#endif