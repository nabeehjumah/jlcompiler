#ifndef project_h
#define project_h


#include <windows.h>
#include <stdio.h>



typedef enum{type_integer,type_string,type_boolean,type_float,type_none} j_type;
static char* type_names[]={"integer","string","boolean","float","none"};

typedef enum{
	ste_var,
	ste_const,
	ste_routine,
	ste_undefined} ste_entry_type;

struct SymbolTableEntry{
	char* name;
	SymbolTableEntry* next;
	ste_entry_type entry_type;
	int offset;
	union{
		struct{
			j_type type;
		}var;
		struct{
			int value;
		}constant;
		struct{
			j_type result_type;
		}routine;
	}f;
};

typedef enum
{
		/*Literals*/
		lx_identifier, lx_integer, lx_string, lx_float,
		/*keywords*/
		kw_program,
		kw_var, kw_constant, kw_integer, kw_boolean, kw_string, kw_float,
		kw_true, kw_false, kw_if, kw_fi, kw_then, kw_else,
		kw_while, kw_do, kw_od,
		kw_and, kw_or, kw_cand, kw_cor,
		kw_read, kw_write,
		kw_for, kw_from, kw_to, kw_by,
		kw_function, kw_procedure, kw_return, kw_not, kw_begin, kw_end,/*Operators*/
		lx_lparen, lx_rparen, lx_lbracket, lx_rbracket,
		lx_colon, lx_dot, lx_semicolon,lx_comma,lx_colon_eq,
		lx_plus,lx_minus,lx_star,lx_slash,
		lx_eq, lx_neq, lx_lt, lx_le, lx_gt, lx_ge, lx_eof
}
LEXEME_TYPE;

struct TOKEN
{
	LEXEME_TYPE type;
	int value;//can be used istead of str_ptr for IDs and strings
	float float_value;
	char *str_ptr;//points to strings or Identifiers, can use value
				 //instead but with type casting
};

#define keys 32
static char * keyword[] = {
	"and", "begin", "boolean", "by", "cand", "constant", "cor",
		"do", "else", "end", "false", "fi", "float", "for", "from",
		"function", "if", "integer", "not","od", "or", "procedure",
		"program", "read", "return", "string", "then", "to", "true",
		"var", "while", "write"
};

static LEXEME_TYPE key_type[] = {
		kw_and, kw_begin, kw_boolean, kw_by, kw_cand, kw_constant,
		kw_cor, kw_do, kw_else, kw_end, kw_false, kw_fi, kw_float,
		kw_for, kw_from, kw_function, kw_if, kw_integer, kw_not,
		kw_od, kw_or, kw_procedure, kw_program, kw_read, kw_return,
		kw_string, kw_then, kw_to, kw_true, kw_var, kw_while, kw_write
};





typedef SymbolTableEntry  symbol_table_entry;
typedef struct ast_list_cell 
{
 struct ast_node *head; /* Recursive datatype definition */
 struct ast_list_cell *tail;
} ast_list;
typedef struct ste_list_cell
{
 symbol_table_entry *head;
 struct ste_list_cell *tail;
} ste_list;

////////////////////////////////////////////////////////////
/* Type of an AST node. */
typedef enum
{
	ast_var_decl,		 /* Top-level variable declaration */
	ast_const_decl,		 /* II constant declaration */
	ast_routine_decl,	 /* II procedure or function decl. */
	ast_assign,			 /* Assignment statement */
	ast_if,				 /* If-then-else statement */
	ast_while,			 /* While statement */
	ast_for,			 /* For statement */
	ast_read,			 /* Read statement */
	ast_write,			 /* Write statement */
	ast_call,			 /* Procedure call */
	ast_block,			 /* Begin-end block */
	ast_return,			 /* Return statement */
	ast_var,			 /* Variable reference */
	ast_integer,		 /* Integer literal */
	ast_string,			 /* String literal */
	ast_boolean,		 /* Boolean literal */
	ast_times,			 /*  operator */
	ast_divide,			/* / operator */
	ast_plus,			 /* + operator */
	ast_minus,			 /* -operator */
	ast_eq,				/* = operator */
	ast_neq,			/* != operator */

	ast_lt, /* < operator */
	ast_le, /* <= operator */
	ast_gt, /* > operator */
	ast_ge, /* >= operator */
	ast_and, /* and operator */
	ast_or, /* or operator */
	ast_cand, /* cand operator */
	ast_cor, /* cor operator */
	ast_not, /* not operator */
	ast_uminus, /* unary -operator */
	ast_eof, /* End of File */
	ast_float, /* float */
	ast_itof, /* convert integr to float */
}AST_type;

///////////////////////////////////////////////////////////////////////
/* The actual datatype for an abstract syntax tree (AST) node. */
typedef struct ast_node
{
 AST_type      type; /* Type of the AST node */
 union /* The fields depend on a node's type */
	{
	 struct{
			symbol_table_entry *name; /* Variable's symbol table entry */
			j_type type; /* Declared type of variable */
	 } a_var_decl;
	
	 struct{
			symbol_table_entry *name; /* Constant's symbol table entry */
			int value; /* Evaluated value of constant */
	 } a_const_decl;
	
	 struct{
			symbol_table_entry *name; /* Routine's symbol table entry */
			ste_list *formals; /* List of formal parameters */
			j_type result_type; /* Type of result (none_type for procedures) */
			struct ast_node *body; /* Body of routine */
	} a_routine_decl;
	
	 struct{
		     symbol_table_entry *lhs; /* Target of assignment */
			 struct ast_node *rhs; /* Right side of assignment */
	} a_assign;
	
	 struct{
			struct ast_node *predicate; /* Predicate */
			struct ast_node *conseq; /* Consequent */
			struct ast_node *altern; /* Alternative (NULL for if-then stmt) */
	} a_if;
	
	struct{
			struct ast_node *predicate; /* Predicate */
			struct ast_node *body; /* Body */
	} a_while;
	
	struct {
			symbol_table_entry *var; /* Index variable */
			struct ast_node *lower_bound; /* Lower iteration bound */
			struct ast_node *upper_bound; /* Upper iteration bound */
			struct ast_node *body; /* Body */
	}a_for;
	
	struct{
			symbol_table_entry *var; /* Target variable */
	} a_read;

	struct{
				symbol_table_entry *var; /* Source variable */
	} a_write;

	struct{
			symbol_table_entry *callee; /* Symbol table entry for function */
			ast_list *arg_list; /* List of actual arguments */
	} a_call;

	struct{
		   ste_list *vars; /* Symbol table entries of local variables */
		  ast_list *stmts; /* Statements in block */
	} a_block;

	struct{
		struct ast_node *expr; /* Return value */
	} a_return;

	struct{
		symbol_table_entry *var; /* Symbol table entry for variable */
	} a_var;

	struct{
		int value; /* Integer value */
	} a_integer;

	struct{
		float value;
	} a_float;

	struct{
		char *string; /* String value */
	} a_string;

	struct{
		int value; /* Boolean value */
	}a_boolean;

	struct{ 
		struct ast_node *arg; /* Argument */
		j_type type;
	} a_unary_op;

	struct{
		struct ast_node *larg; /* Argument 1 */
		struct ast_node *rarg; /* Argument 2 */
		j_type type;
		j_type rel_type;
	} a_binary_op;

	struct {
		struct ast_node *arg;
	} a_itof; 
  
 } f;  // union 
} AST; // ASt structure



#endif