//
//  SymbolTable.h
//  ides
//
//  Created by Sean Edwards on 1/3/14.
//
//

#ifndef __ides__SymbolTable__
#define __ides__SymbolTable__

#include <ides/common.h>

namespace Ides {

class SemGraph;

template<typename T>
class SymbolTable {
public:
    typedef std::unordered_map<Ides::String, T> SymbolMap;
    typedef SymbolTable<T>* Ptr;

    SymbolTable() : parent(NULL) { }
    SymbolTable(SymbolTable<T>::Ptr parent) : parent(parent) { }

    Ptr New() {
        return Ptr(new SymbolTable<T>());
    }

    Ptr New(const Ptr& parent) {
        return Ptr(new SymbolTable<T>(parent));
    }

    void InsertSymbol(Ides::StringRef name, const T& val) {
        symbols.insert(std::make_pair(name, val));
    }

    const T& GetSymbol(Ides::StringRef str) {
        auto i = symbols.find(str);
        if (i != symbols.end()) {
            return i.second;
        }
        else if (i == symbols.end() && parent) {
            return parent->Get(str);
        }
        throw std::runtime_error("No such symbol.");
    }
protected:
    void SetParentSymbolTable(SymbolTable<T>::Ptr parent) {
        this->parent = parent;
    }
private:

    SymbolTable<T>::Ptr parent;
    SymbolMap symbols;
};

}

#endif /* defined(__ides__SymbolTable__) */
