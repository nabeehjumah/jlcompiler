#ifndef symboltable_h
#define symboltable_h

#include "project.h"

#define SLOTSNO 10


class NSymbolTable
{
private:
	SymbolTableEntry **slots;
	int fold_case;

	int number_entries;
	int number_probes;
	int number_hits;
	int max_search_dist;
public:
	NSymbolTable* next;
	NSymbolTable();
	NSymbolTable(int);
	void clear_symbol_table();
	SymbolTableEntry* GetSymbol(char*);
	SymbolTableEntry* PutSymbol(char*);
	void print_symbol_stats();
	int hash(char*);
};

class NStack{
public:
	NSymbolTable* top;
	int fold_case;
	NStack();
	NStack(int);
	~NStack();
	SymbolTableEntry* PutSymbol(char*);
	SymbolTableEntry* GetSymbol(char*);
	void AddLevel();
	void RemoveLevel();
};


#endif