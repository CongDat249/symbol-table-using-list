#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Symbol
{
private:
    string name;
    int type;
    int scope;
    string value;
    Symbol *next;
    Symbol *prev;

public:
    Symbol(string, int, int);

    friend class SymbolTable;
};

class SymbolTable
{
private:
    Symbol *head;
    Symbol *tail;
    int cur_scope;

public:
    SymbolTable();
    ~SymbolTable();
    Symbol *search(string);
    void run(string filename);
    void insert(smatch);
    void assign(smatch);
    void begin();
    void end();
    void lookup(smatch);
    void print();
    void rprint();
};
#endif