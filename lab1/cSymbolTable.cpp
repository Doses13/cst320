#include "cSymbolTable.h"

cSymbolTable::cSymbolTable()
{
}

symbolTable_t *cSymbolTable::IncreaseScope()
{
    return nullptr;
}

symbolTable_t *cSymbolTable::DecreaseScope()
{
    return nullptr;
}

void cSymbolTable::Insert(cSymbol *sym)
{
}

cSymbol *cSymbolTable::Find(string name)
{
    return nullptr;
}

cSymbol *cSymbolTable::FindLocal(string name)
{
    return nullptr;
}
