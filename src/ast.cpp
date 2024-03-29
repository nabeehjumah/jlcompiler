#include "ast.h"

AST * make_ast_node (AST_type type, ...) 
{
	AST *n = new AST; // change to new
	va_list ap;
	

	if (n == NULL) fatal_error ();
	
	va_start( ap, type );     /* Initialize variable arguments. */
	n->type = type;
		
	switch (type) {
		case ast_var_decl:
			n->f.a_var_decl.name = va_arg (ap, symbol_table_entry *);
			n->f.a_var_decl.type = va_arg (ap, j_type);		
			break;

		case ast_const_decl:
			n->f.a_const_decl.name = va_arg (ap,symbol_table_entry *);
			n->f.a_const_decl.value = va_arg (ap, int); 
			break;

		case ast_routine_decl:
			n->f.a_routine_decl.name = va_arg (ap, symbol_table_entry *);
			n->f.a_routine_decl.formals = va_arg (ap, ste_list *);
			n->f.a_routine_decl.result_type =va_arg (ap, j_type );
			n->f.a_routine_decl.body = va_arg (ap, AST *);
			break;

		case ast_assign:		
			n->f.a_assign.lhs = va_arg (ap, symbol_table_entry *);	
			n->f.a_assign.rhs = va_arg (ap, AST *);
			break;

		case ast_if:
			n->f.a_if.predicate = va_arg (ap, AST *);
			n->f.a_if.conseq = va_arg (ap, AST *);
			n->f.a_if.altern = va_arg (ap, AST *);
			break;

		case ast_while:
			n->f.a_while.predicate = va_arg (ap, AST *);
			n->f.a_while.body = va_arg (ap, AST *);
			break;

		case ast_for:
			n->f.a_for.var = va_arg (ap, symbol_table_entry *);
			n->f.a_for.lower_bound = va_arg(ap, AST *);
			n->f.a_for.upper_bound = va_arg (ap, AST *);
			n->f.a_for.body = va_arg (ap, AST *);
			break;

		case ast_read: //page 3
			n->f.a_read.var = va_arg (ap, symbol_table_entry *);
			break;

		case ast_write:
			n->f.a_write.var = va_arg (ap, symbol_table_entry *);
			break;

		case ast_call:
			n->f.a_call.callee = va_arg (ap, symbol_table_entry *);
			n->f.a_call.arg_list = va_arg (ap, ast_list *);
			break;

		case ast_block:
			n->f.a_block.vars = va_arg (ap, ste_list *);
			n->f.a_block.stmts = va_arg (ap, ast_list *);
			
		break;

		case ast_return:
			n->f.a_return.expr = va_arg (ap, AST *);
			break;

		case ast_var:
			n->f.a_var.var = va_arg (ap, symbol_table_entry *);
			break;

		case ast_integer:
			n->f.a_integer.value = va_arg (ap, int);
			break;

		case ast_float:
			n->f.a_float.value = va_arg (ap, float);
			break;
		case ast_string:
			n->f.a_string.string = va_arg (ap, char *);
			break;
		case ast_boolean:
			n->f.a_boolean.value = va_arg (ap, int);
			break;

		case ast_times:
		case ast_divide:
		case ast_plus:
		case ast_minus:
		case ast_eq:
		case ast_neq:
		case ast_lt:
		case ast_le:
		case ast_gt:
		case ast_ge:
		case ast_and:
		case ast_or:
		case ast_cand:
		case ast_cor:
			n->f.a_binary_op.larg = va_arg (ap, AST *);
			n->f.a_binary_op.rarg = va_arg (ap, AST *);
			break;

		case ast_not:
		case ast_uminus:
			n->f.a_unary_op.arg = va_arg (ap, AST *);
			break;
    // new page page 4
		case ast_itof:
			n->f.a_itof.arg = va_arg (ap, AST *);
			break;
		case ast_eof:
			break;
		default:
			fatal_error ();
	}
	return (n);
}

void fatal_error()
{
	printf("erorr\n");
}


