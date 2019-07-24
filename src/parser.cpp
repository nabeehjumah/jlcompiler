#include "parser.h"
j_type return_type;
int return_ok;
NParser :: NParser(char* file)
{
	scanner = new NScanner(file);
	lookahead=scanner->Scan();
	st=new NStack();
	root=NULL;
};
NParser::~NParser()
{
	delete scanner;
}

void NParser::Error(char* error)
{
	scanner->Error(error);
}
ast_list* NParser::Parse()
{
	while(lookahead)
		decl();
	return root;
}
 
int NParser::match(int type)
{
	if(type==lookahead->type)
	{   
		current=lookahead;
    lookahead=scanner->Scan();
	return 1;
	}
	else
	{
	return 0;
	}
}

	int NParser::decl()
	{
		ast_list *cell;
		AST *n;
		symbol_table_entry *ste;
		char* name=new char[1024];
	
		if(match(kw_var))
		{
		 int r;
		 r = match(lx_identifier);
		 if(r==0)Error("identifier expected");
		 strcpy(name,current->str_ptr);
		 ste = st->PutSymbol(name);
		 if(!ste)Error("name is already used");
		 r = match(lx_colon);
		 if(r==0)Error("[:] expected");
		 r=type();
		 if(r==type_none)Error("data type expected");
		 ste->f.var.type=(j_type)r;
		 n = make_ast_node(ast_var_decl ,ste,r);
		 cell=root;
		 if(root==NULL)
		 {
			 root=cell=new ast_list;
			cell->tail=NULL;
			cell->head=n;
		 }
		 else{
		 while(cell->tail)cell=cell->tail;
		 cell->tail= new ast_list;
		 cell=cell->tail;
		 cell->head=n;
		 cell->tail=NULL;
		 }
		 return 1;
		 }


		else if(match(kw_constant))
		{
		 int r;
		 r = match(lx_identifier);
		 if(r==0)Error("identifier expected");
		 strcpy(name,current->str_ptr);
 		 ste = st->PutSymbol(name);
		 ste->offset=-1;//like global
		 if(!ste)Error("name is already used");
		 r = match(lx_eq);
		 if(r==0)Error("[=] expected");
		 AST* s=expr();
		 if(s==NULL)Error("expression expected");
		 ste->f.constant.value=s->f.a_integer.value;
		 n = make_ast_node(ast_const_decl ,ste,s->f.a_integer.value);
		 cell=root;
		 if(root==NULL)
		 {
			 root=cell=new ast_list;
			cell->tail=NULL;
			cell->head=n;
		 }
		 else{
		 while(cell->tail)cell=cell->tail;
		 cell->tail= new ast_list;
		 cell=cell->tail;
		 cell->head=n;
		 cell->tail=NULL;
		 }

		 return 1;
		}


		else if(match(kw_function))
		{
		 return_ok=0;
		 int r;
		 char*name=new char[1024];
		 r = match(lx_identifier);
		 if(r==0)Error("identifier expected");
		 strcpy(name,current->str_ptr);
		 ste = st->PutSymbol(name);
		 if(!ste)Error("name is already used");
		 st->AddLevel();
		 ste_list* a= formal_list();

		 r=match(lx_colon);
		 if(r==0)Error("[:] expected");

		 r=type();
		 if(r==type_none)Error("data type expected");
		 ste->f.routine.result_type=(j_type)r;
		 return_type=(j_type)r;

		 n = make_ast_node(ast_routine_decl ,ste,a,r,NULL);
		 cell=root;
		 if(root==NULL)
		 {
			 root=cell=new ast_list;
			cell->tail=NULL;
			cell->head=n;
		 }
		 else{
		 while(cell->tail)cell=cell->tail;
		 cell->tail= new ast_list;
		 cell=cell->tail;
		 cell->head=n;
		 cell->tail=NULL;
		 }

		 AST* b=block();
		 if(!b)Error("[begin..end] block expected");
		 n->f.a_routine_decl.body=b;
		 st->RemoveLevel();

		 if(return_ok==0)Error("function should return value");

		 return 1;
		}
		else if(match(kw_procedure))
		{
		 return_ok=0;
		 int r;
		 char*name=new char[1024];
		 r = match(lx_identifier);
		 if(r==0)Error("identifier expected");
		 strcpy(name,current->str_ptr);
		 ste = st->PutSymbol(name);
		 if(!ste)Error("name is already used");
		 ste->f.routine.result_type=type_none;
		
		 st->AddLevel();
		 ste_list* a= formal_list();

		 n = make_ast_node(ast_routine_decl ,ste,a,type_none,NULL);
		 cell=root;
		 if(root==NULL)
		 {
			 root=cell=new ast_list;
			cell->tail=NULL;
			cell->head=n;
		 }
		 else{
		 while(cell->tail)cell=cell->tail;
		 cell->tail= new ast_list;
		 cell=cell->tail;
		 cell->head=n;
		 cell->tail=NULL;

		 AST* b=block();
		 if(!b)Error("[begin..end] block expected");
		 n->f.a_routine_decl.body=b;
		 st->RemoveLevel();

 		 if(return_ok!=0)Error("procedure should not return any value");
		 }

		 return 1;
		}
		if(!lookahead)
			return 0;
		else
			Error("unexpected code out of blocks");
	};
	int NParser::type()
	{
		int r;
		r= match(kw_integer);
		if(r==1) return type_integer;
		r= match(kw_boolean);
		if(r==1) return type_boolean ;
		r=match(kw_string);
		if(r==1)return type_string;
		return type_none;

	};
	ste_list* NParser::formal_list()
	{
		int r;
		ste_list* a;
		r= match(lx_lparen);
		if(r==0)Error("[(] expected");
		r = match(lx_rparen);
		if(r==1) return NULL;
		a = formals();
		r = match(lx_rparen);
		if(r==0) Error("[)] expected");
		return a;
	};

	ste_list* NParser::formals()
	{
		int r;
		ste_list* a=NULL;
		int done=0;
		while(!done)
		{
			r=match(lx_identifier);
			if(r==0)Error("identifier expected");
			char* name=new char[1024];
			strcpy(name,current->str_ptr);
			symbol_table_entry* ste;
//			ste=new symbol_table_entry;
			ste = st->PutSymbol(name);
			if(!ste)Error("name is already used");
			r = match(lx_colon);
			if(r==0)Error("[:] expected");
			r = type();
			if(r==type_none)Error("data type expected");
			ste->f.var.type=(j_type)r;
			r = match(lx_comma);
			if(r==0)done=1;
			if(a==NULL)
			{
				a=new ste_list;
				a->tail=NULL;
				a->head=ste;
			}
			else
			{
				ste_list* b;
				b=a;
				while(b->tail)
					b=b->tail;
				b->tail=new ste_list;
				b=b->tail;
				b->tail=NULL;
				b->head=ste;
			}
		}
		return a;
	};
	AST* NParser::stmt()
	{
		int r;
		AST* a;
		r = match(lx_identifier);
		if(r==1)
		{
			char *name=new char[1024];
			strcpy(name,current->str_ptr);
			symbol_table_entry *ste;
			ste=st->GetSymbol(name);
			if(ste==NULL)Error("undeclared identifier");
			r = match(lx_colon_eq);
			if (r == 0)
			{
				ast_list*b = arg_list();
				a=make_ast_node(ast_call,ste,b);
				
				ast_list* p=root;
				while(p)
				{
					if(p->head->type==ast_routine_decl
						&& p->head->f.a_routine_decl.name==ste)break;
					p=p->tail;
				}
				ste_list* formals=p->head->f.a_routine_decl.formals;
				while(b || formals)
				{
					if(!b || !formals)
						Error("incompatible number of arguments for call");
					if(expr_type(b->head)!=formals->head->f.var.type)
						Error("type mismatch in call arguments");
					b=b->tail;
					formals=formals->tail;
				}
				return a;
			}
			AST* b=expr();
			a=make_ast_node(ast_assign,ste,b);
			if(ste->f.var.type!=expr_type(b))Error("type mismatch");
			return a;
		}

		r = match(kw_if);
		if(r == 1)
		{
			AST* b=expr();
			if(expr_type(b)!=type_boolean)Error("if condition should be boolean");
			r = match(kw_then);
			if(r == 0) Error("[then] expected");
			AST* c= stmt();
			r = match(kw_fi);
			if(r==1)
			{
				a=make_ast_node(ast_if,b,c,NULL);
				return a;
			}
			r = match(kw_else);
			if(r==0)Error("[else] expected");
			AST* d = stmt();
			r = match(kw_fi);
			if(r == 0) Error("[fi] expected");
			a=make_ast_node(ast_if,b,c,d);
			return a;
		}
		r = match(kw_while);
		if(r==1)
		{
			AST*b=expr();
			if(expr_type(b)!=type_boolean)Error("while condition should be boolean");
			r=match(kw_do);
			if(r==0)Error("[do] expected");
			AST*c=stmt();
			r=match(kw_od);
			if(r==0)Error("[od] expected");
			a=make_ast_node(ast_while,b,c);
			return a;
		}
		r= match(kw_for);
		if(r==1)
		{
			r=match(lx_identifier);
			if(r==0)Error("identifier expected");
			char *name=new char[1024];
			strcpy(name,current->str_ptr);
			symbol_table_entry *ste;
			ste=st->GetSymbol(name);
			if(ste==NULL)Error("undeclared identifier");
			if(ste->f.var.type!=type_integer)Error("for index should be integer");
			r=match(lx_colon_eq);
			if(r==0)Error("[:=] expected");
			AST*b=expr();
			if(expr_type(b)!=type_integer)Error("min. index should be integer");
			r=match(kw_to);
			if(r==0)Error("[to] expected");
			AST*c=expr();
			if(expr_type(c)!=type_integer)Error("max. index should be integer");
			r=match(kw_do);
			if(r==0)Error("[do] expected");
			AST*d=stmt();
			r=match(kw_od);
			if(r==0)Error("[od] expected");
			a=make_ast_node(ast_for,ste,b,c,d);
			return a;
		}
		r = match(kw_read);
		if(r==1)
		{
			r = match(lx_lparen);
			if(r==0)Error("[(] expected");
			r = match(lx_identifier);
			if(r==0)Error("identifier expected");
			char *name=new char[1024];
			strcpy(name,current->str_ptr);
			symbol_table_entry *ste;
			ste=st->GetSymbol(name);
			if(ste==NULL)Error("undeclared identifier");
			r = match(lx_rparen);
			if(r==0)Error("[)] expected");
			a=make_ast_node(ast_read,ste);
			return a;
		}
		r = match(kw_write);
		if(r==1)
		{
			r = match(lx_lparen);
			if(r==0)Error("[(] expected");
			r = match(lx_identifier);
			if(r==0)Error("identifier expected");
			char *name=new char[1024];
			strcpy(name,current->str_ptr);
			symbol_table_entry *ste;
			ste=st->GetSymbol(name);
			if(ste==NULL)Error("undeclared identifier");
			r = match(lx_rparen);
			if(r==0)Error("[)] expected");
			a=make_ast_node(ast_write,ste);
			return a;
		}
		r = match(kw_return);
		if(r==1)
		{
			r = match(lx_lparen);
			if(r==0)Error("[(] expected");
			AST*b=expr();
			if(expr_type(b)!=return_type)Error("return type mismatch");
			r = match(lx_rparen);
			if(r==0)Error("[)] expected");
			a=make_ast_node(ast_return,b);
			return_ok=1;
			return a;
		}
		return block();
	};
	
	AST* NParser::block()
	{
		AST* a;
		int r;
		r = match(kw_begin);
		if(r==0) return NULL;
		st->AddLevel();
		ste_list*b = var_decl_list();
		ast_list*c = stmt_list();
		r = match(kw_end);
		if (r == 0) Error("[end] expected");
		st->RemoveLevel();
		a=make_ast_node(ast_block,b,c);
		return a;
	}
		;
	ste_list* NParser::var_decl_list()
	{
		ste_list*a=NULL;
		int r;
		while(match(kw_var))
		{
			r = match(lx_identifier);
			if(r==0)Error("identifier expected");
			char *name=new char[1024];
			strcpy(name,current->str_ptr);
			symbol_table_entry *ste;
			ste=st->PutSymbol(name);
			if(!ste)Error("name is already used");
			r = match(lx_colon);
			if(r==0)Error("[:] expected");
			r = type();
			ste->f.var.type=(j_type)r;
			r=match(lx_semicolon);
			if(r==0)Error("[;] expected");
			if(a==NULL)
			{
				a=new ste_list;
				a->tail=NULL;
				a->head=ste;
			}
			else
			{
				ste_list* b=a;
				while(b->tail)b=b->tail;
				b->tail=new ste_list;
				b=b->tail;
				b->tail=NULL;
				b->head=ste;
			}
		}
		return a;
	};
	ast_list* NParser::stmt_list()
	{
		int r;
		ast_list* a=NULL;
		AST* b;
		while(b=stmt())
		{
			r=match(lx_semicolon);
			if(r==0)Error("[;] expected");
			if(a==NULL)
			{
				a=new ast_list;
				a->tail=NULL;
				a->head=b;
			}
			else
			{
				ast_list* c=a;
				while(c->tail)c=c->tail;
				c->tail=new ast_list;
				c=c->tail;
				c->tail=NULL;
				c->head=b;
			}

		}
		return a;
	};
	ast_list * NParser::arg_list()
	{
		int r;
		ast_list *a;
		r= match(lx_lparen);
		if(r==0)Error("[(] expected");
		r = match(lx_rparen);
		if(r==1) return NULL;
		a = args();
		if(a==NULL) Error("arguments expected");
		r = match(lx_rparen);
		if(r==0) Error("[)] expected");
		return a;
	};
	ast_list * NParser::args()
	{
		int r;
		AST* a;
		ast_list* b=NULL;
		ast_list* c=NULL;
		int done=0;
		while(!done)
		{
			a=expr();
			if(!a)Error("expression expected");
			if(b==NULL)
			{
				b=new ast_list;
				b->tail=NULL;
				b->head=a;
			}
			else
			{
				c=b;
				while(c->tail)c=c->tail;
				c->tail=new ast_list;
				c=c->tail;
				c->tail=NULL;
				c->head=a;
			}
			r=match(lx_comma);
			if(r==0)
				done=1;
		}
		return b;
	};
	AST* NParser::expr()
	{
		AST* a;
		int r1,r2,r3,r4;
		AST* b=E1();
		expr_type(b);
		r1=match(kw_and);
		r2=match(kw_or);
		r3=match(kw_cand);
		r4=match(kw_cor);
		if(!r1 && !r2 && !r3 && !r4)
			return b;
		AST* c=expr();
		expr_type(c);
		if(r1)
			a=make_ast_node(ast_and,b,c);
		else if(r2)
			a=make_ast_node(ast_or,b,c);
		else if(r3)
			a=make_ast_node(ast_cand,b,c);
		else if(r4)
			a=make_ast_node(ast_cor,b,c);
		return a;
	}
	AST* NParser::E1()
	{
		AST* a;
		int r1,r2,r3,r4,r5,r6;
		AST* b=E2();
		expr_type(b);
		r1=match(lx_eq);
		r2=match(lx_neq);
		r3=match(lx_lt);
		r4=match(lx_le);
		r5=match(lx_gt);
		r6=match(lx_ge);
		if(!r1 && !r2 && !r3 && !r4 && !r5 && !r6)
			return b;
		AST* c=E1();
		expr_type(c);
		if(r1)
			a=make_ast_node(ast_eq,b,c);
		else if(r2)
			a=make_ast_node(ast_neq,b,c);
		else if(r3)
			a=make_ast_node(ast_lt,b,c);
		else if(r4)
			a=make_ast_node(ast_le,b,c);
		else if(r5)
			a=make_ast_node(ast_gt,b,c);
		else if(r6)
			a=make_ast_node(ast_ge,b,c);
		return a;
	}
	AST* NParser::E2()
	{
		AST* a=E3();
		while(true){
			int r1,r2;
			r1=match(lx_plus);
			r2=match(lx_minus);
			if(!r1 && !r2)
				return a;
			AST* b=E3();
			if(r1)
				a=make_ast_node(ast_plus,a,b);
			else if(r2)
				a=make_ast_node(ast_minus,a,b);
		}
	}
	AST* NParser::E3()
	{
		AST* a=E4();
		while(true){
			int r1,r2;
			r1=match(lx_star);
			r2=match(lx_slash);
			if(!r1 && !r2)
				return a;
			AST* b=E4();
			if(r1)
				a=make_ast_node(ast_times,a,b);
			else if(r2)
				a=make_ast_node(ast_divide,a,b);
		}
	}
	AST* NParser::E4()
	{
		AST* a;
		int r1,r2;
		r1=match(lx_minus);
		r2=match(kw_not);
		AST* b=E5();
		expr_type(b);
		if(!r1 && !r2)
			return b;
		if(r1)
			a=make_ast_node(ast_uminus,b);
		else if(r2)
			a=make_ast_node(ast_not,b);
		return a;
	}
	AST* NParser::E5()
	{
		AST* a;
		int r;
		r=match(lx_lparen);
		if(!r)
			return E6();
		a=E2();
		expr_type(a);
		r=match(lx_rparen);
		if(!r)Error("right parenthesis[)] expected");
		return a;
	}
	AST* NParser::E6()
	{
		AST* a;
		int r;
		r = match(lx_identifier);
		if (r == 1)
		{
			char *name=new char[1024];
			strcpy(name,current->str_ptr);
			symbol_table_entry *ste;
			ste=st->GetSymbol(name);
			if(ste==NULL)Error("undeclared identifier");

			if(lookahead->type==lx_lparen)
			{
				ast_list *b = arg_list();
				a=make_ast_node(ast_call,ste,b);

				ast_list* p=root;
				while(p)
				{
					if(p->head->type==ast_routine_decl
						&& p->head->f.a_routine_decl.name==ste)break;
					p=p->tail;
				}
				ste_list* formals=p->head->f.a_routine_decl.formals;
				while(b || formals)
				{
					if(!b || !formals)
						Error("incompatible number of arguments for call");
					if(expr_type(b->head)!=formals->head->f.var.type)
						Error("type mismatch in call arguments");
					b=b->tail;
					formals=formals->tail;
				}
				return a;
			}
			else
			{
				a=make_ast_node(ast_var,ste);
				return a;
			}
		}
		r = match(lx_integer);
		if (r == 1){
			a=make_ast_node(ast_integer,current->value);
			return a;
		}
		r= match(lx_string);
		if ( r == 1) {
			a=make_ast_node(ast_string,current->str_ptr);
			return a;
		}
		r = match(kw_true);
		if(r==1){
			a=make_ast_node(ast_boolean,1);
			return a;
		}
		r = match(kw_false);
		if(r==1){
			a=make_ast_node(ast_boolean,0);
			return a;
		}
	};
