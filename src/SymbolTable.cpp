#include "symboltable.h"



int NSymbolTable::hash(char* str)
{
	int math=0;
	int i;
	int j=strlen(str);
	for(i=0;i<j;i++)
	{
		math^=str[i]*i;
	}
	return math%SLOTSNO;
}
NSymbolTable::NSymbolTable()
{
	next=NULL;
	slots=new SymbolTableEntry*[SLOTSNO];
	for(int i=0;i<SLOTSNO;i++)
		slots[i]=NULL;
	number_entries=0;
	number_probes=SLOTSNO;
	number_hits=0;
	max_search_dist=0;
	fold_case=0;
}
NSymbolTable::NSymbolTable(int fold_case_flag)
{
	next=NULL;
	fold_case=fold_case_flag;
	slots=new SymbolTableEntry*[SLOTSNO];
	for(int i=0;i<SLOTSNO;i++)
		slots[i]=NULL;
	number_entries=0;
	number_probes=SLOTSNO;
	number_hits=0;
	max_search_dist=0;
}
SymbolTableEntry* NSymbolTable::PutSymbol(char* str)
{
	if(fold_case)
	{
		for(int i=0;i<strlen(str);i++)
			str[i]=tolower(str[i]);
	}
	int index=hash(str);
	SymbolTableEntry* p;
	p=slots[index];
	while(p)
	{
		if(strcmp(p->name,str)==0)
			return NULL;
		p=p->next;
	}
	number_entries++;
	SymbolTableEntry* entry;
	entry=new SymbolTableEntry;
	entry->name=str;
	entry->next=NULL;
	if(slots[index]==NULL)
	{
		if(max_search_dist==0)
			max_search_dist=1;
		number_hits++;
		slots[index]=entry;
		return entry;
	}
	else
	{
		int max=0;
		p=slots[index];
		max++;
		while(p->next)
		{
			max++;		
			p=p->next;
		}
		max++;
		if(max_search_dist<max)
			max_search_dist=max;
		p->next=entry;
		return entry;
	}
}
void NSymbolTable::clear_symbol_table()
{
	int index;
	SymbolTableEntry* p,*p1;
	for(index=0;index<SLOTSNO;index++)
	{
		p=slots[index];
		while(p)
		{
			p1=p->next;
			delete p;
			p=p1;
		}
		slots[index]=NULL;
	}
	number_entries=0;
	number_hits=0;
	max_search_dist=0;
}
void NSymbolTable::print_symbol_stats()
{
	printf("\nhash table status:");
	printf("\n#entries=%d",number_entries);	
	printf("\n#lists=%d",number_probes);	
	printf("\n#free lists=%d",number_probes-number_hits);
	printf("\n#max entries searched=%d",max_search_dist);
	
}
SymbolTableEntry* NSymbolTable::GetSymbol(char* str)
{
	if(fold_case)
	{
		for(int i=0;i<strlen(str);i++)
			str[i]=tolower(str[i]);
	}
	int index=hash(str);
	SymbolTableEntry* p;
	p=slots[index];
	while(p)
	{
		if(strcmp(p->name,str)==0)
			return p;
		p=p->next;
	}
	return NULL;
}

NStack::NStack()
{
	fold_case=0;
	top=new NSymbolTable();
}
NStack::NStack(int fold_case_flag)
{
	fold_case=fold_case_flag;
	top=new NSymbolTable(fold_case_flag);
}
NStack::~NStack()
{
	NSymbolTable* p;
	while(top)
	{
		p=top->next;
		top->clear_symbol_table();
		delete top;
		top=p;
	}
}
SymbolTableEntry* NStack::PutSymbol(char* name)
{
	return top->PutSymbol(name);
}
SymbolTableEntry* NStack::GetSymbol(char* name)
{
	NSymbolTable* p=top;
	SymbolTableEntry* r;
	while(true)
	{
		if(p==NULL)return NULL;
		r=p->GetSymbol(name);
		if(r!=NULL)return r;
		p=p->next;
	}
}
void NStack::AddLevel()
{
	NSymbolTable* p;
	p=top;
	if(fold_case)
		top=new NSymbolTable(1);
	else
		top=new NSymbolTable();
	top->next=p;
}
void NStack::RemoveLevel()
{
	NSymbolTable* p;
	p=top->next;
	top=p;
}