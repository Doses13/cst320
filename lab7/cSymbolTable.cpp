#include "cSymbolTable.h"

cSymbolTable g_symbolTable;

cSymbolTable::cSymbolTable()
{
}

symbolTable_t *cSymbolTable::IncreaseScope()
{
    symbolTable_t* newScope = new symbolTable_t();
    m_scopes.push_back(newScope);
    return newScope; // return value is ignored, but required by header
}

symbolTable_t *cSymbolTable::DecreaseScope()
{
     if (m_scopes.empty())
        return nullptr;

    symbolTable_t* top = m_scopes.back();
    m_scopes.pop_back();

    return top;
}

void cSymbolTable::Insert(cSymbol *sym)
{
    if (m_scopes.empty())
        return;

    symbolTable_t* currentScope = m_scopes.back();
    currentScope->insert({sym->GetName(), sym});
}

cSymbol *cSymbolTable::Find(string name)
{
    for (auto scopeIt = m_scopes.rbegin(); scopeIt != m_scopes.rend(); ++scopeIt)
    {
        symbolTable_t* scope = *scopeIt;
        auto it = scope->find(name);
        if (it != scope->end())
        {
            return it->second; // found the symbol
        }
    }
    return nullptr; // symbol not found in any scope
}

cSymbol *cSymbolTable::FindLocal(string name)
{
    if (m_scopes.empty())
        return nullptr;

    symbolTable_t* currentScope = m_scopes.back();
    auto it = currentScope->find(name);
    if (it != currentScope->end())
    {
        return it->second; // found the symbol
    }
    return nullptr; // symbol not found in the local scope
}