j_type NParser::expr_type(AST* e)
{
	j_type l,r;
	if(e->type==ast_times||e->type==ast_divide||e->type==ast_minus){
		l=expr_type(e->f.a_binary_op.larg);
		r=expr_type(e->f.a_binary_op.rarg);
		if(l!=r)Error("type mismatch");
		if(l!=type_integer)Error("arithmetic operations support integers only");
		return type_integer;
	}
	else if(e->type==ast_plus){
		l=expr_type(e->f.a_binary_op.larg);
		r=expr_type(e->f.a_binary_op.rarg);
		if(l!=r)Error("type mismatch");
		if(l!=type_integer && l!=type_string)Error("[+] operation supports integer+integer or string+string");
		return l;
	}
	else if(e->type==ast_eq||e->type==ast_neq){
		l=expr_type(e->f.a_binary_op.larg);
		r=expr_type(e->f.a_binary_op.rarg);
		if(l!=r)Error("type mismatch");
		return type_boolean;
	}
	else if(e->type==ast_lt||e->type==ast_le
		||e->type==ast_gt||e->type==ast_ge){
		l=expr_type(e->f.a_binary_op.larg);
		r=expr_type(e->f.a_binary_op.rarg);
		if(l!=r)Error("type mismatch");
		if(l!=type_integer)Error("relational operations support integers only");
		return type_boolean;
	}
	else if(e->type==ast_and||e->type==ast_or
		||e->type==ast_cand||e->type==ast_cor){
		l=expr_type(e->f.a_binary_op.larg);
		r=expr_type(e->f.a_binary_op.rarg);
		if(l!=r)Error("type mismatch");
		if(l!=type_boolean)Error("relational conjunctions support booleans");
		return type_boolean;
	}
	else if(e->type==ast_not||e->type==ast_uminus){
		l=expr_type(e->f.a_unary_op.arg);
		if(l!=type_integer)Error("unary operations support integers only");
		return type_integer;
	}
	else if(e->type==ast_call){
		return e->f.a_call.callee->f.routine.result_type;
	}
	else if(e->type==ast_var){
		return e->f.a_var.var->f.var.type;
	}
	else if(e->type==ast_integer){
		return type_integer;
	}
	else if(e->type==ast_string){
		return type_string;
	}
	else if(e->type==ast_boolean){
		return type_boolean;
	}
}

