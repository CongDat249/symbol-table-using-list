#include "SymbolTable.h"

Symbol::Symbol(string name, int type, int scope)
{
    this->name = name;
    this->type = type;
    this->scope = scope;
    this->value = "";
    this->next = nullptr;
    this->prev = nullptr;
}

SymbolTable::SymbolTable()
{
    this->head = nullptr;
    this->tail = nullptr;
    cur_scope = 0;
}

SymbolTable::~SymbolTable()
{
    Symbol *tmp = this->head;

    while (this->head != NULL)
    {
        tmp = head->next;
        delete head;
        head = tmp;
    }
}

void SymbolTable::insert(smatch m)
{
    // Create new symbol
    string name = m.str(2);
    int type = m.str(3) == "number" ? 0 : 1;
    int scope = this->cur_scope;
    Symbol *n = new Symbol(name, type, scope);

    // Symbol table is null
    if (this->head == nullptr)
    {
        this->head = n;
        this->tail = n;
        return;
    }

    // Check redeclared
    Symbol *tmp = this->tail;
    while (tmp != nullptr)
    {
        if (tmp->name == "B" && tmp->scope == scope)
            break;

        if (tmp->name == name && tmp->scope == scope)
        {
            delete n;
            throw Redeclared(m.str(0));
        }

        tmp = tmp->prev;
    }

    // Add new symbol
    this->tail->next = n;
    n->prev = this->tail;
    this->tail = n;
}

Symbol *SymbolTable::search(string name)
{
    int scope = this->cur_scope;
    Symbol *tmp = this->tail;
    while (tmp != nullptr)
    {
        if (tmp->name == "B" && tmp->scope == scope)
            scope--;

        if (tmp->name == name && tmp->scope == scope)
            break;

        tmp = tmp->prev;
    }
    return tmp;
}

void SymbolTable::assign(smatch m)
{
    // Regex
    regex number_expr("\\d+");
    regex string_expr("\'[A-Za-z0-9 ]*\'");
    regex var_expr("[a-z][\\w]*");

    string name = m.str(2);
    string value = m.str(3);

    // Check undeclared
    Symbol *tmp = this->search(name);

    // Assign value
    if (tmp == nullptr)
    {
        throw Undeclared(m.str(0));
    }

    if (!(regex_match(value, number_expr) ||
          regex_match(value, string_expr) ||
          regex_match(value, var_expr)))
    {
        throw InvalidInstruction(m.str(0));
    }

    if ((tmp->type == 0 && regex_match(value, number_expr)) ||
        (tmp->type == 1 && regex_match(value, string_expr)))
    {
        tmp->value = value;
        return;
    }

    if (regex_match(value, var_expr))
    {
        Symbol *sys = this->search(value);
        if (sys == nullptr)
        {
            throw Undeclared(m.str(0));
        }

        if (tmp->type == sys->type)
        {
            tmp->value = sys->value;
            return;
        }
    }

    throw TypeMismatch(m.str(0));
}

void SymbolTable::begin()
{
    this->cur_scope += 1;
    Symbol *n = new Symbol("B", 2, this->cur_scope);

    if (this->head == nullptr)
    {
        this->head = n;
        this->tail = n;
    }
    else
    {
        this->tail->next = n;
        n->prev = this->tail;
        this->tail = n;
    }
}

void SymbolTable::end()
{
    if (cur_scope <= 0)
    {
        throw UnknownBlock();
    }
    this->cur_scope -= 1;
}

void SymbolTable::lookup(smatch m)
{
    string name = m.str(2);
    Symbol *sys = search(name);

    if (sys == nullptr)
    {
        throw Undeclared(m.str(0));
    }

    cout << sys->scope << endl;
}

void SymbolTable::print()
{
    int scope = this->cur_scope;
    string res = "";
    Symbol *tmp = this->tail;
    while (tmp != nullptr)
    {
        if (tmp->name == "B" && tmp->scope == scope)
            scope--;

        else if (tmp->scope == scope && tmp == this->search(tmp->name))
        {
            res = tmp->name + "//" + to_string(tmp->scope) + " " + res;
        }

        tmp = tmp->prev;
    }

    if (res.length() != 0)
        cout << res.substr(0, res.length() - 1) << endl;
}

void SymbolTable::rprint()
{
    int scope = this->cur_scope;
    string res = "";
    Symbol *tmp = this->tail;
    while (tmp != nullptr)
    {
        if (tmp->name == "B" && tmp->scope == scope)
            scope--;

        else if (tmp->scope == scope && tmp == this->search(tmp->name))
        {
            res += tmp->name + "//" + to_string(tmp->scope) + " ";
        }

        tmp = tmp->prev;
    }

    if (res.length() != 0)
        cout << res.substr(0, res.length() - 1) << endl;
}

void SymbolTable::run(string filename)
{
    // Regex
    smatch m;
    regex insert_expr("(INSERT) ([a-z][\\w]*) (number|string)");
    regex assign_expr("(ASSIGN) ([a-z][\\w]*) ([a-z0-9\\'][\\w\' ]*)");
    regex begin_expr("(BEGIN)");
    regex end_expr("(END)");
    regex lookup_expr("(LOOKUP) ([a-z][\\w]*)");
    regex print_expr("(PRINT)");
    regex rprint_expr("(RPRINT)");

    // Read file
    string s;
    ifstream file(filename);
    while (getline(file, s))
    {
        // Insert command
        if (regex_match(s, m, insert_expr))
        {
            this->insert(m);
            cout << "success\n";
        }

        // Assign command
        else if (regex_match(s, m, assign_expr))
        {
            this->assign(m);
            cout << "success\n";
        }

        // Begin command
        else if (regex_match(s, begin_expr))
        {
            this->begin();
        }

        // End command
        else if (regex_match(s, end_expr))
        {
            this->end();
        }

        // Lookup command
        else if (regex_match(s, m, lookup_expr))
        {
            this->lookup(m);
        }

        // Print
        else if (regex_match(s, print_expr))
        {
            this->print();
        }

        // Rprint
        else if (regex_match(s, rprint_expr))
        {
            this->rprint();
        }

        // Invalid instruction
        else
        {
            throw InvalidInstruction(s);
        }
    }

    if (this->cur_scope > 0)
    {
        throw UnclosedBlock(this->cur_scope);
    }
}