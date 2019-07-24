#include "scanner.h"

NScanner::NScanner(char* name)
{
	m_file =new NReader(name);
};

void NScanner::Error(char* error)
{
	m_file->Error(error);
}

TOKEN* NScanner::Scan()
{
	char c;
	TOKEN* t=new TOKEN();
	while(1){
		c = m_file->GetChar();
		// space
		if ( c == ' ' || c =='\t' || c==10 || c==13)
			continue;
		//end of file
		else if(c==0)
			return 0;
		// -
		else if (c == '-'){
			c = m_file->GetChar();
			//comment
			if( c == '-'){
				char a,b;
				a=m_file->GetChar();
				do{
					b=a;
					a=m_file->GetChar();
				}while((a!='-' || b!='-') && (a!='\n'));
				continue;
			}
			//// non comment
			else{
				if(c==' '){//operator
					t->type=lx_minus;
					return t;
				}
				else if(isdigit(c)){//negative
					int i=1;
					int isreal=0;
					char buf[20];
					buf[0]='-';
					buf[1]=c;
					while(1){
						c=m_file->GetChar();
						i++;
						if(isdigit(c)){
							buf[i]=c;
							continue;
						}
						else if(c=='.'){
							buf[i]=c;
							isreal=1;
							continue;
						}
						else
							break;
					}
					buf[i]=0;
					m_file->UngetChar();
					if(!isreal){
						t->value=atoi(buf);
						t->type=lx_integer;
					}
					else{
						t->float_value=atof(buf);
						t->type=lx_float;
					}
					return t;
				}
				else
					m_file->Error("negation only for literal numbers");
			}// non comment
		}//-
		// literal numbers
		else if(isdigit(c)){
			int i=0;
			int isreal=0;
			char buf[20];
			buf[0]=c;
			while(1){
				c=m_file->GetChar();
				i++;
				if(isdigit(c)){
					buf[i]=c;
					continue;
				}
				else if(c=='.'){
					isreal=1;
					buf[i]=c;
					continue;
				}
				else
					break;
			}
			m_file->UngetChar();
			buf[i]=0;
			if(!isreal){
				t->value=atoi(buf);
				t->type=lx_integer;
			}
			else{
				t->float_value=atof(buf);
				t->type=lx_float;
			}
			return t;
		}
		//identifier
		else if(isalpha(c)){
			int i=0;
			char* buf=new char[1024];
			do{
				buf[i]=c;
				c=m_file->GetChar();
				i++;
			}while(isalnum(c)||c=='_');
			m_file->UngetChar();
			buf[i]=0;
			for(i=0;i<keys;i++){
				if(!strcmp(keyword[i],buf)){
					t->type=key_type[i];
					return t;
				}
			}
			t->str_ptr=buf;
			t->type=lx_identifier;
			return t;
		}
		//string
		else if(c=='\"'){
			int i=0;
			char* buf=new char[1024];
			while((c=m_file->GetChar())!='\"'){
				buf[i]=c;
				i++;
			}
			buf[i]=0;
			t->str_ptr=buf;
			t->type=lx_string;
			return t;
		}
		//operators
		else if(c=='('){
			t->type=lx_lparen;
			return t;
		}
		else if(c==')'){
			t->type=lx_rparen;
			return t;
		}
		else if(c==':'){
			c=m_file->GetChar();
			if(c=='='){
				t->type=lx_colon_eq;
				return t;
			}
			else{
				m_file->UngetChar();
				t->type=lx_colon;
				return t;
			}
		}
		else if(c=='+'){
			t->type=lx_plus;
			return t;
		}
		else if(c=='*'){
			t->type=lx_star;
			return t;
		}
		else if(c=='/'){
			t->type=lx_slash;
			return t;
		}
		else if(c=='='){
			t->type=lx_eq;
			return t;

		}
		else if(c=='!'){
			c=m_file->GetChar();
			if(c!='=')//error;
				m_file->Error("undefined operator !, may use !=");
			t->type=lx_neq;
			return t;
		}
		else if(c=='<'){
			c=m_file->GetChar();
			if(c=='='){
				t->type=lx_le;
				return t;
			}
			else{
				m_file->UngetChar();
				t->type=lx_lt;
				return t;
			}
		}
		else if(c=='>'){
			c=m_file->GetChar();
			if(c=='='){
				t->type=lx_ge;
				return t;
			}
			else{
				m_file->UngetChar();
				t->type=lx_gt;
				return t;
			}
		}
		else if(c=='.'){
			t->type=lx_dot;
			return t;
		}
		else if(c==';'){
			t->type=lx_semicolon;
			return t;
		}
		else if(c=='['){
			t->type=lx_lbracket;
			return t;
		}
		else if(c==']'){
			t->type=lx_rbracket;
			return t;
		}
		else if(c==','){
			t->type=lx_comma;
			return t;
		}
		else m_file->Error("unacceptable symbol");
	}//end while
};
