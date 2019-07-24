#include "generator.h"

NGenerator::NGenerator(char* file)
{
	parser=new NParser(file);
	tree=parser->Parse();
	print_ast_list(stdout,tree," ",5);
	f=fopen("out.asm","w");
}

NGenerator::~NGenerator()
{
	fclose(f);
	delete parser;
}

void NGenerator::Generate()
{
	ast_list* p=tree;
	for ( ; p != NULL; p = p->tail){
		GenerateNode(p->head);
	}
}

void NGenerator::GenerateNode(AST *n)
{int d=10;
if (n == NULL) return;
char *l;
switch (n->type)
{
	case ast_var_decl:
		n->f.a_var_decl.name->offset=-1;
		fprintf(f,"\n.data");
		fprintf (f,"\n%s: .word 1\n", GenVar(n->f.a_var_decl.name));
		break;
	case ast_const_decl:
		fprintf(f,"\n.data");
		fprintf (f,"\n%s: .word 1", GenVar(n->f.a_const_decl.name));
		fprintf (f,"\n li $t0,%d",n->f.a_const_decl.value);
		fprintf (f,"\n sw $t0,%s\n",GenVar(n->f.a_const_decl.name));
		break;
	case ast_routine_decl:
		{
			fprintf(f,"\n.text");
			fprintf(f,"\n%s:",ste_name (n->f.a_routine_decl.name));
			fprintf(f,"\n addiu $sp,$sp,-4 #push return address");
			fprintf(f,"\n sw $ra,($sp)");
			fprintf(f,"\n addiu $sp,$sp,-4 #push frame pointer");
			fprintf(f,"\n sw $fp,($sp)");
			offset=0;
			ste_list* p=n->f.a_routine_decl.formals;
			while(p){
				p->head->offset=offset;
				offset+=4;
				p=p->tail;
			}
			retOff=offset;
			offset+=8;//jump+fp
			fprintf(f,"\n addiu $fp,$sp,%d #formals space",offset);
			//body
			GenerateNode(n->f.a_routine_decl.body);
			//cleanup and return
			fprintf(f,"\n move $sp,$fp #clean up");
			fprintf(f,"\n lw $t0,%d($fp)",-4-retOff);
			fprintf(f,"\n lw $fp,%d($fp)",-8-retOff);
			fprintf(f,"\n jr $t0");

			//end mark
			fprintf (f,"\n\n");
		    break;
		}
	case ast_assign:
		fprintf(f,"\n #assignment");
		GenerateNode(n->f.a_assign.rhs);
		fprintf (f,"\n sw $v1,%s", GenVar(n->f.a_assign.lhs));
		break;
	case ast_if:
		{
		fprintf(f,"\n #if statement");
		l=Label();
		char* l2=Label();
		GenerateNode(n->f.a_if.predicate);
		fprintf (f,"\n beqz $v1,%s",l);
		GenerateNode(n->f.a_if.conseq);
		fprintf (f,"\n j %s",l2);
		fprintf (f,"\n%s:",l);
		GenerateNode(n->f.a_if.altern);
		fprintf (f,"\n%s:",l2);
		}
	    break;
	case ast_while:
		{
		fprintf(f,"\n #while loop");
		l=Label();
		char* l2=Label();
		fprintf (f,"\n%s:",l);
		GenerateNode(n->f.a_while.predicate);
		fprintf (f,"\n beqz $v1,%s",l2);
		GenerateNode(n->f.a_while.body);
		fprintf (f,"\n j %s",l);
		fprintf (f,"\n%s:",l2);
		}
		break;
	case ast_for:
		fprintf(f,"\n #for loop");
		char*l2;
		l=Label();
		l2=Label();
		GenerateNode(n->f.a_for.lower_bound);
		fprintf(f,"\n sw $v1,%s",GenVar(n->f.a_for.var));
		
		fprintf(f,"\n%s:",l);
		GenerateNode(n->f.a_for.body);

        GenerateNode(n->f.a_for.upper_bound);
		fprintf(f,"\n lw $t0,%s",GenVar(n->f.a_for.var));
		fprintf(f,"\n beq $t0,$v1,%s",l2);
		fprintf(f,"\n lw $t0,%s",GenVar(n->f.a_for.var));
		fprintf(f,"\n addiu $t0,1");
		fprintf(f,"\n sw $t0,%s",GenVar(n->f.a_for.var));
		fprintf(f,"\n j %s",l);
		fprintf(f,"\n%s:",l2);
		break;
	case ast_read:
		fprintf(f,"\n #read variable");
		switch(n->f.a_read.var->f.var.type){
		case type_integer:
			fprintf(f,"\n li $v0,5");
			fprintf(f,"\n syscall");
			fprintf(f,"\n sw $v0,%s",GenVar(n->f.a_read.var));
			break;
		case type_string:
			{
				fprintf(f,"\n li $a0,1000");
				fprintf(f,"\n li $v0,9");
				fprintf(f,"\n syscall");
				fprintf(f,"\n sw $v0,%s",GenVar(n->f.a_read.var));
				fprintf(f,"\n lw $a0,%s",GenVar(n->f.a_read.var));
				fprintf(f,"\n li $a1,1000");
				fprintf(f,"\n li $v0,8");
				fprintf(f,"\n syscall");
			}
			break;
		}
		break; 
	case ast_write:
		fprintf(f,"\n #write variable");
		switch(n->f.a_write.var->f.var.type){
		case type_integer:
			fprintf(f,"\n lw $a0,%s",GenVar(n->f.a_read.var));
			fprintf(f,"\n li $v0,1");
			fprintf(f,"\n syscall");
			break;
		case type_string:
			fprintf(f,"\n lw $a0,%s",GenVar(n->f.a_read.var));
			fprintf(f,"\n li $v0,4");
			fprintf(f,"\n syscall");
			break;
		}
		break; 
	case ast_call:
	{
		fprintf(f,"\n #call statement");
		ast_list*p;
		p=n->f.a_call.arg_list;
		while(p){
			GenerateNode(p->head);//push args
			fprintf(f,"\n addiu $sp,$sp,-4");
			fprintf(f,"\n sw $v1,($sp)");
			p=p->tail;
		}
		fprintf(f,"\n jal %s",n->f.a_call.callee->name);
//		fprintf(f,"\n nop");
//		fprintf(f,"\n move $v1,$v0");
		break;
	}
	case ast_block:
	{
		int locals=0;
		ste_list* v=n->f.a_block.vars;
		for(;v!=NULL;v=v->tail){
			v->head->offset=offset;
			offset+=4;
			locals+=4;
		}
		fprintf(f,"\n addiu $sp,$sp,%d #locals space",-1*locals);
		ast_list *p;
		p=n->f.a_block.stmts;
		for(;p!=NULL;p=p->tail){
			GenerateNode(p->head);
		}
	    break;
	}
	case ast_return:
		fprintf(f,"\n #return statement");
		GenerateNode(n->f.a_return.expr);

//		fprintf(f,"\n move $v0,$v1");
		fprintf(f,"\n move $sp,$fp");

		fprintf(f,"\n lw $t0,%d($fp)",-4-retOff);
		fprintf(f,"\n lw $fp,%d($fp)",-8-retOff);
		fprintf(f,"\n jr $t0");
		break;
	case ast_var:
		fprintf (f,"\n lw $v1,%s", GenVar(n->f.a_var.var));
		break; 
	case ast_integer:
		fprintf (f,"\n li $v1,%d", n->f.a_integer.value);
		break; 
	case ast_string:
		char* str_label;
		str_label=Label();
		l=Label();

		fprintf(f,"\n j %s",l);
		fprintf(f,"\n.data");
		fprintf (f, "\n%s: .asciiz \"%s\"",str_label,n->f.a_string.string);
		fprintf(f,"\n.text");
		fprintf(f,"\n%s:",l);
		fprintf (f, "\n la $v1,%s",str_label);
		break;
	case ast_boolean:
		fprintf (f,"\n li $v1,%d", n->f.a_boolean.value);
		break;
	case ast_times:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf (f,"\n mul $v1,$t0,$v1");
		break;
	case ast_divide:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf (f,"\n div $v1,$t0,$v1");
		break;
	case ast_plus:
		if(parser->expr_type(n)==type_string)
		{
		GenerateNode(n->f.a_binary_op.larg);
		fprintf (f,"\n move $a1,$v1");
		GenerateNode(n->f.a_binary_op.rarg);
		fprintf (f,"\n move $a2,$v1");

		fprintf(f,"\n li $a0,1000");
		fprintf(f,"\n li $v0,9");
		fprintf(f,"\n syscall");

		fprintf (f,"\n move $v1,$v0");//catenated address
		l=Label();
		char* l2=Label();
		fprintf (f,"\n%s:",l);
		fprintf (f,"\n lb $t0,($a1)");
		fprintf (f,"\n beqz $t0,%s",l2);
		fprintf (f,"\n sb $t0,($v0)");
		fprintf (f,"\n addiu $a1,$a1,1");
		fprintf (f,"\n addiu $v0,$v0,1");
		fprintf (f,"\n j %s",l);
		fprintf (f,"\n%s:",l2);

		l=Label();
		l2=Label();
		fprintf (f,"\n%s:",l);
		fprintf (f,"\n lb $t0,($a2)");
		fprintf (f,"\n beqz $t0,%s",l2);
		fprintf (f,"\n sb $t0,($v0)");
		fprintf (f,"\n addiu $a2,$a2,1");
		fprintf (f,"\n addiu $v0,$v0,1");
		fprintf (f,"\n j %s",l);
		fprintf (f,"\n%s:",l2);
		
		fprintf (f,"\n li $t0,0");
		fprintf (f,"\n sb $t0,($v0)");
		}
		else
		{
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf (f,"\n add $v1,$t0,$v1");
		}
		break;
	case ast_minus:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf (f,"\n sub $v1,$t0,$v1");
		break;
	case ast_cand:
	case ast_and:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf (f,"\n and $v1,$t0,$v1");
		break;
	case ast_cor:
	case ast_or:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf (f,"\n or $v1,$t0,$v1");
		break;
	case ast_eq:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);
		fprintf(f,"\n move $t1,$v1");

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf(f,"\n li $v1,0");
		fprintf (f,"\n seq $v1,$t0,$t1");
		break;
	case ast_neq:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);
		fprintf(f,"\n move $t1,$v1");

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf(f,"\n li $v1,0");
		fprintf (f,"\n sne $v1,$t0,$t1");
		break;
	case ast_lt:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);
		fprintf(f,"\n move $t1,$v1");

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf(f,"\n li $v1,0");
		fprintf (f,"\n slt $v1,$t0,$t1");
		break;
	case ast_le:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);
		fprintf(f,"\n move $t1,$v1");

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf(f,"\n li $v1,0");
		fprintf (f,"\n sle $v1,$t0,$t1");
		break;
	case ast_gt:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);
		fprintf(f,"\n move $t1,$v1");

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf(f,"\n li $v1,0");
		fprintf (f,"\n sgt $v1,$t0,$t1");
		break;
	case ast_ge:
		GenerateNode(n->f.a_binary_op.larg);
		fprintf(f,"\n addiu $sp,$sp,-4");
		fprintf(f,"\n sw $v1,($sp)");
		GenerateNode(n->f.a_binary_op.rarg);
		fprintf(f,"\n move $t1,$v1");

		fprintf(f,"\n lw $t0,($sp)");
		fprintf(f,"\n addiu $sp,$sp,4");
		fprintf(f,"\n li $v1,0");
		fprintf (f,"\n sge $v1,$t0,$t1");
		break;
	case ast_not:
		GenerateNode(n->f.a_unary_op.arg);
		fprintf (f,"\n not $v1,$v1");
		break;
	case ast_uminus:
		GenerateNode(n->f.a_unary_op.arg);
		fprintf (f,"\n nrg $v1,$v1");
		break;
  } 
}

char* NGenerator::Label()
{
	static int i=0;
	i++;
	char* r=new char[10];
	sprintf(r,"label%04X",i);
	return r;
}
char *NGenerator::GenVar(symbol_table_entry * e)
{
	char* r;
	if(e->offset==-1)
		return e->name;
	else{
		r=new char[10];
		sprintf(r,"%d($fp) #%s",-4 -(e->offset),e->name);
		return r;
	}
}