void print_ast_node (FILE *f, AST *n)
{
	  p_a_n (f, n, 0);
}


 void p_a_n (FILE *f, AST *n, int d)
{
if (n == NULL) return;
switch (n->type)
{
// new page 6
	case ast_var_decl:
		fprintf (f,"var %s: %s;", ste_name (n->f.a_var_decl.name),
                    type_names [n->f.a_var_decl.type]);
		 nl_indent (f, d);
		break;
	case ast_const_decl:
		fprintf (f, "constant %s = %d;", ste_name (n->f.a_const_decl.name),
					n->f.a_const_decl.value);
		nl_indent (f, d); 
		break;
	case ast_routine_decl:
		{ 
			if (n->f.a_routine_decl.result_type == type_none)
				fprintf (f, "procedure %s (", ste_name (n->f.a_routine_decl.name));
			else 
				fprintf (f, "function %s (", ste_name (n->f.a_routine_decl.name));
			print_ste_list (f, n->f.a_routine_decl.formals, "", ", ", -1);
			if (n->f.a_routine_decl.result_type == type_none){
				fprintf (f, ")");
				nl_indent (f, d + 2);
			} else{
					fprintf (f, ") : %s",
						type_names [n->f.a_routine_decl.result_type]);
					nl_indent (f, d + 2);
			} 
			p_a_n (f, n->f.a_routine_decl.body, d + 2);
			fprintf (f, ";");
			nl_indent (f, d);
		    break;
		}
	case ast_assign:
		fprintf (f, "%s := ", ste_name (n->f.a_assign.lhs));
		p_a_n (f, n->f.a_assign.rhs, d);
		break;
	case ast_if:
		fprintf (f, "if ");
		p_a_n (f, n->f.a_if.predicate, d);
		fprintf (f, " then");
		nl_indent (f, d + 2);
		p_a_n (f, n->f.a_if.conseq, d + 2);
		if (n->f.a_if.altern != NULL){ 
			nl_indent (f, d);
			fprintf (f, "else");
			nl_indent (f, d + 2);
			p_a_n (f, n->f.a_if.altern, d + 2);
		}
	    break;
	case ast_while:
		fprintf (f, "while ");
		p_a_n (f, n->f.a_while.predicate, d);
		fprintf (f, " do");
		nl_indent (f, d + 2);
		p_a_n (f, n->f.a_while.body, d + 2);
		nl_indent (f, d);
		fprintf (f, "od");// new page
		break;
	case ast_for:
		fprintf (f, "for %s = ", ste_name (n->f.a_for.var));
		p_a_n (f, n->f.a_for.lower_bound, d); 
		fprintf (f, " to "); 
        p_a_n (f, n->f.a_for.upper_bound, d);
		fprintf (f, " do");
		nl_indent (f, d + 2);
		p_a_n (f, n->f.a_for.body, d + 2);
		nl_indent (f, d);
		fprintf (f, "od");
		break;
	case ast_read:
		fprintf (f, "read (%s)", ste_name (n->f.a_read.var));
		break; 
	case ast_write:
		fprintf (f, "write (%s)", ste_name (n->f.a_read.var));
		break; 
	case ast_call:
		fprintf(f, "%s (", ste_name (n->f.a_call.callee));
		print_ast_list (f, n->f.a_call.arg_list, ", ", -1);
		fprintf (f, ")");
		break;
	case ast_block:
		fprintf (f, "begin");
		nl_indent (f, d + 2);
		print_ste_list (f, n->f.a_block.vars, "var ", "", d + 2);
		print_ast_list (f, n->f.a_block.stmts, ";", d + 2);
		nl_indent (f, d);
		fprintf (f, "end");
	    break;
	case ast_return:
		fprintf (f,"return (");
		p_a_n (f, n->f.a_return.expr, d);
		fprintf (f, ")");
		break;
	case ast_var:
		fprintf (f, "%s", ste_name (n->f.a_var.var));
		break; 
	case ast_integer:
		fprintf (f, "%d", n->f.a_integer.value);
		break; 
	case ast_float:
		fprintf (f, "%f", n->f.a_float.value);
		break; 
	case ast_string:
		fprintf (f, "\"%s\"", n->f.a_string.string);
		break;
	case ast_boolean:
		fprintf (f, n->f.a_boolean.value ? "true" : "false");
		break;
	case ast_times:
	case ast_divide: // new page
	case ast_plus:
	case ast_minus:
	case ast_eq:
	case ast_neq:
	case ast_lt:
	case ast_le:
	case ast_gt:
	case ast_ge:
	case ast_and:
	case ast_or:
	case ast_cand:
	case ast_cor:
	  fprintf (f, "(");
	  p_a_n (f, n->f.a_binary_op.larg, d);
	  switch (n->type) {
		case ast_times:
				fprintf (f, " * ");
				break;
		case ast_divide:
			fprintf (f, " / ");
			break;
		case ast_plus:
			fprintf (f, " + ");
			break;
		case ast_minus:
			fprintf (f, " -");
			break;
		case ast_eq:
			fprintf (f, " = ");
			break;
		case ast_neq:
			fprintf (f, " != ");
			break;
		case ast_lt:
			fprintf (f, " < ");
			break;
		case ast_le:
			fprintf (f, " <= ");
			break;
		case ast_gt:
			fprintf (f, " > ");
			break;
		 case ast_ge:
			fprintf (f, " >= ");
			break;
		case ast_and:
			fprintf (f, " and ");
			break;
		case ast_or:
			fprintf (f, " or ");
			break; // new page
		case ast_cand:
			fprintf (f, " cand ");
			break;
		case ast_cor:
			fprintf (f, " cor ");
			break;
		default:
		break;
       }
	   p_a_n (f, n->f.a_binary_op.rarg, d);
	   fprintf (f, ")");
	   break;
	case ast_not:
		fprintf (f, "(not ");
		p_a_n (f, n->f.a_unary_op.arg, d);
		fprintf (f, ")");
		break;
	case ast_uminus:
		fprintf (f, "( -");
		p_a_n (f, n->f.a_unary_op.arg, d);
		fprintf (f, ")");
		break;
	case ast_eof:
		fprintf (f, "EOF");
		break;
	default:
		fatal_error ();
  } 
}
///////////////////////////////////////////////////
/* Print a list of AST nodes. */
j_type ste_var_type(symbol_table_entry*n)
{
return n->f.var.type;
}
 void print_ste_list (FILE *f, ste_list *L, char *prefix, char *sep, int d)
{ 
	for ( ; L != NULL; L= L->tail) {
			fprintf (f, "%s%s : %s", prefix, ste_name (L->head),
						type_names [ste_var_type(L->head)]);
			if (L->tail || d >= 0) fprintf (f, sep); // new page 10
			if (d >= 0) nl_indent (f, d);


   }

}
/* Print a newline and indent D space. */
 void nl_indent (FILE *f, int d)
{
	
	fprintf (f, "\n");
	while (d > 0) {fprintf (f, " ");d--;}
}

/* Print a list of AST nodes. */
 void print_ast_list (FILE *f, ast_list *L, char *sep, int d)
{
	for ( ; L != NULL; L = L->tail){
		p_a_n (f, L->head, d);
		if (L->tail || d>0){
			fprintf (f, sep);
			if (L->tail && d >= 0) nl_indent (f, d);
		}
	}

}
////////////////////////////////////////////////////////////////
char *ste_name(symbol_table_entry * ent){
	char*name=new char[1024];
	strcpy(name,ent->name);
	return name; }