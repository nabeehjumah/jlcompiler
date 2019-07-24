#ifndef ast_h
#define ast_h

#include "project.h"

 /* Externally-visible functions: */

AST *make_ast_node (AST_type, ... );
void print_ast_node (FILE *, AST *);
void p_a_n (FILE *, AST *, int);
void fatal_error();
void nl_indent (FILE *f, int d);
void print_ste_list (FILE *f, ste_list *L, char *prefix, char *sep, int d);
void print_ast_list (FILE *, ast_list *, char *, int);
char *ste_name(symbol_table_entry * ent);
void nl_indent (FILE *, int);
void p_a_n (FILE *, AST *, int);
void print_ast_list (FILE *, ast_list *, char *, int);
void print_ste_list (FILE *, ste_list *, char *, char *, int);

#endif